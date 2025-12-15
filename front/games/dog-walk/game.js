const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');

// Game Configuration
const CONFIG = {
    LEVEL_1_GOAL: 100,
    LEVEL_2_GOAL: 200, // Total score needed
    BOSS_COINS_NEEDED: 10,
    BOSS_HITS_NEEDED: 10,
    BOSS_HP: 10,
    PLAYER_MAX_HP: 3,
    BOSS_SLOW_DURATION: 300 // 5 seconds at 60fps
};

// Game State
let currentState = 'LOADING'; // LOADING, TUTORIAL, PLAYING, LEVEL_TRANSITION, BOSS_FIGHT, GAME_OVER, VICTORY
let currentLevel = 1;
let score = 0;
let frameCount = 0;
let startTime = 0;

// Boss State
let bossState = {
    active: false,
    hp: 10,
    coinsCollected: 0,
    hitsTaken: 0,
    x: 0, 
    y: 0,
    vx: 0, 
    vy: 0,
    width: 120,
    height: 120,
    moveTimer: 0,
    action: 'IDLE' // IDLE, CHASE_BONE, RANDOM_MOVE
};

const bgmCommon = new Audio('assets/common.mp3');
bgmCommon.loop = true;
const bgmBoss = new Audio('assets/boss1.mp3');
bgmBoss.loop = true;

// Leaderboard Helper
function loadLeaderboard() {
    try {
        return JSON.parse(localStorage.getItem('dogWalkLeaderboard')) || [];
    } catch (e) {
        return [];
    }
}

function saveLeaderboard(name, hp) {
    const list = loadLeaderboard();
    list.push({ name, hp, date: new Date().toLocaleDateString() });
    list.sort((a, b) => b.hp - a.hp); // Sort by HP descending
    localStorage.setItem('dogWalkLeaderboard', JSON.stringify(list));
    renderLeaderboard();
}

function renderLeaderboard() {
    const list = loadLeaderboard();
    const ul = document.getElementById('leaderboard-list');
    if (ul) {
        ul.innerHTML = '';
        list.forEach((item, index) => {
            const li = document.createElement('li');
            li.textContent = `No.${index + 1} ${item.name} (剩余血量: ${item.hp})`;
            ul.appendChild(li);
        });
    }
}

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

// Mouse Events
window.addEventListener('mousedown', (e) => updateInput(e.clientX, e.clientY, true));
window.addEventListener('mousemove', (e) => updateInput(e.clientX, e.clientY, null));
window.addEventListener('mouseup', () => updateInput(mouse.x, mouse.y, false));

// Touch Events
window.addEventListener('touchstart', (e) => {
    if (e.target === canvas) {
        e.preventDefault();
        updateInput(e.touches[0].clientX, e.touches[0].clientY, true);
    }
}, { passive: false });
window.addEventListener('touchmove', (e) => {
    if (e.target === canvas) {
        e.preventDefault();
        updateInput(e.touches[0].clientX, e.touches[0].clientY, null);
    }
}, { passive: false });
window.addEventListener('touchend', (e) => {
    if (e.target === canvas) {
        e.preventDefault(); // Prevent click emulation
        updateInput(mouse.x, mouse.y, false);
    }
});

const assets = {
    dog: { src: 'assets/dog-removebg-preview.png' },
    bone: { src: 'assets/bone-removebg-preview.png' },
    gold: { src: 'assets/glod-removebg-preview.png' }, 
    shou: { src: 'assets/shou-removebg-preview.png' },
    shouCatch: { src: 'assets/shouCatch-removebg-preview.png' },
    sword: { src: 'assets/sword-removebg-preview.png' },
    xianjing1: { src: 'assets/xianjing1-removebg-preview.png' },
    xianjing2: { src: 'assets/xianjing2-removebg-preview.png' },
    zidan: { src: 'assets/zidan-removebg-preview.png' },
    zidan1: { src: 'assets/zidan1-removebg-preview.png' },
    caodi: { src: 'assets/caodi.png' },
    shatan: { src: 'assets/shatan.png' },
    nitan: { src: 'assets/nitan.png' },
    shuitan: { src: 'assets/shuitan.png' },
    boss: { src: 'assets/boss1.png' }
};

const images = {};
let assetsLoaded = 0;
const totalAssets = Object.keys(assets).length;

// Preload Assets
function loadAssets() {
    const loadingBar = document.getElementById('loading-bar');
    
    Object.keys(assets).forEach(key => {
        const img = new Image();
        img.src = assets[key].src;
        img.onload = () => {
            assetsLoaded++;
            const percent = (assetsLoaded / totalAssets) * 100;
            if (loadingBar) loadingBar.style.width = `${percent}%`;
            
            if (assetsLoaded === totalAssets) {
                setTimeout(() => {
                    document.getElementById('loading-screen').classList.add('hidden');
                    showTutorial();
                }, 500);
            }
        };
        img.onerror = () => {
            console.warn(`Failed to load ${assets[key].src}`);
            img.isMissing = true;
            assetsLoaded++; // Still proceed
        };
        images[key] = img;
    });
}

// --- UI Management ---
function showTutorial() {
    currentState = 'TUTORIAL';
    document.getElementById('tutorial-screen').classList.remove('hidden');
}

function startGame() {
    document.getElementById('tutorial-screen').classList.add('hidden');
    document.getElementById('score-board').classList.remove('hidden');
    
    // Play Common BGM
    bgmCommon.currentTime = 0;
    bgmCommon.play().catch(e => console.log("Audio play failed:", e));
    
    initLevel(1);
}

function showLevelComplete(msg) {
    currentState = 'LEVEL_TRANSITION';
    document.getElementById('level-msg').innerText = msg;
    document.getElementById('level-complete-screen').classList.remove('hidden');
}

function nextLevel() {
    document.getElementById('level-complete-screen').classList.add('hidden');
    if (currentLevel === 1) initLevel(2);
    else if (currentLevel === 2) initBossLevel();
}

function gameOver() {
    bgmCommon.pause();
    bgmBoss.pause();
    currentState = 'GAME_OVER';
    document.getElementById('final-score').innerText = score;
    document.getElementById('game-over-screen').classList.remove('hidden');
}

function showVictory() {
    bgmBoss.pause();
    currentState = 'VICTORY';
    document.getElementById('victory-screen').classList.remove('hidden');
    
    // Leaderboard Setup
    const nameInput = document.getElementById('player-name');
    if (nameInput) nameInput.value = '';
    const submitBtn = document.getElementById('submit-score-btn');
    if (submitBtn) submitBtn.disabled = false;
    
    renderLeaderboard();
}

// --- Game Logic ---

// Helper: Collision
function checkCollision(circle1, circle2) {
    const dx = circle1.x - circle2.x;
    const dy = circle1.y - circle2.y;
    const distance = Math.sqrt(dx * dx + dy * dy);
    return distance < (circle1.radius + circle2.radius);
}

function checkRectCollision(rect1, rect2) {
    return (rect1.x - rect1.width/2 < rect2.x + rect2.width/2 &&
            rect1.x + rect1.width/2 > rect2.x - rect2.width/2 &&
            rect1.y - rect1.height/2 < rect2.y + rect2.height/2 &&
            rect1.y + rect1.height/2 > rect2.y - rect2.height/2);
}


// Entities
class Hand {
    constructor() {
        this.width = 80;
        this.height = 80;
    }
    draw() {
        const x = mouse.x;
        const y = mouse.y;
        const img = mouse.down ? images.shouCatch : images.shou;
        if (img && !img.isMissing) {
            ctx.drawImage(img, x - this.width/2, y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = 'rgba(0, 0, 255, 0.5)';
            ctx.beginPath(); ctx.arc(x, y, 30, 0, Math.PI*2); ctx.fill();
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
        this.radius = 15;
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
        if (images.bone && !images.bone.isMissing) {
            ctx.drawImage(images.bone, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = '#eee';
            ctx.beginPath(); 
            ctx.ellipse(this.x, this.y, 20, 10, 0, 0, Math.PI*2); 
            ctx.fill();
            ctx.strokeStyle = '#ccc'; ctx.stroke();
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
        this.radius = 25;
        this.facingRight = true;
        this.speedModifier = 1.0;
        this.isSlowed = false;
        
        // New mechanics
        this.hp = 1; // Default 1, set to 3 in Boss level
        this.invincibleTimer = 0;
    }
    
    takeDamage() {
        if (this.invincibleTimer > 0) return false;
        
        this.hp--;
        if (this.hp > 0) {
            this.invincibleTimer = 120; // 2 seconds invincibility
            return false; // Not dead yet
        }
        return true; // Dead
    }

    update(bone) {
        if (this.invincibleTimer > 0) this.invincibleTimer--;
        
        let speed = this.baseSpeed * this.speedModifier;
        if (this.isSlowed) speed *= 0.5;

        if (bone.isHeld) {
            this.facingRight = bone.x > this.x;
        } else {
            const dx = bone.x - this.x;
            const dy = bone.y - this.y;
            const dist = Math.sqrt(dx*dx + dy*dy);
            
            if (dist > 10) {
                const moveX = (dx / dist) * speed;
                const moveY = (dy / dist) * speed;
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
        // Blink if invincible
        if (this.invincibleTimer > 0 && Math.floor(frameCount / 4) % 2 === 0) return;

        if (images.dog && !images.dog.isMissing) {
            ctx.save();
            ctx.translate(this.x, this.y);
            if (!this.facingRight) ctx.scale(-1, 1);
            ctx.drawImage(images.dog, -this.width/2, -this.height/2, this.width, this.height);
            // Draw slow effect
            if (this.isSlowed) {
                ctx.fillStyle = 'rgba(139, 69, 19, 0.5)'; // Brown tint
                ctx.beginPath(); ctx.arc(0, 20, 10, 0, Math.PI*2); ctx.fill();
            }
            ctx.restore();
        } else {
            ctx.fillStyle = '#8B4513';
            ctx.beginPath(); ctx.arc(this.x, this.y, this.radius, 0, Math.PI*2); ctx.fill();
            // Eyes to show direction
            ctx.fillStyle = 'white';
            const eyeOffset = this.facingRight ? 10 : -10;
            ctx.beginPath(); ctx.arc(this.x + eyeOffset, this.y - 10, 5, 0, Math.PI*2); ctx.fill();
        }
    }
}

class Terrain {
    constructor(type) {
        this.type = type; // 'nitan', 'shuitan'
        this.x = Math.random() * (canvas.width - 100) + 50;
        this.y = Math.random() * (canvas.height - 100) + 50;
        this.width = 80;
        this.height = 80;
        this.radius = 35;
    }
    draw() {
        const img = images[this.type];
        if (img && !img.isMissing) {
            ctx.drawImage(img, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        }
    }
}

class Trap {
    constructor(type) {
        this.type = type; 
        this.x = Math.random() * (canvas.width - 100) + 50;
        this.y = Math.random() * (canvas.height - 100) + 50;
        this.width = 60;
        this.height = 60;
        this.radius = 25;
        this.state = 'WARNING'; 
        this.warningTimer = (currentLevel === 'BOSS' || currentLevel === 2) ? 90 : 120; // Faster in later levels
        this.activeTimer = 300; 
        this.angle = 0;
    }
    update() {
        if (this.state === 'WARNING') {
            this.warningTimer--;
            if (this.warningTimer <= 0) this.state = 'ACTIVE';
        } else if (this.state === 'ACTIVE') {
            this.activeTimer--;
            if (this.type === 'sword') this.angle += 0.2;
        }
    }
    draw() {
        if (this.state === 'WARNING') {
            ctx.save();
            ctx.translate(this.x, this.y);
            ctx.beginPath(); ctx.arc(0, 0, this.radius + 10, 0, Math.PI * 2);
            ctx.fillStyle = `rgba(255, 0, 0, ${0.3 + Math.sin(frameCount * 0.2) * 0.2})`; 
            ctx.fill();
            ctx.strokeStyle = 'red'; ctx.lineWidth = 2; ctx.stroke();
            const img = images[this.type];
            if (img && !img.isMissing) {
                ctx.globalAlpha = 0.5;
                ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
                ctx.globalAlpha = 1.0;
            }
            ctx.restore();
        } else if (this.state === 'ACTIVE') {
            const img = images[this.type];
            if (img && !img.isMissing) {
                ctx.save();
                ctx.translate(this.x, this.y);
                if (this.type === 'sword') ctx.rotate(this.angle);
                ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
                ctx.restore();
            }
        }
    }
    isDangerous() { return this.state === 'ACTIVE'; }
}

class Bullet {
    constructor(type) {
        this.type = type;
        this.radius = 15;
        this.width = 40;
        this.height = 20;
        
        // Warning System
        this.state = 'WARNING';
        this.warningTimer = 120; // 2 seconds

        // Determine Spawn & Target
        const side = Math.floor(Math.random() * 4); 
        // 0:Top, 1:Right, 2:Bottom, 3:Left
        
        // Set start position OUTSIDE screen
        switch(side) {
            case 0: this.x = Math.random() * canvas.width; this.y = -50; break;
            case 1: this.x = canvas.width + 50; this.y = Math.random() * canvas.height; break;
            case 2: this.x = Math.random() * canvas.width; this.y = canvas.height + 50; break;
            case 3: this.x = -50; this.y = Math.random() * canvas.height; break;
        }

        const targetX = Math.random() * canvas.width;
        const targetY = Math.random() * canvas.height;
        const angle = Math.atan2(targetY - this.y, targetX - this.x);
        
        const diffMultiplier = 1 + (score / 500); // Simple difficulty scaling
        const speed = (type === 'zidan1' ? 8 : 4) * diffMultiplier;
        
        this.vx = Math.cos(angle) * speed;
        this.vy = Math.sin(angle) * speed;
        this.rotation = angle;
        
        // Warning Indicator Position (Clamped to screen edge)
        this.indicatorX = Math.max(20, Math.min(canvas.width - 20, this.x));
        this.indicatorY = Math.max(20, Math.min(canvas.height - 20, this.y));
        if (side === 0) this.indicatorY = 30;
        if (side === 1) this.indicatorX = canvas.width - 30;
        if (side === 2) this.indicatorY = canvas.height - 30;
        if (side === 3) this.indicatorX = 30;
    }

    update() {
        if (this.state === 'WARNING') {
            this.warningTimer--;
            if (this.warningTimer <= 0) this.state = 'ACTIVE';
        } else {
            this.x += this.vx;
            this.y += this.vy;
        }
    }

    draw() {
        if (this.state === 'WARNING') {
            // Draw Exclamation Mark with Direction
            ctx.fillStyle = `rgba(255, 0, 0, ${0.5 + Math.sin(frameCount * 0.3) * 0.5})`;
            ctx.beginPath();
            ctx.arc(this.indicatorX, this.indicatorY, 20, 0, Math.PI*2);
            ctx.fill();
            
            // Draw Arrow
            ctx.save();
            ctx.translate(this.indicatorX, this.indicatorY);
            ctx.rotate(this.rotation);
            ctx.fillStyle = 'white';
            ctx.beginPath();
            ctx.moveTo(10, 0);
            ctx.lineTo(-5, 5);
            ctx.lineTo(-5, -5);
            ctx.fill();
            ctx.restore();
        } else {
            const img = images[this.type];
            if (img && !img.isMissing) {
                ctx.save();
                ctx.translate(this.x, this.y);
                ctx.rotate(this.rotation);
                ctx.drawImage(img, -this.width/2, -this.height/2, this.width, this.height);
                ctx.restore();
            }
        }
    }
    
    isDangerous() { return this.state === 'ACTIVE'; }
    isOffScreen() {
        return (this.state === 'ACTIVE') && 
               (this.x < -100 || this.x > canvas.width + 100 || 
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
        if (images.gold && !images.gold.isMissing) {
            ctx.drawImage(images.gold, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = 'gold';
            ctx.beginPath();
            ctx.arc(this.x, this.y, this.radius, 0, Math.PI * 2);
            ctx.fill();
            ctx.strokeStyle = 'orange';
            ctx.lineWidth = 2;
            ctx.stroke();
        }
    }
}

class Boss {
    constructor() {
        this.x = canvas.width / 2;
        this.y = 100;
        this.width = 150;
        this.height = 150;
        this.radius = 60;
        this.speed = 1.5; // Half of Dog's baseSpeed (3)
        this.moveTimer = 0;
        this.targetX = this.x;
        this.targetY = this.y;
        this.isSlowed = false;
        this.slowTimer = 0;
    }
    
    takeKnockback() {
        // Push boss to the nearest edge or just away
        // Simple: push to top or a safe distance
        this.targetY = 50;
        this.targetX = canvas.width / 2;
        this.x = this.targetX;
        this.y = this.targetY;
        this.moveTimer = 60; // Stun briefly?
    }

    update(bone, golds, terrains) {
        // Handle Slow Status
        if (this.isSlowed) {
            this.slowTimer--;
            if (this.slowTimer <= 0) this.isSlowed = false;
        }

        // Terrain Interaction
        if (terrains) {
            terrains.forEach(t => {
                if (t.type === 'nitan' && checkCollision(this, t)) {
                    this.isSlowed = true;
                    this.slowTimer = CONFIG.BOSS_SLOW_DURATION;
                }
            });
        }

        let currentSpeed = this.speed;
        if (this.isSlowed) currentSpeed *= 0.5;

        // State Logic
        if (this.moveTimer > 0) {
            this.moveTimer--;
            // Random Move
        } else {
            // Check for bone (priority)
            if (!bone.isHeld) {
                this.targetX = bone.x;
                this.targetY = bone.y;
                
                // Eat Bone Check
                if (checkCollision(this, bone)) {
                    // Boss eats bone logic? "Eats bone then random move 2s"
                    // We can't actually destroy the bone, maybe just push it or trigger effect
                    // Requirement: "Eat bone -> random move 2s"
                    this.moveTimer = 120; // 2s
                    this.targetX = Math.random() * (canvas.width - 100) + 50;
                    this.targetY = Math.random() * (canvas.height - 100) + 50;
                }
            } else {
                // Bone held, maybe chase random gold or player?
                // Default: Chase nearest gold to heal
                if (golds.length > 0) {
                    // Find nearest
                    let nearest = golds[0];
                    let minD = 9999;
                    golds.forEach(g => {
                        const d = Math.hypot(g.x - this.x, g.y - this.y);
                        if (d < minD) { minD = d; nearest = g; }
                    });
                    this.targetX = nearest.x;
                    this.targetY = nearest.y;
                } else {
                    // Idle move
                    if (Math.random() < 0.02) {
                         this.targetX = Math.random() * (canvas.width - 100) + 50;
                         this.targetY = Math.random() * (canvas.height - 100) + 50;
                    }
                }
            }
        }

        // Move
        const dx = this.targetX - this.x;
        const dy = this.targetY - this.y;
        const dist = Math.sqrt(dx*dx + dy*dy);
        if (dist > 5) {
            this.x += (dx/dist) * currentSpeed;
            this.y += (dy/dist) * currentSpeed;
        }

        // Heal Check (Collision with gold)
        // Handled in main loop for array splicing
    }

    draw() {
        if (images.boss && !images.boss.isMissing) {
            ctx.drawImage(images.boss, this.x - this.width/2, this.y - this.height/2, this.width, this.height);
        } else {
            ctx.fillStyle = 'purple';
            ctx.beginPath(); ctx.arc(this.x, this.y, this.radius, 0, Math.PI*2); ctx.fill();
        }
        
        // HP Bar
        ctx.fillStyle = 'red';
        ctx.fillRect(this.x - 50, this.y - 80, 100, 10);
        ctx.fillStyle = 'green';
        ctx.fillRect(this.x - 50, this.y - 80, 100 * (bossState.hp / CONFIG.BOSS_HP), 10);
    }
}


// Global Instances
let hand, bone, dog, boss;
let obstacles = [];
let golds = [];
let terrains = [];

function initLevel(level) {
    currentLevel = level;
    currentState = (level === 'BOSS') ? 'BOSS_FIGHT' : 'PLAYING';
    
    hand = new Hand();
    bone = new Bone();
    dog = new Dog();
    obstacles = [];
    golds = [];
    terrains = [];
    
    if (level === 1) {
        score = 0; 
        dog.hp = 1;
    }
    
    if (level === 2) {
        dog.hp = 1;
        // Initial Terrains
        for(let i=0; i<3; i++) terrains.push(new Terrain('nitan'));
        for(let i=0; i<2; i++) terrains.push(new Terrain('shuitan'));
    }
    
    if (level === 'BOSS') {
        initBoss();
        dog.hp = CONFIG.PLAYER_MAX_HP;
    } else {
        document.getElementById('boss-stats').classList.add('hidden');
    }

    document.getElementById('level-info').innerText = level === 'BOSS' ? 'Boss战' : `关卡: ${level}`;
    document.getElementById('score').innerText = `金币: ${score}`;
    
    updateUI();
}

function initBossLevel() {
    bgmCommon.pause();
    bgmBoss.currentTime = 0;
    bgmBoss.play().catch(e => console.log("Audio play failed:", e));
    initLevel('BOSS');
}

function initBoss() {
    boss = new Boss();
    bossState.hp = CONFIG.BOSS_HP;
    bossState.coinsCollected = 0;
    bossState.hitsTaken = 0;
    document.getElementById('boss-stats').classList.remove('hidden');
    updateBossUI();
}

function updateBossUI() {
    document.getElementById('boss-hp').innerText = bossState.hp;
    document.getElementById('boss-hits').innerText = bossState.hitsTaken;
    document.getElementById('boss-coins').innerText = bossState.coinsCollected;
}

function updateUI() {
    document.getElementById('score').innerText = `金币: ${score}`;
}

function gameLoop() {
    if (currentState !== 'PLAYING' && currentState !== 'BOSS_FIGHT') {
        requestAnimationFrame(gameLoop);
        return;
    }

    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // 1. Draw Background
    let bgImg = images.caodi;
    if (currentLevel === 2 || currentLevel === 'BOSS') bgImg = images.shatan;
    
    if (bgImg && !bgImg.isMissing) {
        // Tiled or Stretched? Stretched for mobile usually better for simplicity
        ctx.drawImage(bgImg, 0, 0, canvas.width, canvas.height);
    } else {
        ctx.fillStyle = (currentLevel === 1) ? '#90EE90' : '#F4A460';
        ctx.fillRect(0, 0, canvas.width, canvas.height);
    }

    frameCount++;

    // 2. Spawner
    let spawnRate = 60;
    if (currentState === 'BOSS_FIGHT') spawnRate = 40; // Faster spawns in Boss fight

    if (frameCount % spawnRate === 0) { 
        // Gold
        if (golds.length < (currentState === 'BOSS_FIGHT' ? 8 : 5)) {
             if (Math.random() < 0.4) golds.push(new Gold());
        }
        
        // Obstacles
        let obstacleChance = 0.6;
        if (currentState === 'BOSS_FIGHT') obstacleChance = 0.8; // More traps in Boss fight

        if (Math.random() < obstacleChance) {
             const type = Math.random() > 0.5 ? 'zidan' : 'xianjing1'; 
             if (type === 'zidan') obstacles.push(new Bullet(Math.random()>0.8?'zidan1':'zidan'));
             else obstacles.push(new Trap(Math.random()>0.5?'xianjing1':'xianjing2'));
        }
        
        // Random Terrain Spawn (Mud/Water)
        // Allowed in L2 and Boss Fight
        if (currentLevel === 2 || currentState === 'BOSS_FIGHT') {
            if (Math.random() < 0.2 && terrains.length < 10) {
                const type = Math.random() > 0.6 ? 'nitan' : 'shuitan';
                terrains.push(new Terrain(type));
            }
        }
    }

    // 3. Updates & Draws
    
    // Terrains (Bottom)
    terrains.forEach((t, idx) => {
        t.draw();
        // Interaction
        if (checkCollision(dog, t)) {
            if (t.type === 'nitan') dog.isSlowed = true;
            if (t.type === 'shuitan') dog.isSlowed = false;
        }
        // Remove old terrains if too many? Or just let them be. 
        // Maybe remove if boss fight to keep arena changing?
        // For now keep simple.
    });
    
    // Traps
    obstacles.forEach((obs, index) => {
        obs.update();
        obs.draw();
        
        if (obs.isDangerous()) {
            if (checkCollision(dog, obs)) {
                if (dog.takeDamage()) {
                    gameOver();
                } else {
                    // Just took damage, update UI
                    updateUI();
                }
            }
            // Boss Hit Logic
            if (currentState === 'BOSS_FIGHT' && boss && checkRectCollision(boss, obs)) {
                // Boss hit by trap/bullet
                bossState.hitsTaken++;
                bossState.hp--; 
                if (bossState.hp < 0) bossState.hp = 0;
                
                updateBossUI();
                obstacles.splice(index, 1);
                return; // Obs destroyed
            }
        }
        
        if (obs.activeTimer <= 0 || (obs.isOffScreen && obs.isOffScreen())) {
            obstacles.splice(index, 1);
        }
    });

    // Boss
    if (currentState === 'BOSS_FIGHT' && boss) {
        boss.update(bone, golds, terrains);
        boss.draw();
        
        // Boss vs Dog
        if (checkCollision(boss, dog)) {
            if (dog.takeDamage()) {
                gameOver();
            } else {
                boss.takeKnockback();
                updateUI();
            }
        }
        
        // Boss vs Gold (Heal)
        golds.forEach((g, i) => {
            if (checkCollision(boss, g)) {
                if (bossState.hp < CONFIG.BOSS_HP) bossState.hp++;
                golds.splice(i, 1);
                updateBossUI();
            }
        });
        
        // Win Condition Check
        if (bossState.coinsCollected >= CONFIG.BOSS_COINS_NEEDED && 
            bossState.hitsTaken >= CONFIG.BOSS_HITS_NEEDED) {
            showVictory();
        }
    }

    // Entities
    bone.update();
    bone.draw();
    dog.update(bone);
    dog.draw();
    hand.draw();

    // Golds
    golds.forEach((gold, index) => {
        gold.draw();
        if (checkCollision(dog, gold)) {
            score += 10;
            updateUI();
            
            if (currentState === 'BOSS_FIGHT') {
                bossState.coinsCollected++;
                updateBossUI();
            }
            
            golds.splice(index, 1);
            
            // Level Progression
            if (currentState === 'PLAYING') {
                if (currentLevel === 1 && score >= CONFIG.LEVEL_1_GOAL) showLevelComplete('草地关卡完成！即将前往沙滩...');
                if (currentLevel === 2 && score >= CONFIG.LEVEL_2_GOAL) showLevelComplete('沙滩关卡完成！Boss战即将开始！');
            }
        }
    });

    requestAnimationFrame(gameLoop);
}


// Buttons
document.getElementById('start-game-btn').addEventListener('click', startGame);
document.getElementById('skip-tutorial-btn').addEventListener('click', startGame);
document.getElementById('next-level-btn').addEventListener('click', nextLevel);
document.getElementById('restart-btn').addEventListener('click', () => {
    document.getElementById('game-over-screen').classList.add('hidden');
    startGame();
});
document.getElementById('home-btn').addEventListener('click', () => {
    window.location.href = '../../index.html';
});

document.getElementById('submit-score-btn').addEventListener('click', () => {
    const nameInput = document.getElementById('player-name');
    const name = nameInput.value.trim();
    if (name) {
        saveLeaderboard(name, dog.hp);
        nameInput.value = '';
        document.getElementById('submit-score-btn').disabled = true;
        alert('成绩已提交！');
    } else {
        alert('请输入名字！');
    }
});

// Init
loadAssets();
gameLoop();
