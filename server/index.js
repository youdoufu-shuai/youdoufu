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
  max: 2000 // limit each IP to 2000 requests per windowMs (increased for polling)
});
app.use('/api/', limiter);

// Static files (Frontend)
app.use(express.static(path.join(__dirname, '../front')));
app.use('/image', express.static(path.join(__dirname, '../image')));
app.use('/music', express.static(path.join(__dirname, '../music')));

// Multer setup for file uploads
const upload = multer({
    storage: multer.memoryStorage(),
    limits: { fileSize: 50 * 1024 * 1024 } // 50MB limit
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

// Route: Text to Image (txt2img) - Async Task Version
app.post('/api/generate-text', upload.none(), (req, res) => {
    try {
        const { prompt, size } = req.body;

        if (!prompt) {
            return res.status(400).json({ error: 'Prompt is required' });
        }

        const taskId = 'task_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);

        console.log(`[${taskId}] Starting Async Txt2Img Task`);
        console.log(`[${taskId}] Prompt:`, prompt);

        // Initialize Task
        tasks.set(taskId, { status: 'processing', timestamp: Date.now() });

        // Respond immediately
        res.json({ taskId, status: 'processing', message: 'Task started successfully' });

        // Start background processing
        (async () => {
            try {
                let imageUrl = null;
                console.log(`[${taskId}] Trying /images/generations with nano-banana-2-2k...`);

                try {
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
                        timeout: 180000,
                        maxBodyLength: Infinity
                    });

                    if (imageResponse.data && imageResponse.data.data && imageResponse.data.data.length > 0) {
                        imageUrl = imageResponse.data.data[0].url;
                        console.log(`[${taskId}] Success (nano-banana-2-2k):`, imageUrl);
                    }
                } catch (primaryError) {
                    console.log(`[${taskId}] Primary model failed:`, primaryError.message);
                    throw primaryError; // Re-throw to trigger fallback
                }

                if (imageUrl) {
                    tasks.set(taskId, {
                        status: 'completed',
                        imageUrl: imageUrl,
                        description: prompt,
                        timestamp: Date.now()
                    });
                } else {
                    throw new Error("No image URL from primary model");
                }

            } catch (err) {
                // Fallback to DALL-E 3
                console.log(`[${taskId}] Falling back to DALL-E 3...`);
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
                    
                    if (fallbackResponse.data && fallbackResponse.data.data && fallbackResponse.data.data.length > 0) {
                        const fallbackUrl = fallbackResponse.data.data[0].url;
                        console.log(`[${taskId}] Success (DALL-E 3):`, fallbackUrl);
                        
                        tasks.set(taskId, {
                            status: 'completed',
                            imageUrl: fallbackUrl,
                            description: prompt,
                            timestamp: Date.now()
                        });
                    } else {
                        throw new Error("No image URL from fallback model");
                    }

                } catch (fallbackError) {
                    console.error(`[${taskId}] Fallback failed:`, fallbackError.message);
                    tasks.set(taskId, {
                        status: 'failed',
                        error: "All image generation attempts failed.",
                        timestamp: Date.now()
                    });
                }
            }
        })();

    } catch (error) {
        console.error("Txt2Img Setup Error:", error.message);
        if (!res.headersSent) {
            res.status(500).json({ error: error.message });
        }
    }
});

// Task Queue for Async Processing (Avoids Nginx 504/502 Timeouts)
const tasks = new Map();

// Cleanup old tasks every hour
setInterval(() => {
    const now = Date.now();
    for (const [id, task] of tasks.entries()) {
        if (now - (task.timestamp || 0) > 3600000) { // 1 hour
            tasks.delete(id);
        }
    }
}, 3600000);

// Route: Check Task Status
app.get('/api/task-status/:id', (req, res) => {
    const taskId = req.params.id;
    const task = tasks.get(taskId);
    if (!task) {
        return res.status(404).json({ error: 'Task not found or expired' });
    }
    res.json(task);
});

// Route: Image to Image (img2img) - Async Task Version
app.post('/api/generate-image', upload.single('image'), (req, res) => {
    try {
        const { prompt, size } = req.body;
        const imageFile = req.file;

        if (!imageFile) {
            return res.status(400).json({ error: 'Image is required for Image-to-Image generation' });
        }

        // Generate Task ID
        const taskId = 'task_' + Date.now() + '_' + Math.random().toString(36).substr(2, 9);
        
        console.log(`[${taskId}] Starting Async Img2Img Task`);
        console.log(`[${taskId}] Prompt:`, prompt || "(No prompt)");

        // Initialize Task
        tasks.set(taskId, { status: 'processing', timestamp: Date.now() });

        // Respond immediately to client with Task ID
        res.json({ taskId, status: 'processing', message: 'Task started successfully' });

        // Start background processing (Detached from HTTP response)
        (async () => {
            try {
                // Convert image to base64 (Data URI)
                const base64Image = bufferToBase64(imageFile.buffer, imageFile.mimetype);
                
                let imageUrl = null;

                console.log(`[${taskId}] Trying /images/edits with nano-banana-2-2k...`);
                
                // Construct FormData
                const form = new FormData();
                form.append('model', 'nano-banana-2-2k');
                form.append('prompt', prompt || "optimize image");
                form.append('n', 1);
                form.append('size', size || "512x512");
                form.append('image', imageFile.buffer, {
                    filename: 'input.png',
                    contentType: imageFile.mimetype
                });
                
                // Add transparent mask
                const maskBuffer = Buffer.from('iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII=', 'base64');
                form.append('mask', maskBuffer, {
                    filename: 'mask.png',
                    contentType: 'image/png'
                });

                const formHeaders = form.getHeaders();
                
                // Request to AI Provider
                const imageResponse = await axios.post(`${PLATO_API_URL}/images/edits`, form, {
                    headers: {
                        'Authorization': `Bearer ${PLATO_API_KEY}`,
                        ...formHeaders
                    },
                    timeout: 300000, // 300s timeout
                    maxBodyLength: Infinity,
                    maxContentLength: Infinity
                });
                
                if (imageResponse.data && imageResponse.data.data && imageResponse.data.data.length > 0) {
                    imageUrl = imageResponse.data.data[0].url;
                    console.log(`[${taskId}] Success:`, imageUrl);
                }

                if (imageUrl) {
                    tasks.set(taskId, { 
                        status: 'completed', 
                        imageUrl: imageUrl, 
                        description: prompt,
                        timestamp: Date.now()
                    });
                } else {
                    throw new Error("No image URL received from API");
                }

            } catch (err) {
                console.error(`[${taskId}] Failed:`, err.message);
                tasks.set(taskId, { 
                    status: 'failed', 
                    error: err.response ? JSON.stringify(err.response.data) : err.message,
                    timestamp: Date.now()
                });
            }
        })();

    } catch (error) {
        console.error("Img2Img Setup Error:", error.message);
        if (!res.headersSent) {
            res.status(500).json({ error: error.message });
        }
    }
});

app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
});
