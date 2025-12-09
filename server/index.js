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

// Define API Configuration with defaults for robustness
const PLATO_API_URL = process.env.PLATO_API_URL || 'https://one-api.bltcy.top/v1';
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
  contentSecurityPolicy: false // Disable CSP for now to avoid issues with inline scripts/styles in simple HTML
}));
app.use(cors());
app.use(express.json({ limit: '50mb' }));
app.use(express.urlencoded({ extended: true, limit: '50mb' }));

// Rate Limiting
const limiter = rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100 // limit each IP to 100 requests per windowMs
});
app.use('/api/', limiter);

// Static files (Frontend)
app.use(express.static(path.join(__dirname, '../front')));
app.use('/image', express.static(path.join(__dirname, '../image')));

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

// Route: Optimize Prompt
app.post('/api/optimize', async (req, res) => {
    try {
        const { prompt } = req.body;
        
        if (!prompt) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        const response = await axios.post(`${PLATO_API_URL}/chat/completions`, {
            model: 'nano-banana-2-4k',
            messages: [
                {
                    role: 'system',
                    content: 'You are a helpful AI assistant. Your task is to optimize the user\'s image generation prompt. Make it more descriptive, artistic, and suitable for high-quality image generation. Return ONLY the optimized prompt.'
                },
                {
                    role: 'user',
                    content: prompt
                }
            ]
        }, {
            headers: {
                'Authorization': `Bearer ${PLATO_API_KEY}`,
                'Content-Type': 'application/json'
            }
        });

        const optimizedPrompt = response.data.choices[0].message.content;
        res.json({ optimizedPrompt });

    } catch (error) {
        console.error('Error optimizing prompt:', error.response ? error.response.data : error.message);
        res.status(500).json({ error: 'Failed to optimize prompt' });
    }
});

// Serve generated images
app.use('/generated_images', express.static(path.join(__dirname, 'generated_images')));

// Route: Generate Image
app.post('/api/generate', upload.single('image'), async (req, res) => {
    try {
        const { prompt } = req.body;
        const imageFile = req.file;

        if (!imageFile && !prompt) {
            return res.status(400).json({ error: 'Image or Prompt is required' });
        }

        // Try using Gemini 3 Pro for everything first (as user requested)
        // Some OneAPI providers map "draw..." requests in chat completions to image generation
        // OR return a markdown image.
        
        let generationPrompt = prompt;
        let base64Image = null;

        if (imageFile) {
            base64Image = bufferToBase64(imageFile.buffer, imageFile.mimetype);
        }

        console.log("Starting generation with prompt:", prompt);

        try {
            // Attempt 1: Direct Image Generation with nano-banana-2-4k
            // Skipping Chat Completion since it caused "bad_response_body" error
            
            let imageUrl = null;
            let content = prompt; // Default description is the prompt itself

            try {
                console.log("Trying /images/generations with nano-banana-2-4k...");
                const imageResponse = await axios.post(`${PLATO_API_URL}/images/generations`, {
                    model: 'nano-banana-2-4k', 
                    prompt: prompt,
                    n: 1,
                    size: "1024x1024"
                }, {
                    headers: {
                        'Authorization': `Bearer ${PLATO_API_KEY}`,
                        'Content-Type': 'application/json'
                    }
                });
                
                if (imageResponse.data && imageResponse.data.data && imageResponse.data.data.length > 0) {
                    imageUrl = imageResponse.data.data[0].url;
                    console.log("nano-banana-2-4k generation successful:", imageUrl);
                }
            } catch (imgGenError) {
                console.log("Direct image generation with nano-banana-2-4k failed:", imgGenError.response ? imgGenError.response.data : imgGenError.message);
                
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
                        }
                    });
                    imageUrl = fallbackResponse.data.data[0].url;
                } catch (fallbackError) {
                    console.error("Fallback DALL-E 3 failed:", fallbackError.response ? fallbackError.response.data : fallbackError.message);
                    throw new Error("All image generation attempts failed.");
                }
            }

            if (imageUrl) {
                // Try to DOWNLOAD and SAVE the image
                try {
                    const imageFileName = `gen-${Date.now()}.png`;
                    const localImagePath = path.join(generatedImagesDir, imageFileName);
                    
                    const writer = fs.createWriteStream(localImagePath);
                    const imgStreamResponse = await axios({
                        url: imageUrl,
                        method: 'GET',
                        responseType: 'stream',
                        timeout: 30000 // 30s timeout for download
                    });

                    imgStreamResponse.data.pipe(writer);

                    await new Promise((resolve, reject) => {
                        writer.on('finish', resolve);
                        writer.on('error', reject);
                    });

                    // Return relative path so it works on any domain/deployment
                    const localUrl = `/generated_images/${imageFileName}`;
                    
                    res.json({
                        imageUrl: localUrl,
                        description: content // Return the prompt as description
                    });
                } catch (downloadError) {
                    console.error("Failed to download image, returning remote URL:", downloadError.message);
                    // Fallback to remote URL if download fails
                    res.json({
                        imageUrl: imageUrl,
                        description: content
                    });
                }

            } else {
                 res.json({
                    imageUrl: null,
                    description: content,
                    message: "Could not generate image URL."
                });
            }

        } catch (innerError) {
             console.error('Generation Pipeline Error:', innerError.response ? innerError.response.data : innerError.message);
             res.status(500).json({ error: 'Generation failed: ' + innerError.message });
        }

    } catch (error) {
        console.error('Server Error:', error.response ? error.response.data : error.message);
        res.status(500).json({ error: 'Failed to process request' });
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
