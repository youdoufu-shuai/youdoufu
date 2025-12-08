
// --- CONFIG & STATE ---
const API_BASE = 'http://localhost:3011/api';

// --- DOM ELEMENTS ---
// Common
const startBtn = document.getElementById('start-btn');
const introOverlay = document.getElementById('intro-overlay');
const statusText = document.getElementById('status-text');
const warpOverlay = document.getElementById('warp-overlay');

// Sections
const homeSection = document.getElementById('home-section');
const aiToolSection = document.getElementById('ai-tool-section');

// Navigation
const toToolboxBtn = document.getElementById('to-toolbox');
const backHomeBtn = document.getElementById('back-home-btn');

// AI Tool
const imageUpload = document.getElementById('image-upload');
const preview = document.getElementById('preview');
const promptInput = document.getElementById('prompt-input');
const optimizeBtn = document.getElementById('optimize-btn');
const generateBtn = document.getElementById('generate-btn');
const loading = document.getElementById('loading');
const resultImage = document.getElementById('result-image');
const resultText = document.getElementById('result-text');
const downloadLink = document.getElementById('download-link');

// Audio
const audio = document.getElementById('bg-music');
const musicBtn = document.getElementById('music-toggle');
const visualizerCanvas = document.getElementById('audio-visualizer');
const vCtx = visualizerCanvas ? visualizerCanvas.getContext('2d') : null;

// Particles
const particleCanvas = document.getElementById('particles-js');
const ctx = particleCanvas ? particleCanvas.getContext('2d') : null;

// --- NAVIGATION LOGIC ---
function showSection(section) {
    // Hide all sections
    if(homeSection) {
        homeSection.classList.remove('visible');
        homeSection.style.display = 'none';
    }
    if(aiToolSection) {
        aiToolSection.classList.remove('visible');
        aiToolSection.style.display = 'none';
    }

    // Show target
    if(section) {
        section.style.display = 'block';
        // Small delay to allow display:block to apply before adding visible class for transition
        setTimeout(() => {
            section.classList.add('visible');
        }, 50);
    }
}

if (toToolboxBtn) {
    toToolboxBtn.addEventListener('click', (e) => {
        e.preventDefault();
        showSection(aiToolSection);
    });
}

if (backHomeBtn) {
    backHomeBtn.addEventListener('click', () => {
        showSection(homeSection);
    });
}


// --- AUDIO SYSTEM ---
let audioCtx, analyser, dataArray, bufferLength;
let isMusicPlaying = false;
let isAudioInitialized = false;

function initAudio() {
    if (isAudioInitialized || !audio) return;
    try {
        audioCtx = new (window.AudioContext || window.webkitAudioContext)();
        analyser = audioCtx.createAnalyser();
        const source = audioCtx.createMediaElementSource(audio);
        source.connect(analyser);
        analyser.connect(audioCtx.destination);
        analyser.fftSize = 256;
        bufferLength = analyser.frequencyBinCount;
        dataArray = new Uint8Array(bufferLength);
        isAudioInitialized = true;
        drawVisualizer();
    } catch (e) {
        console.log("Audio API Error:", e);
        drawFakeVisualizer();
    }
}

function drawVisualizer() {
    if (!visualizerCanvas || !vCtx) return;
    requestAnimationFrame(drawVisualizer);
    
    if (!isMusicPlaying) {
        vCtx.clearRect(0, 0, visualizerCanvas.width, visualizerCanvas.height);
        return;
    }

    if(analyser) {
        analyser.getByteFrequencyData(dataArray);
        vCtx.clearRect(0, 0, visualizerCanvas.width, visualizerCanvas.height);

        const barWidth = (visualizerCanvas.width / bufferLength) * 2.5;
        let barHeight;
        let x = 0;

        for(let i = 0; i < bufferLength; i++) {
            barHeight = dataArray[i] / 2;
            vCtx.fillStyle = `rgb(${barHeight + 100}, 50, 255)`; 
            vCtx.fillRect(x, visualizerCanvas.height - barHeight, barWidth, barHeight);
            x += barWidth + 1;
        }
    }
}

function drawFakeVisualizer() {
    if (!visualizerCanvas || !vCtx) return;
    if(!isMusicPlaying) return requestAnimationFrame(drawFakeVisualizer);
    
    vCtx.clearRect(0, 0, visualizerCanvas.width, visualizerCanvas.height);
    vCtx.fillStyle = '#00f3ff';
    
    const time = Date.now() * 0.005;
    const barCount = 100;
    const barWidth = visualizerCanvas.width / barCount;
    
    for(let i=0; i<barCount; i++) {
        const h = Math.sin(time + i * 0.1) * 50 + Math.random() * 20;
        vCtx.fillRect(i * barWidth, visualizerCanvas.height - h, barWidth - 2, h);
    }
    requestAnimationFrame(drawFakeVisualizer);
}

if (musicBtn && audio) {
    musicBtn.addEventListener('click', () => {
        if (!isAudioInitialized) initAudio();
        
        if (isMusicPlaying) {
            audio.pause();
            musicBtn.style.animation = 'none';
            musicBtn.style.opacity = '0.5';
        } else {
            audio.play().catch(e => console.log("Audio play failed:", e));
            if (audioCtx && audioCtx.state === 'suspended') audioCtx.resume();
            musicBtn.style.animation = 'pulse 2s infinite';
            musicBtn.style.opacity = '1';
        }
        isMusicPlaying = !isMusicPlaying;
    });
}

function resizeVisualizer() {
    if(visualizerCanvas) {
        visualizerCanvas.width = window.innerWidth;
        visualizerCanvas.height = 150;
    }
}
window.addEventListener('resize', resizeVisualizer);
resizeVisualizer();


// --- PARTICLE SYSTEM ---
let width, height;
let particles = [];

// Animation States
const STATE_IDLE = 0;
const STATE_FORMING = 1;
const STATE_SPHERE = 2;
const STATE_EXPLODE = 3;

let currentState = STATE_IDLE;
let sphereRadius = 200;
const colors = ['#00f3ff', '#bc13fe', '#ff0055', '#ffffff'];

function resize() {
    if(!particleCanvas) return;
    width = particleCanvas.width = window.innerWidth;
    height = particleCanvas.height = window.innerHeight;
    sphereRadius = Math.min(width, height) * 0.25;
}
window.addEventListener('resize', resize);
resize();

class Particle {
    constructor() {
        this.reset();
    }

    reset() {
        this.x = Math.random() * width;
        this.y = Math.random() * height;
        this.vx = (Math.random() - 0.5) * 0.5; 
        this.vy = (Math.random() - 0.5) * 0.5;
        this.size = Math.random() * 2 + 1; 
        this.color = colors[Math.floor(Math.random() * colors.length)];
        
        this.theta = Math.random() * Math.PI * 2;
        this.phi = Math.acos((Math.random() * 2) - 1);
        this.baseRadius = sphereRadius;
        
        this.targetX = 0;
        this.targetY = 0;
    }

    update(time) {
        if (currentState === STATE_IDLE) {
            this.x += this.vx;
            this.y += this.vy;
            if (this.x < 0 || this.x > width) this.vx *= -1;
            if (this.y < 0 || this.y > height) this.vy *= -1;
        }
        else if (currentState === STATE_FORMING || currentState === STATE_SPHERE) {
            const cx = width / 2;
            const cy = height / 2;

            const noise = Math.sin(time * 0.002 + this.theta * 5) * 20; 
            const r = this.baseRadius + noise;
            const rotSpeed = 0.001 * time;
            
            const x3d = r * Math.sin(this.phi) * Math.cos(this.theta + rotSpeed);
            const y3d = r * Math.sin(this.phi) * Math.sin(this.theta + rotSpeed);
            const z3d = r * Math.cos(this.phi);

            const scale = 1000 / (1000 - z3d); 
            this.targetX = cx + x3d * scale;
            this.targetY = cy + y3d * scale;

            const dx = this.targetX - this.x;
            const dy = this.targetY - this.y;
            
            const ease = currentState === STATE_FORMING ? 0.05 : 0.2;
            this.x += dx * ease;
            this.y += dy * ease;
        }
        else if (currentState === STATE_EXPLODE) {
            const cx = width / 2;
            const cy = height / 2;
            const dx = this.x - cx;
            const dy = this.y - cy;
            const dist = Math.sqrt(dx*dx + dy*dy) || 1;
            
            this.vx = (dx / dist) * 30;
            this.vy = (dy / dist) * 30;
            
            this.x += this.vx;
            this.y += this.vy;
        }
    }

    draw() {
        if(!ctx) return;
        ctx.fillStyle = this.color;
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.size, 0, Math.PI * 2);
        ctx.fill();
    }
}

function initParticles() {
    particles = [];
    for (let i = 0; i < 400; i++) { 
        particles.push(new Particle());
    }
}
initParticles();

function animate() {
    if(!ctx) return;
    ctx.clearRect(0, 0, width, height);
    
    if (currentState === STATE_SPHERE) {
        ctx.strokeStyle = 'rgba(0, 243, 255, 0.05)';
        ctx.lineWidth = 0.5;
        for (let i = 0; i < particles.length; i++) {
                const p1 = particles[i];
                const p2 = particles[(i+1)%particles.length];
                const dx = p1.x - p2.x;
                const dy = p1.y - p2.y;
                if (dx*dx + dy*dy < 2500) { 
                ctx.beginPath();
                ctx.moveTo(p1.x, p1.y);
                ctx.lineTo(p2.x, p2.y);
                ctx.stroke();
                }
        }
    }

    const time = Date.now();
    particles.forEach(p => {
        p.update(time);
        p.draw();
    });
    requestAnimationFrame(animate);
}
animate();


// --- START SEQUENCE ---
if (startBtn) {
    startBtn.addEventListener('click', () => {
        startBtn.style.opacity = '0';
        startBtn.style.pointerEvents = 'none';
        
        currentState = STATE_FORMING;
        if(statusText) {
            statusText.style.opacity = '1';
            statusText.innerText = "核心汇聚中...";
        }

        setTimeout(() => {
            currentState = STATE_SPHERE;
            if(statusText) statusText.innerText = "能量填充...";
        }, 1500);

        setTimeout(() => {
            currentState = STATE_EXPLODE;
            if(statusText) {
                statusText.innerText = "启动!";
                statusText.style.transform = "scale(2)";
                statusText.style.opacity = "0";
            }
            
            if(warpOverlay) warpOverlay.style.opacity = '1';
            
            setTimeout(() => {
                if(warpOverlay) warpOverlay.style.opacity = '0';
                if(introOverlay) introOverlay.style.display = 'none';
                
                // Show Home Section instead of AI Tool
                showSection(homeSection);
            }, 500);

        }, 3000); 
    });
}


// --- AI APP LOGIC ---
if(imageUpload) {
    imageUpload.addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function(e) {
                preview.src = e.target.result;
                preview.style.display = 'block';
            }
            reader.readAsDataURL(file);
        }
    });
}

window.setPrompt = function(text) {
    if(promptInput) promptInput.value = text;
}

if(optimizeBtn) {
    optimizeBtn.addEventListener('click', async () => {
        const prompt = promptInput.value;
        if (!prompt) return alert('请输入提示词');
        
        loading.style.display = 'block';
        optimizeBtn.disabled = true;
        
        try {
            const res = await fetch(`${API_BASE}/optimize`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ prompt })
            });
            const data = await res.json();
            if (data.optimizedPrompt) {
                promptInput.value = data.optimizedPrompt;
            } else {
                alert('优化失败');
            }
        } catch (err) {
            console.error(err);
            alert('优化出错');
        } finally {
            loading.style.display = 'none';
            optimizeBtn.disabled = false;
        }
    });
}

if(generateBtn) {
    generateBtn.addEventListener('click', async () => {
        const prompt = promptInput.value;
        const file = imageUpload.files[0];
        
        if (!prompt && !file) return alert('请上传图片或输入提示词');

        loading.style.display = 'block';
        resultImage.style.display = 'none';
        downloadLink.style.display = 'none';
        resultText.innerText = '';
        generateBtn.disabled = true;

        const formData = new FormData();
        formData.append('prompt', prompt);
        if (file) {
            formData.append('image', file);
        }

        try {
            const res = await fetch(`${API_BASE}/generate`, {
                method: 'POST',
                body: formData
            });
            const data = await res.json();
            
            if (data.imageUrl) {
                resultImage.src = data.imageUrl;
                resultImage.style.display = 'block';
                downloadLink.href = data.imageUrl;
                downloadLink.style.display = 'inline-block';
            }
            
            if (data.description) {
                resultText.innerText = data.description;
            }

            if (!data.imageUrl && !data.description) {
                    resultText.innerText = "未收到结果，请重试。";
            }

        } catch (err) {
            console.error(err);
            resultText.innerText = '生成出错: ' + err.message;
        } finally {
            loading.style.display = 'none';
            generateBtn.disabled = false;
        }
    });
}
