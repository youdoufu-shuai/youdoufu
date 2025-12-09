const API_BASE = 'http://localhost:3011/api';

// --- DOM ELEMENTS ---
const startBtn = document.getElementById('start-btn');
const introOverlay = document.getElementById('intro-overlay');
const statusText = document.getElementById('status-text');
const warpOverlay = document.getElementById('warp-overlay');

const homeSection = document.getElementById('home-section');
const aiToolSection = document.getElementById('ai-tool-section');

const toToolboxBtn = document.getElementById('to-toolbox');
const backHomeBtn = document.getElementById('back-home-btn');

const imageUpload = document.getElementById('image-upload');
const preview = document.getElementById('preview');
const promptInput = document.getElementById('prompt-input');
const optimizeBtn = document.getElementById('optimize-btn');
const generateBtn = document.getElementById('generate-btn');
const loading = document.getElementById('loading');
const resultImage = document.getElementById('result-image');
const resultText = document.getElementById('result-text');
const downloadLink = document.getElementById('download-link');

const audio = document.getElementById('bg-music');
const musicBtn = document.getElementById('music-toggle');
const visualizerCanvas = document.getElementById('audio-visualizer');
const vCtx = visualizerCanvas ? visualizerCanvas.getContext('2d') : null;

const particleCanvas = document.getElementById('particles-js');
const ctx = particleCanvas ? particleCanvas.getContext('2d') : null;

// --- NAVIGATION ---
function showSection(section) {
    if(homeSection) { homeSection.classList.remove('visible'); homeSection.style.display = 'none'; }
    if(aiToolSection) { aiToolSection.classList.remove('visible'); aiToolSection.style.display = 'none'; }
    if(section) {
        section.style.display = 'block';
        setTimeout(() => section.classList.add('visible'), 50);
    }
}

if (toToolboxBtn) toToolboxBtn.addEventListener('click', (e) => { e.preventDefault(); showSection(aiToolSection); });
if (backHomeBtn) backHomeBtn.addEventListener('click', () => showSection(homeSection));

// --- AUDIO ---
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
    if (!isMusicPlaying) { vCtx.clearRect(0, 0, visualizerCanvas.width, visualizerCanvas.height); return; }
    if(analyser) {
        analyser.getByteFrequencyData(dataArray);
        vCtx.clearRect(0, 0, visualizerCanvas.width, visualizerCanvas.height);
        const barWidth = (visualizerCanvas.width / bufferLength) * 2.5;
        let x = 0;
        for(let i = 0; i < bufferLength; i++) {
            const barHeight = dataArray[i] / 2;
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
            audio.play().catch(e => console.log(e));
            if (audioCtx && audioCtx.state === 'suspended') audioCtx.resume();
            musicBtn.style.animation = 'pulse 2s infinite';
            musicBtn.style.opacity = '1';
        }
        isMusicPlaying = !isMusicPlaying;
    });
}

function resizeVisualizer() {
    if(visualizerCanvas) { visualizerCanvas.width = window.innerWidth; visualizerCanvas.height = 150; }
}
window.addEventListener('resize', resizeVisualizer);
resizeVisualizer();

// --- PARTICLES & ANIMATION ---
let width, height;
let particles = [];

const STATE_IDLE_SPHERE = 0;
const STATE_FORMING_ROCKET = 1;
const STATE_IGNITION = 2;
const STATE_LAUNCH = 3;
const STATE_SPACE_FLIGHT = 4;
const STATE_BLACK_HOLE = 5;
const STATE_BIG_BANG = 6;
const STATE_HOME = 7;

let currentState = STATE_IDLE_SPHERE;
let sphereRadius = 200;
let currentShapeName = '';
let rotationAngle = 0;

// Home Scenes
let homeSceneIndex = 0;
const HOME_SCENES = ['GALAXY', 'SOLAR', 'THREE_BODY', 'EARTH_MOON'];
let lastSceneChangeTime = 0;
const SCENE_DURATION = 10000;

// Three Body
let threeBodySystem = { bodies: [], trails: [] };
// Earth Moon
let earthRotation = 0;

const colors = ['#00f3ff', '#bc13fe', '#ff0055', '#ffffff', '#ffd700'];

function resize() {
    if(!particleCanvas) return;
    width = particleCanvas.width = window.innerWidth;
    height = particleCanvas.height = window.innerHeight;
    sphereRadius = Math.min(width, height) * 0.25;
}
window.addEventListener('resize', resize);
resize();

class Particle {
    constructor() { this.reset(); }
    reset() {
        this.x = Math.random() * width;
        this.y = Math.random() * height;
        this.theta = Math.random() * Math.PI * 2;
        this.phi = Math.acos((Math.random() * 2) - 1);
        this.baseRadius = sphereRadius;
        this.targetX = null;
        this.targetY = null;
        this.isBackground = true;
        this.size = Math.random() * 2 + 0.5;
        this.color = colors[Math.floor(Math.random() * colors.length)];
        this.vx = (Math.random() - 0.5) * 0.5;
        this.vy = (Math.random() - 0.5) * 0.5;
        this.noiseOffset = Math.random() * 1000;
        this.targetData = null; // For Earth/Moon
    }

    update(time) {
        rotationAngle += 0.002;

        if (currentState === STATE_IDLE_SPHERE) {
            const cx = width / 2;
            const cy = height / 2;
            const rotSpeed = 0.0008 * time;
            
            // Morphing Sphere
            const noise1 = Math.sin(time * 0.002 + this.phi * 3 + this.theta * 2);
            const noise2 = Math.cos(time * 0.003 + this.phi * 5);
            const pulse = Math.sin(time * 0.001) * 0.2 + 1;
            const r = this.baseRadius * pulse + (noise1 * 15 + noise2 * 10);
            const wave = Math.sin(this.phi * 10 + time * 0.005) * 5;
            const finalR = r + wave;

            const x3d = finalR * Math.sin(this.phi) * Math.cos(this.theta + rotSpeed);
            const y3d = finalR * Math.sin(this.phi) * Math.sin(this.theta + rotSpeed);
            const z3d = finalR * Math.cos(this.phi);
            
            const scale = 1000 / (1000 - z3d);
            const projX = cx + x3d * scale;
            const projY = cy + y3d * scale;
            
            this.x += (projX - this.x) * 0.1;
            this.y += (projY - this.y) * 0.1;

            if (Math.random() < 0.01) {
                this.color = colors[Math.floor(Math.abs(Math.sin(time * 0.001 + this.theta)) * colors.length)];
            }
        } 
        else if (currentState >= STATE_FORMING_ROCKET && currentState < STATE_HOME) {
            if (currentState === STATE_BIG_BANG) {
                const cx = width / 2; const cy = height / 2;
                const dx = this.x - cx; const dy = this.y - cy;
                const dist = Math.sqrt(dx*dx + dy*dy) || 1;
                const speed = (Math.random() * 20 + 5) * (dist < 100 ? 2 : 1);
                this.vx = (dx / dist) * speed; 
                this.vy = (dy / dist) * speed;
                this.x += this.vx; this.y += this.vy;
                return;
            }

            if (this.targetX !== null) {
                if (currentState === STATE_LAUNCH) {
                    if (!this.isBackground) {
                        this.targetY -= 8; 
                        this.x += (Math.random()-0.5);
                    } else {
                        this.y += 25; 
                        if (this.y > height) { this.y = 0; this.x = Math.random() * width; }
                        return;
                    }
                }

                let finalTargetX = this.targetX;
                let finalTargetY = this.targetY;
                
                if (currentState === STATE_SPACE_FLIGHT || currentState === STATE_BLACK_HOLE) {
                     const cx = width / 2; const cy = height / 2;
                     const dx = this.targetX - cx; const dy = this.targetY - cy;
                     const dist = Math.sqrt(dx*dx + dy*dy);
                     const angle = Math.atan2(dy, dx);
                     let rotSpeed = 0.002;
                     if (currentShapeName === 'BLACK_HOLE') rotSpeed = 0.03;
                     if (currentShapeName === 'GALAXY') rotSpeed = 0.008;
                     const newAngle = angle + rotSpeed * (currentShapeName === 'GALAXY' ? (300/(dist+10)) : 1);
                     const noiseX = Math.sin(time * 0.002 + this.noiseOffset) * 2;
                     const noiseY = Math.cos(time * 0.002 + this.noiseOffset) * 2;
                     finalTargetX = cx + Math.cos(newAngle) * dist + noiseX;
                     finalTargetY = cy + Math.sin(newAngle) * dist + noiseY;
                     this.targetX = cx + Math.cos(angle + rotSpeed*0.1) * dist; 
                     this.targetY = cy + Math.sin(angle + rotSpeed*0.1) * dist;
                }

                if (currentState === STATE_IGNITION && !this.isBackground) {
                    finalTargetX += (Math.random()-0.5) * 8;
                    finalTargetY += (Math.random()-0.5) * 8;
                }
                const ease = currentState === STATE_FORMING_ROCKET ? 0.08 : 0.15;
                this.x += (finalTargetX - this.x) * ease;
                this.y += (finalTargetY - this.y) * ease;
            } else {
                this.x += this.vx; this.y += this.vy;
                if (this.x < 0 || this.x > width) this.vx *= -1;
                if (this.y < 0 || this.y > height) this.vy *= -1;
            }
        }
        else if (currentState === STATE_HOME) {
            updateHomeParticle(this, time);
        }
    }

    draw() {
        if(!ctx) return;
        ctx.beginPath();
        ctx.arc(this.x, this.y, this.size, 0, Math.PI * 2);
        ctx.fillStyle = this.color;
        ctx.fill();
    }
}

function initParticles() {
    particles = [];
    for (let i = 0; i < 800; i++) particles.push(new Particle());
}
initParticles();

function defineRocketShape() {
    const cx = width / 2;
    const cy = height * 0.7;
    let targets = [];
    // Detailed Rocket
    for(let i=0; i<300; i++) {
        if (i < 50) targets.push({x: cx + (Math.random()-0.5)*i*0.5, y: cy - 120 + i*2}); // Sharp Nose
        else if (i < 250) targets.push({x: cx + (Math.random()-0.5)*30, y: cy - 40 + (i-50)*0.8}); // Body
        else targets.push({x: cx + (i%2==0?1:-1)*(30+(i-250)*1.2), y: cy + 30 + (i-250)}); // Wide Fins
    }
    assignTargets(targets);
}

function defineSolarSystem() {
    const cx = width / 2; const cy = height / 2;
    let targets = [];
    [50, 100, 180, 280, 400].forEach(r => {
        for(let i=0; i<80; i++) {
            const a = Math.random() * Math.PI * 2;
            targets.push({x: cx + Math.cos(a)*r, y: cy + Math.sin(a)*r});
        }
    });
    for(let i=0; i<50; i++) {
        const a = Math.random() * Math.PI * 2;
        const r = Math.random() * 20;
        targets.push({x: cx + Math.cos(a)*r, y: cy + Math.sin(a)*r});
    }
    currentShapeName = 'SOLAR';
    assignTargets(targets);
}

function defineGalaxy() {
    const cx = width / 2; const cy = height / 2;
    let targets = [];
    const arms = 4;
    for(let i=0; i<600; i++) {
            const arm = i % arms;
            const r = (i / arms) * 0.8 + 20;
            const angle = i * 0.02 + (arm * Math.PI * 2 / arms);
            const spread = (Math.random()-0.5) * 20;
            targets.push({ x: cx + Math.cos(angle)*r + spread, y: cy + Math.sin(angle)*r + spread });
    }
    currentShapeName = 'GALAXY';
    assignTargets(targets);
}

function defineBlackHole() {
    const cx = width / 2; const cy = height / 2;
    let targets = [];
    for(let i=0; i<100; i++) {
        const a = Math.random() * Math.PI * 2;
        targets.push({x: cx + Math.cos(a)*30, y: cy + Math.sin(a)*30});
    }
    for(let i=0; i<500; i++) {
        const a = Math.random() * Math.PI * 2;
        const r = 50 + Math.random() * 300;
        targets.push({x: cx + Math.cos(a)*r, y: cy + Math.sin(a)*(r*0.4)}); 
    }
    currentShapeName = 'BLACK_HOLE';
    assignTargets(targets);
}

function assignTargets(targets) {
    targets.sort(() => Math.random() - 0.5);
    particles.forEach((p, i) => {
        if (i < targets.length) {
            p.targetX = targets[i].x;
            p.targetY = targets[i].y;
            p.isBackground = false;
        } else {
            p.targetX = null;
            p.isBackground = true;
        }
    });
}

// --- HOME SCENE MANAGERS ---
function initThreeBody() {
    const cx = width / 2; const cy = height / 2;
    threeBodySystem.bodies = [
        { x: cx, y: cy - 100, vx: 5, vy: 0, mass: 100, color: '#ff0055' },
        { x: cx - 86, y: cy + 50, vx: -2.5, vy: -4.3, mass: 100, color: '#00f3ff' },
        { x: cx + 86, y: cy + 50, vx: -2.5, vy: 4.3, mass: 100, color: '#bc13fe' }
    ];
    threeBodySystem.trails = [];
    currentShapeName = 'THREE_BODY';
    particles.forEach(p => { p.isBackground = true; p.vx = (Math.random()-0.5)*0.2; p.vy = (Math.random()-0.5)*0.2; });
}

function updateThreeBodyPhysics() {
    const bodies = threeBodySystem.bodies;
    const G = 0.5;
    for (let i = 0; i < bodies.length; i++) {
        for (let j = 0; j < bodies.length; j++) {
            if (i === j) continue;
            const dx = bodies[j].x - bodies[i].x;
            const dy = bodies[j].y - bodies[i].y;
            const dist = Math.sqrt(dx*dx + dy*dy) || 1;
            const force = (G * bodies[i].mass * bodies[j].mass) / (dist * dist);
            bodies[i].vx += (force * dx / dist) / bodies[i].mass;
            bodies[i].vy += (force * dy / dist) / bodies[i].mass;
        }
    }
    bodies.forEach(b => {
        b.x += b.vx; b.y += b.vy;
        const cx = width/2; const cy = height/2;
        const dist = Math.sqrt((b.x-cx)**2 + (b.y-cy)**2);
        if(dist > 400) { b.vx -= (b.x - cx) * 0.0001; b.vy -= (b.y - cy) * 0.0001; }
        if(Math.random() < 0.3) threeBodySystem.trails.push({ x: b.x, y: b.y, life: 1.0, color: b.color });
    });
    threeBodySystem.trails.forEach(t => t.life -= 0.005);
    threeBodySystem.trails = threeBodySystem.trails.filter(t => t.life > 0);
}

function defineEarthMoon() {
    const cx = width / 2; const cy = height / 2;
    let targets = [];
    for(let i=0; i<400; i++) targets.push({ type: 'earth', theta: Math.random()*Math.PI*2, phi: Math.acos((Math.random()*2)-1), r: 100 });
    for(let i=0; i<50; i++) targets.push({ type: 'moon', theta: Math.random()*Math.PI*2, phi: Math.acos((Math.random()*2)-1), r: 20 });
    currentShapeName = 'EARTH_MOON';
    particles.forEach((p, i) => {
        if (i < targets.length) { p.targetData = targets[i]; p.isBackground = false; }
        else { p.targetData = null; p.isBackground = true; }
    });
}

function updateHomeParticle(p, time) {
    const scene = HOME_SCENES[homeSceneIndex];
    const cx = width / 2; const cy = height / 2;

    if (scene === 'THREE_BODY') {
        p.x += p.vx; p.y += p.vy;
        if(p.x < 0) p.x = width; if(p.x > width) p.x = 0;
        if(p.y < 0) p.y = height; if(p.y > height) p.y = 0;
        return;
    }
    if (scene === 'EARTH_MOON') {
        if (!p.isBackground && p.targetData) {
            earthRotation += 0.00001;
            const rot = time * 0.0005;
            let tx, ty;
            if (p.targetData.type === 'earth') {
                const r = p.targetData.r;
                const theta = p.targetData.theta + rot;
                const phi = p.targetData.phi;
                const x3d = r * Math.sin(phi) * Math.cos(theta);
                const y3d = r * Math.sin(phi) * Math.sin(theta);
                const z3d = r * Math.cos(phi);
                const tilt = 0.4;
                const y3d_t = y3d * Math.cos(tilt) - z3d * Math.sin(tilt);
                const z3d_t = y3d * Math.sin(tilt) + z3d * Math.cos(tilt);
                const scale = 1000 / (1000 - z3d_t);
                tx = cx + x3d * scale; ty = cy + y3d_t * scale;
            } else {
                const orbitR = 250;
                const moonOrbitSpeed = rot * 0.5;
                const mx = Math.cos(moonOrbitSpeed) * orbitR;
                const mz = Math.sin(moonOrbitSpeed) * orbitR;
                const r = p.targetData.r;
                const theta = p.targetData.theta + rot * 2;
                const phi = p.targetData.phi;
                const x3d = r * Math.sin(phi) * Math.cos(theta) + mx;
                const y3d = r * Math.sin(phi) * Math.sin(theta);
                const z3d = r * Math.cos(phi) + mz;
                const scale = 1000 / (1000 - z3d);
                tx = cx + x3d * scale; ty = cy + y3d * scale;
            }
            p.x += (tx - p.x) * 0.1; p.y += (ty - p.y) * 0.1;
        } else {
             p.x += p.vx; p.y += p.vy;
             if(p.x < 0) p.x = width; if(p.x > width) p.x = 0;
             if(p.y < 0) p.y = height; if(p.y > height) p.y = 0;
        }
        return;
    }
    if (scene === 'GALAXY' || scene === 'SOLAR') {
         if (currentShapeName !== scene) { if(scene === 'GALAXY') defineGalaxy(); else defineSolarSystem(); }
         if (!p.isBackground && p.targetX !== null) {
             const dx = p.targetX - cx; const dy = p.targetY - cy;
             const dist = Math.sqrt(dx*dx + dy*dy);
             const angle = Math.atan2(dy, dx);
             let rotSpeed = 0.001;
             const newAngle = angle + rotSpeed * (scene === 'GALAXY' ? (200/(dist+10)) : 1);
             const noiseX = Math.sin(time * 0.001 + p.noiseOffset);
             const noiseY = Math.cos(time * 0.001 + p.noiseOffset);
             const tx = cx + Math.cos(newAngle) * dist + noiseX;
             const ty = cy + Math.sin(newAngle) * dist + noiseY;
             p.targetX = cx + Math.cos(angle + rotSpeed*0.1) * dist; 
             p.targetY = cy + Math.sin(angle + rotSpeed*0.1) * dist;
             p.x += (tx - p.x) * 0.1; p.y += (ty - p.y) * 0.1;
         } else {
             p.x += p.vx; p.y += p.vy;
             if(p.x < 0) p.x = width; if(p.x > width) p.x = 0;
             if(p.y < 0) p.y = height; if(p.y > height) p.y = 0;
         }
    }
}

function animateLoop() {
    if(!ctx) return;
    const time = Date.now();

    if (currentState === STATE_HOME) {
        if (time - lastSceneChangeTime > SCENE_DURATION) {
            lastSceneChangeTime = time;
            homeSceneIndex = (homeSceneIndex + 1) % HOME_SCENES.length;
            const scene = HOME_SCENES[homeSceneIndex];
            if (scene === 'THREE_BODY') initThreeBody();
            else if (scene === 'EARTH_MOON') defineEarthMoon();
            else if (scene === 'GALAXY') defineGalaxy();
            else if (scene === 'SOLAR') defineSolarSystem();
        }
        if (HOME_SCENES[homeSceneIndex] === 'THREE_BODY') updateThreeBodyPhysics();
    }
    
    if (currentState === STATE_LAUNCH || currentState === STATE_BIG_BANG) {
        ctx.fillStyle = 'rgba(5, 5, 10, 0.3)';
        ctx.fillRect(0, 0, width, height);
    } else if (currentState === STATE_HOME && HOME_SCENES[homeSceneIndex] === 'THREE_BODY') {
         ctx.fillStyle = 'rgba(5, 5, 10, 0.1)';
         ctx.fillRect(0, 0, width, height);
    } else {
        ctx.clearRect(0, 0, width, height);
        if (currentState === STATE_IDLE_SPHERE) {
            ctx.strokeStyle = 'rgba(0, 243, 255, 0.05)';
            ctx.lineWidth = 0.5;
            for (let i = 0; i < particles.length; i+=2) {
                    const p1 = particles[i];
                    const p2 = particles[(i+1)%particles.length];
                    const dx = p1.x - p2.x;
                    const dy = p1.y - p2.y;
                    if (dx*dx + dy*dy < 2000) { 
                        ctx.beginPath(); ctx.moveTo(p1.x, p1.y); ctx.lineTo(p2.x, p2.y); ctx.stroke();
                    }
            }
        }
    }

    particles.forEach(p => { p.update(time); p.draw(); });
    
    if (currentState === STATE_HOME && HOME_SCENES[homeSceneIndex] === 'THREE_BODY') {
         threeBodySystem.trails.forEach(t => {
             ctx.fillStyle = t.color; ctx.globalAlpha = t.life; ctx.fillRect(t.x, t.y, 2, 2);
         });
         ctx.globalAlpha = 1.0;
         threeBodySystem.bodies.forEach(b => {
             ctx.beginPath(); ctx.arc(b.x, b.y, 8, 0, Math.PI*2);
             ctx.fillStyle = b.color; ctx.shadowBlur = 15; ctx.shadowColor = b.color; ctx.fill(); ctx.shadowBlur = 0;
         });
    }

    if (currentState === STATE_IGNITION || currentState === STATE_LAUNCH) {
         const cx = width / 2;
         const fixedCy = currentState === STATE_LAUNCH ? height - 100 : height * 0.7 + 60;
         for(let k=0; k<10; k++) {
             ctx.beginPath();
             ctx.arc(cx + (Math.random()-0.5)*30, fixedCy + Math.random()*80, Math.random()*8+2, 0, Math.PI*2);
             ctx.fillStyle = `rgba(255, ${Math.random()*150 + 100}, 0, 0.6)`;
             ctx.fill();
         }
    }
    requestAnimationFrame(animateLoop);
}
animateLoop();

if (startBtn) {
    startBtn.addEventListener('click', () => {
        startBtn.style.opacity = '0';
        startBtn.style.pointerEvents = 'none';
        
        if (!isAudioInitialized) initAudio();
        if (audioCtx && audioCtx.state === 'suspended') audioCtx.resume();
        audio.play().then(() => {
             isMusicPlaying = true;
             musicBtn.style.animation = 'pulse 2s infinite';
             musicBtn.style.opacity = '1';
        }).catch(e => console.log(e));

        currentState = STATE_FORMING_ROCKET;
        defineRocketShape();
        if(statusText) { statusText.style.opacity = '1'; statusText.innerText = "系统装载..."; }

        setTimeout(() => { currentState = STATE_IGNITION; if(statusText) statusText.innerText = "引擎预热..."; }, 2000);
        setTimeout(() => { currentState = STATE_LAUNCH; if(statusText) statusText.innerText = "发射!"; }, 3500);
        setTimeout(() => { currentState = STATE_SPACE_FLIGHT; defineSolarSystem(); if(statusText) statusText.innerText = "进入轨道..."; }, 6000);
        setTimeout(() => { defineGalaxy(); if(statusText) statusText.innerText = "跨越星系..."; }, 9000);
        setTimeout(() => { currentState = STATE_BLACK_HOLE; defineBlackHole(); if(statusText) statusText.innerText = "接近奇点..."; }, 12000);
        
        setTimeout(() => {
            currentState = STATE_BIG_BANG;
            if(warpOverlay) warpOverlay.style.opacity = '1';
            setTimeout(() => {
                if(warpOverlay) warpOverlay.style.opacity = '0';
                if(introOverlay) introOverlay.style.display = 'none';
                currentState = STATE_HOME;
                showSection(homeSection);
                // Init Home Scene
                homeSceneIndex = 0;
                defineGalaxy(); // Start with Galaxy
            }, 1000);
        }, 15000); 
    });
}

if(imageUpload) {
    imageUpload.addEventListener('change', function(e) {
        const file = e.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function(e) { preview.src = e.target.result; preview.style.display = 'block'; }
            reader.readAsDataURL(file);
        }
    });
}
window.setPrompt = function(text) { if(promptInput) promptInput.value = text; }
