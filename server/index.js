const express = require('express');
const cors = require('cors');
const multer = require('multer');
const axios = require('axios');
const FormData = require('form-data');
const fs = require('fs');
const path = require('path');
const morgan = require('morgan');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
require('dotenv').config({ path: path.join(__dirname, '.env') });

const app = express();
const port = process.env.PORT || 3011;

// Trust proxy is required when running behind Nginx/Reverse Proxy
// This fixes the ERR_ERL_UNEXPECTED_X_FORWARDED_FOR error
app.set('trust proxy', 1);

// Define API Configuration with defaults for robustness
const PLATO_API_URL = process.env.PLATO_API_URL || 'https://api.bltcy.ai/v1';
const PLATO_API_KEY = process.env.PLATO_API_KEY || 'sk-VyK6G645s0AIgIsKp2cWkXVz3Z4Srlp3FW8k9YQ7zKQuIdjv';

console.log('----------------------------------------');
console.log('Server Configuration:');
console.log(`PORT: ${port}`);
console.log(`PLATO_API_URL: ${PLATO_API_URL}`);
console.log(`PLATO_API_KEY: ${PLATO_API_KEY ? 'Set (starts with ' + PLATO_API_KEY.substring(0, 3) + ')' : 'Not Set'}`);
console.log('----------------------------------------');

// Middleware
app.use(morgan('dev')); // Logging
app.use(helmet({
  contentSecurityPolicy: false // Disable CSP for now
}));
app.use(cors());
app.use(express.json({ limit: '50mb' }));
app.use(express.urlencoded({ extended: true, limit: '50mb' }));

// Disable caching for all requests during development
app.use((req, res, next) => {
    res.set('Cache-Control', 'no-store, no-cache, must-revalidate, private');
    res.set('Pragma', 'no-cache');
    res.set('Expires', '0');
    next();
});

// Rate Limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100 // limit each IP to 100 requests per windowMs
});
app.use('/api/', limiter);

// Static files (Frontend)
app.use(express.static(path.join(__dirname, '../front')));
app.use('/image', express.static(path.join(__dirname, '../image')));
app.use('/music', express.static(path.join(__dirname, '../music')));

// Multer setup for file uploads
const upload = multer({
    storage: multer.memoryStorage(),
    limits: { fileSize: 10 * 1024 * 1024 } // 10MB limit
});

// Ensure generated_images directory exists
const generatedImagesDir = path.join(__dirname, 'generated_images');
if (!fs.existsSync(generatedImagesDir)) {
    fs.mkdirSync(generatedImagesDir, { recursive: true });
}

// Helper to encode image to base64
const bufferToBase64 = (buffer, mimetype) => {
    return `data:${mimetype};base64,${buffer.toString('base64')}`;
};

// Serve generated images
app.use('/generated_images', express.static(path.join(__dirname, 'generated_images')));

// Route: Proxy Download
app.get('/api/proxy-download', async (req, res) => {
    try {
        const { url } = req.query;
        if (!url) {
            return res.status(400).send('Missing url parameter');
        }

        const response = await axios({
            url: url,
            method: 'GET',
            responseType: 'stream',
            timeout: 60000
        });

        res.setHeader('Content-Disposition', `attachment; filename="generated-image-${Date.now()}.png"`);
        res.setHeader('Content-Type', response.headers['content-type']);
        
        response.data.pipe(res);
    } catch (error) {
        console.error('Proxy download error:', error.message);
        res.status(500).send('Failed to download image');
    }
});

// Route: Text to Image (txt2img)
app.post('/api/generate-text', upload.none(), async (req, res) => {
    try {
        const { prompt, size } = req.body;

        if (!prompt) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        console.log("Starting Text-to-Image generation with prompt:", prompt);
        console.log("Requested size:", size || "Default (512x512)");

        let imageUrl = null;
        let content = prompt;

        try {
            console.log("Trying /images/generations with nano-banana-2-2k...");
            
            // Explicitly set timeout for API request to avoid 502/504 from upstream
            const imageResponse = await axios.post(`${PLATO_API_URL}/images/generations`, {
                model: 'nano-banana-2-2k', 
                prompt: prompt,
                n: 1,
                size: size || "512x512"
            }, {
                headers: {
                    'Authorization': `Bearer ${PLATO_API_KEY}`,
                    'Content-Type': 'application/json'
                },
                timeout: 180000, // Reduced to 180s to fail faster if stuck
                maxBodyLength: Infinity
            });
            
            if (imageResponse.data && imageResponse.data.data && imageResponse.data.data.length > 0) {
                imageUrl = imageResponse.data.data[0].url;
                console.log("nano-banana-2-2k generation successful:", imageUrl);
            }
        } catch (imgGenError) {
            console.log("Text-to-Image nano-banana-2-2k failed:", JSON.stringify(imgGenError.response ? imgGenError.response.data : imgGenError.message, null, 2));
            
            // Fallback to DALL-E 3
            console.log("Falling back to DALL-E 3...");
            try {
                const fallbackResponse = await axios.post(`${PLATO_API_URL}/images/generations`, {
                    model: 'dall-e-3',
                    prompt: prompt,
                    n: 1,
                    size: "1024x1024"
                }, {
                    headers: {
                        'Authorization': `Bearer ${PLATO_API_KEY}`,
                        'Content-Type': 'application/json'
                    },
                    timeout: 180000
                });
                imageUrl = fallbackResponse.data.data[0].url;
            } catch (fallbackError) {
                console.error("Fallback DALL-E 3 failed:", fallbackError.response ? fallbackError.response.data : fallbackError.message);
                throw new Error("All image generation attempts failed.");
            }
        }

        if (imageUrl) {
            // Respond immediately with remote URL
            res.json({
                imageUrl: imageUrl,
                description: content
            });
        } else {
            throw new Error("No image URL received from API");
        }

    } catch (error) {
        console.error("Text-to-Image Error:", error.message);
        res.status(500).json({ error: error.message });
    }
});

// Route: Image to Image (img2img)
app.post('/api/generate-image', upload.single('image'), async (req, res) => {
    try {
        const { prompt, size } = req.body;
        const imageFile = req.file;

        if (!imageFile) {
            return res.status(400).json({ error: 'Image is required for Image-to-Image generation' });
        }

        console.log("Starting Image-to-Image generation with prompt:", prompt || "(No prompt provided)");
        console.log("Requested size:", size || "Default (512x512)");
        
        // Convert image to base64
        // Many OneAPI providers expect just the base64 string without the data URI prefix for 'image' field,
        // OR a full data URI. We will try full data URI first as it's more standard for 'image_url' or some adapters.
        // However, for 'image' field in some SD wrappers, it might want raw base64.
        // Let's try standard Data URI first.
        const base64Image = bufferToBase64(imageFile.buffer, imageFile.mimetype);
        
        // Also prepare raw base64 just in case
        // const rawBase64 = imageFile.buffer.toString('base64');

        let imageUrl = null;

        try {
            console.log("Trying /images/edits with nano-banana-2-2k (Img2Img)...");
            
            // Construct FormData payload for Img2Img (Standard OpenAI /images/edits format)
            const form = new FormData();
            form.append('model', 'nano-banana-2-2k');
            form.append('prompt', prompt || "optimize image");
            form.append('n', 1);
            form.append('size', size || "512x512");
            form.append('image', imageFile.buffer, {
                filename: 'input.png',
                contentType: imageFile.mimetype
            });
            
            // Add a transparent mask (1x1 PNG) to satisfy OpenAI /edits requirement
            // Some providers require a mask for this endpoint.
            // A fully transparent mask implies "edit the whole image" (or whatever the model default is).
            const maskBuffer = Buffer.from('iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=', 'base64');
            form.append('mask', maskBuffer, {
                filename: 'mask.png',
                contentType: 'image/png'
            });

            // We need to get headers from the form-data instance
            const formHeaders = form.getHeaders();
            
            console.log(`Sending Image-to-Image request. Image size: ${imageFile.size} bytes, MimeType: ${imageFile.mimetype}`);

            const imageResponse = await axios.post(`${PLATO_API_URL}/images/edits`, form, {
                headers: {
                    'Authorization': `Bearer ${PLATO_API_KEY}`,
                    ...formHeaders
                },
                timeout: 300000, // 300s timeout for image generation
                maxBodyLength: Infinity,
                maxContentLength: Infinity
            });
            
            if (imageResponse.data && imageResponse.data.data && imageResponse.data.data.length > 0) {
                imageUrl = imageResponse.data.data[0].url;
                console.log("nano-banana-2-2k Img2Img successful:", imageUrl);
            }
        } catch (imgGenError) {
            console.log("Image-to-Image nano-banana-2-2k failed:", imgGenError.response ? imgGenError.response.data : imgGenError.message);
            
            // If first attempt fails, maybe try with 'images' array or just raw base64? 
            // Or maybe it is an OpenAI 'edits' endpoint wrapper?
            // For now, let's just error out or try DALL-E 2/3 variations if supported (DALL-E 3 doesn't support img2img/variations via API yet commonly)
            // Let's throw error to let user know config might be wrong
             throw new Error("Image-to-Image generation failed. The model might not support the provided format or image input.");
        }

        if (imageUrl) {
             // Respond immediately with remote URL
             res.json({
                 imageUrl: imageUrl,
                 description: prompt
             });
        } else {
             throw new Error("No image URL received from API");
        }

    } catch (error) {
        console.error("Image-to-Image Error:", error.message);
        res.status(500).json({ error: error.message });
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
