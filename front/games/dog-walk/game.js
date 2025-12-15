const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// Game State
let gameState = 'START'; // START, PLAYING, GAME_OVER
let score = 0;
let frameCount = 0;
let startTime = 0;

// Resize handling
function resize() {
    canvas.width = window.innerWidth;
    canvas.height = window.innerHeight;
}
window.addEventListener('resize', resize);
resize();

// Input handling
const mouse = { x: canvas.width / 2, y: canvas.height / 2, down: false };

function updateInput(x, y, isDown) {
    mouse.x = x;
    mouse.y = y;
    if (isDown !== null) mouse.down = isDown;
}

window.addEventListener('mousedown', (e) => updateInput(e.clientX, e.clientY, true));
window.addEventListener('mousemove', (e) => updateInput(e.clientX, e.clientY, null));
window.addEventListener('mouseup', () => updateInput(mouse.x, mouse.y, false));

window.addEventListener('touchstart', (e) => {
    e.preventDefault();
    updateInput(e.touches[0].clientX, e.touches[0].clientY, true);
}, { passive: false });
window.addEventListener('touchmove', (e) => {
    e.preventDefault();
    updateInput(e.touches[0].clientX, e.touches[0].clientY, null);
}, { passive: false });
window.addEventListener('touchend', () => updateInput(mouse.x, mouse.y, false));

// Assets Loader
const assets = {
    dog: { src: 'assets/dog-removebg-preview.png' },
    bone: { src: 'assets/bone-removebg-preview.png' },
    gold: { src: 'assets/glod-removebg-preview.png' }, // Note: glod typo in filename
    shou: { src: 'assets/shou-removebg-preview.png' },
    shouCatch: { src: 'assets/shouCatch-removebg-preview.png' },
    sword: { src: 'assets/sword-removebg-preview.png' },
    xianjing1: { src: 'assets/xianjing1-removebg-preview.png' },
    xianjing2: { src: 'assets/xianjing2-removebg-preview.png' },
    zidan: { src: 'assets/zidan-removebg-preview.png' },
    zidan1: { src: 'assets/zidan1-removebg-preview.png' }
};

const images = {};
let assetsLoaded = 0;
const totalAssets = Object.keys(assets).length;

Object.keys(assets).forEach(key => {
    const img = new Image();
    img.src = assets[key].src;
    img.onload = () => assetsLoaded++;
    img.onerror = () => {
        console.warn(`Failed to load ${assets[key].src}`);
        img.isMissing = true;
        assetsLoaded++;
    };
    images[key] = img;
});

// Helper: Get difficulty multiplier based on time
function getDifficulty() {
    if (gameState !== 'PLAYING') return 1;
    const elapsed = (Date.now() - startTime) / 1000; // seconds
    // Difficulty increases every 10 seconds, up to a max
    return 1 + Math.min(elapsed / 20, 5); // 1.0 -> 6.0
}

// Helper: Collision Detection (Circle based for smoother gameplay)
function checkCollision(circle1, circle2) {
    const dx = circle1.x - circle2.x;
    const dy = circle1.y - circle2.y;
    const distance = Math.sqrt(dx * dx + dy * dy);
    return distance < (circle1.radius + circle2.radius);
}

// Game Objects

class Hand {
    constructor() {
        this.width = 100;
        this.height = 100;
    }

    draw() {
        const x = mouse.x;
        const y = mouse.y;
        const img = mouse.down ? images.shouCatch : images.shou;
        
        if (img && !img.isMissing && img.complete) {
            ctx.drawImage(img, x - this.width/2, y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = mouse.down ? 'red' : 'blue';
            ctx.beginPath();
            ctx.arc(x, y, 20, 0, Math.PI * 2);
            ctx.fill();
        }
    }
}

class Bone {
    constructor() {
        this.x = canvas.width / 2;
        this.y = canvas.height / 2;
        this.width = 50;
        this.height = 30;
        this.isHeld = false;
        this.radius = 15; // Collision radius
    }

    update() {
        if (mouse.down) {
            this.isHeld = true;
            this.x = mouse.x;
            this.y = mouse.y;
        } else {
            this.isHeld = false;
        }
    }

    draw() {
        if (images.bone && !images.bone.isMissing && images.bone.complete) {
            ctx.drawImage(images.bone, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = 'white';
            ctx.fillRect(this.x - 25, this.y - 15, 50, 30);
        }
    }
}

class Dog {
    constructor() {
        this.x = 100;
        this.y = 100;
        this.width = 80;
        this.height = 60;
        this.baseSpeed = 3;
        this.radius = 25; // Hitbox radius
        this.facingRight = true;
    }

    update(bone) {
        if (bone.isHeld) {
            // Just look at bone
            this.facingRight = bone.x > this.x;
        } else {
            // Walk towards bone
            const dx = bone.x - this.x;
            const dy = bone.y - this.y;
            const dist = Math.sqrt(dx*dx + dy*dy);
            
            if (dist > 10) { // Deadzone
                const moveX = (dx / dist) * this.baseSpeed;
                const moveY = (dy / dist) * this.baseSpeed;
                
                this.x += moveX;
                this.y += moveY;
                this.facingRight = dx > 0;
            }
        }

        // Boundary
        this.x = Math.max(this.radius, Math.min(canvas.width - this.radius, this.x));
        this.y = Math.max(this.radius, Math.min(canvas.height - this.radius, this.y));
    }

    draw() {
        if (images.dog && !images.dog.isMissing && images.dog.complete) {
            ctx.save();
            ctx.translate(this.x, this.y);
            if (!this.facingRight) ctx.scale(-1, 1);
            ctx.drawImage(images.dog, -this.width/2, -this.height/2, this.width, this.height);
            ctx.restore();
        } else {
            ctx.fillStyle = 'brown';
            ctx.beginPath();
            ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
            ctx.fill();
        }
    }
}

class Trap {
    constructor(type) {
        this.type = type; // 'sword', 'xianjing1', 'xianjing2'
        this.x = Math.random() * (canvas.width - 100) + 50;
        this.y = Math.random() * (canvas.height - 100) + 50;
        this.width = 60;
        this.height = 60;
        this.radius = 25;
        
        // State Machine
        this.state = 'WARNING'; // WARNING -> ACTIVE
        this.warningTimer = 120; // 2 seconds at 60fps (decreases with diff)
        this.activeTimer = 300; // 5 seconds
        this.angle = 0; // For sword
        
        // Difficulty adjustments
        const diff = getDifficulty();
        this.warningTimer = Math.max(30, 120 - diff * 10); // Faster warning at high diff
    }

    update() {
        if (this.state === 'WARNING') {
            this.warningTimer--;
            if (this.warningTimer <= 0) {
                this.state = 'ACTIVE';
            }
        } else if (this.state === 'ACTIVE') {
            this.activeTimer--;
            if (this.type === 'sword') {
                this.angle += 0.2;
            }
        }
    }

    draw() {
        if (this.state === 'WARNING') {
            // Draw warning circle
            ctx.save();
            ctx.translate(this.x, this.y);
            ctx.beginPath();
            ctx.arc(0, 0, this.radius + 10, 0, Math.PI * 2);
            ctx.fillStyle = `rgba(255, 0, 0, ${0.3 + Math.sin(frameCount * 0.2) * 0.2})`; // Pulse
            ctx.fill();
            ctx.strokeStyle = 'red';
            ctx.lineWidth = 2;
            ctx.stroke();
            
            // Show faded preview
            const img = images[this.type];
            if (img && !img.isMissing && img.complete) {
                ctx.globalAlpha = 0.5;
                ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
                ctx.globalAlpha = 1.0;
            }
            ctx.restore();
            
        } else if (this.state === 'ACTIVE') {
            const img = images[this.type];
            if (img && !img.isMissing && img.complete) {
                ctx.save();
                ctx.translate(this.x, this.y);
                if (this.type === 'sword') ctx.rotate(this.angle);
                ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
                ctx.restore();
            } else {
                ctx.fillStyle = 'purple';
                ctx.fillRect(this.x - 20, this.y - 20, 40, 40);
            }
        }
    }
    
    isDangerous() {
        return this.state === 'ACTIVE';
    }
}

class Bullet {
    constructor(type) {
        this.type = type; // 'zidan', 'zidan1'
        this.radius = 15;
        this.width = 40;
        this.height = 20;
        
        // Spawn from random edge
        const side = Math.floor(Math.random() * 4); // 0:Top, 1:Right, 2:Bottom, 3:Left
        const diff = getDifficulty();
        const speed = (type === 'zidan1' ? 8 : 4) * (1 + diff * 0.2); // Speed increases with difficulty
        
        switch(side) {
            case 0: // Top
                this.x = Math.random() * canvas.width;
                this.y = -50;
                break;
            case 1: // Right
                this.x = canvas.width + 50;
                this.y = Math.random() * canvas.height;
                break;
            case 2: // Bottom
                this.x = Math.random() * canvas.width;
                this.y = canvas.height + 50;
                break;
            case 3: // Left
                this.x = -50;
                this.y = Math.random() * canvas.height;
                break;
        }

        // Target a random point within the screen to ensure it crosses
        const targetX = Math.random() * canvas.width;
        const targetY = Math.random() * canvas.height;
        const angle = Math.atan2(targetY - this.y, targetX - this.x);
        
        this.vx = Math.cos(angle) * speed;
        this.vy = Math.sin(angle) * speed;
        this.rotation = angle;
    }

    update() {
        this.x += this.vx;
        this.y += this.vy;
    }

    draw() {
        const img = images[this.type];
        if (img && !img.isMissing && img.complete) {
            ctx.save();
            ctx.translate(this.x, this.y);
            ctx.rotate(this.rotation);
            // If flying left (angle > 90 or < -90), maybe flip? 
            // Usually bullets are drawn pointing right. Rotation handles direction.
            ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
            ctx.restore();
        } else {
            ctx.fillStyle = 'orange';
            ctx.beginPath();
            ctx.arc(this.x, this.y, 10, 0, Math.PI * 2);
            ctx.fill();
        }
    }
    
    isOffScreen() {
        return (this.x < -100 || this.x > canvas.width + 100 || 
                this.y < -100 || this.y > canvas.height + 100);
    }
}

class Gold {
    constructor() {
        this.x = Math.random() * (canvas.width - 60) + 30;
        this.y = Math.random() * (canvas.height - 60) + 30;
        this.width = 40;
        this.height = 40;
        this.radius = 20;
    }

    draw() {
        if (images.gold && !images.gold.isMissing && images.gold.complete) {
            ctx.drawImage(images.gold, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = 'gold';
            ctx.beginPath();
            ctx.arc(this.x, this.y, 15, 0, Math.PI * 2);
            ctx.fill();
        }
    }
}

// Global Instances
let hand, bone, dog;
let obstacles = [];
let golds = [];

function init() {
    hand = new Hand();
    bone = new Bone();
    dog = new Dog();
    obstacles = [];
    golds = [];
    score = 0;
    frameCount = 0;
    startTime = Date.now();
    document.getElementById('score').innerText = '金币: 0';
}

function gameLoop() {
    if (gameState !== 'PLAYING') return;

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Draw "Plane" Background (Grid or Grass)
    ctx.fillStyle = '#90EE90'; // Light Green
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    // Grid lines
    ctx.strokeStyle = 'rgba(255,255,255,0.3)';
    ctx.lineWidth = 2;
    const gridSize = 100;
    for(let x=0; x<canvas.width; x+=gridSize) {
        ctx.beginPath(); ctx.moveTo(x,0); ctx.lineTo(x,canvas.height); ctx.stroke();
    }
    for(let y=0; y<canvas.height; y+=gridSize) {
        ctx.beginPath(); ctx.moveTo(0,y); ctx.lineTo(canvas.width,y); ctx.stroke();
    }

    frameCount++;
    const diff = getDifficulty();

    // Spawner logic
    // Rate increases with difficulty. 
    // Base: 60 frames. High diff: 20 frames.
    const spawnRate = Math.max(20, Math.floor(60 / (diff * 0.8)));
    
    if (frameCount % spawnRate === 0) {
        const rand = Math.random();
        if (rand < 0.2) {
            // Gold (less frequent)
            if (golds.length < 5) golds.push(new Gold());
        } else if (rand < 0.6) {
            // Bullet
            const type = Math.random() > 0.7 ? 'zidan1' : 'zidan'; // zidan1 is rare-ish
            obstacles.push(new Bullet(type));
        } else {
            // Trap
            const types = ['sword', 'xianjing1', 'xianjing2'];
            const type = types[Math.floor(Math.random() * types.length)];
            obstacles.push(new Trap(type));
        }
    }

    // Update & Draw Entities
    
    // Traps (Layer 0: On ground)
    obstacles.forEach((obs, index) => {
        if (obs instanceof Trap) {
            obs.update();
            obs.draw();
            
            if (obs.isDangerous() && checkCollision(dog, obs)) {
                gameOver();
            }
            if (obs.state === 'ACTIVE' && obs.activeTimer <= 0) {
                obstacles.splice(index, 1);
            }
        }
    });
    
    // Dog & Bone (Layer 1)
    bone.update();
    bone.draw();

    dog.update(bone);
    dog.draw();

    // Bullets (Layer 2: Flying)
    obstacles.forEach((obs, index) => {
        if (obs instanceof Bullet) {
            obs.update();
            obs.draw();
            
            if (checkCollision(dog, obs)) {
                gameOver();
            }
            if (obs.isOffScreen()) {
                obstacles.splice(index, 1);
            }
        }
    });

    // Golds
    golds.forEach((gold, index) => {
        gold.draw();
        if (checkCollision(dog, gold)) {
            score += 10;
            document.getElementById('score').innerText = '金币: ' + score;
            golds.splice(index, 1);
        }
    });

    // Hand (Top Layer)
    hand.draw();

    requestAnimationFrame(gameLoop);
}

function startGame() {
    gameState = 'PLAYING';
    document.getElementById('start-screen').classList.add('hidden');
    document.getElementById('game-over-screen').classList.add('hidden');
    init();
    gameLoop();
}

function gameOver() {
    gameState = 'GAME_OVER';
    document.getElementById('final-score').innerText = score;
    document.getElementById('game-over-screen').classList.remove('hidden');
}

// UI Event Listeners
document.getElementById('start-btn').addEventListener('click', startGame);
document.getElementById('restart-btn').addEventListener('click', startGame);

// Initial draw
ctx.fillStyle = '#90EE90';
ctx.fillRect(0, 0, canvas.width, canvas.height);
