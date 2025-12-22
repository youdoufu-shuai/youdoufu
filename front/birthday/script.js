document.addEventListener('DOMContentLoaded', () => {
    // 优先初始化核心页面逻辑
    try {
        initEnvelope();
    } catch (e) {
        console.error("Early envelope init failed", e);
    }

    // --- Music Control ---
    try {
        const bgm = document.getElementById('bgm');
        let bgmStarted = false;
        const musicDisc = document.getElementById('music-toggle');
        const musicPanel = document.getElementById('music-panel');
        const playBtn = document.getElementById('music-play-btn');
        const volumeSlider = document.getElementById('volume-slider');

        // Toggle Panel
        if (musicDisc) {
            musicDisc.addEventListener('click', (e) => {
                e.stopPropagation();
                musicPanel.classList.toggle('hidden');
            });
        }

        // Hide panel when clicking outside
        document.addEventListener('click', (e) => {
            if (musicPanel && musicDisc && !musicPanel.contains(e.target) && !musicDisc.contains(e.target)) {
                musicPanel.classList.add('hidden');
            }
        });

        // Play/Pause Logic
        function togglePlay() {
            if (bgm.paused) {
                bgm.play().then(() => {
                    bgmStarted = true;
                    if (musicDisc) musicDisc.classList.add('playing');
                    playBtn.innerText = "⏸";
                }).catch(e => console.log("Playback failed", e));
            } else {
                bgm.pause();
                if (musicDisc) musicDisc.classList.remove('playing');
                playBtn.innerText = "▶";
            }
        }

        if (playBtn) {
            playBtn.addEventListener('click', (e) => {
                e.stopPropagation();
                togglePlay();
            });
        }

        // Volume Logic
        bgm.volume = 0.5; // Set initial volume
        if (volumeSlider) {
            volumeSlider.addEventListener('input', (e) => {
                bgm.volume = e.target.value;
            });
        }

        // Auto-play attempt
        function tryPlayBgm() {
            if (!bgmStarted) {
                bgm.play().then(() => {
                    bgmStarted = true;
                    if (musicDisc) musicDisc.classList.add('playing');
                    playBtn.innerText = "⏸";
                }).catch(e => console.log("BGM autoplay prevented", e));
            }
        }
        document.body.addEventListener('click', tryPlayBgm, { once: true });
        document.body.addEventListener('touchstart', tryPlayBgm, { once: true });
    } catch (e) {
        console.error("Music initialization failed:", e);
    }

    // --- Sound Manager (Synthesized SFX) ---
    const SoundManager = {
        ctx: null,
        init: function() {
            if (!this.ctx) {
                this.ctx = new (window.AudioContext || window.webkitAudioContext)();
            }
            if (this.ctx.state === 'suspended') {
                this.ctx.resume();
            }
        },
        playBalloonPop: function() {
            this.init();
            const t = this.ctx.currentTime;
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            
            osc.frequency.setValueAtTime(500, t);
            osc.frequency.exponentialRampToValueAtTime(50, t + 0.1);
            osc.type = 'triangle';
            
            gain.gain.setValueAtTime(0.3, t);
            gain.gain.exponentialRampToValueAtTime(0.01, t + 0.1);
            
            osc.connect(gain);
            gain.connect(this.ctx.destination);
            
            osc.start(t);
            osc.stop(t + 0.1);
        },
        playFirework: function() {
            this.init();
            const t = this.ctx.currentTime;
            
            // Low boom
            const osc = this.ctx.createOscillator();
            const gain = this.ctx.createGain();
            
            osc.frequency.setValueAtTime(150, t);
            osc.frequency.exponentialRampToValueAtTime(10, t + 0.3);
            osc.type = 'square';
            
            gain.gain.setValueAtTime(0.1, t);
            gain.gain.exponentialRampToValueAtTime(0.01, t + 0.3);
            
            // Lowpass to muffle square wave
            const filter = this.ctx.createBiquadFilter();
            filter.type = 'lowpass';
            filter.frequency.value = 200;
            
            osc.connect(filter);
            filter.connect(gain);
            gain.connect(this.ctx.destination);
            
            osc.start(t);
            osc.stop(t + 0.3);
        }
    };

    // --- Page Navigation System ---
    const pages = {
        envelope: document.getElementById('section-envelope'),
        cake: document.getElementById('section-cake'),
        wish: document.getElementById('section-wish'),
        slideshow: document.getElementById('section-slideshow'),
        album: document.getElementById('section-album'),
        ending: document.getElementById('section-ending')
    };

    function showPage(pageId) {
        Object.values(pages).forEach(page => {
            page.classList.remove('active');
            setTimeout(() => {
                if (!page.classList.contains('active')) {
                    page.classList.add('hidden');
                }
            }, 800); // Wait for transition
        });

        const target = pages[pageId];
        target.classList.remove('hidden');
        // Force reflow
        void target.offsetWidth;
        target.classList.add('active');
    }

    // --- 1. Envelope Logic (Date Picker) ---
    function initEnvelope() {
        document.getElementById('env-title').innerText = config.envelope.title;
        document.getElementById('env-subtitle').innerText = config.envelope.subTitle;

        const envelopeBtn = document.getElementById('envelope-btn');
        const envelopeWrapper = document.querySelector('.envelope-wrapper'); // Get wrapper
        const modal = document.getElementById('date-picker-modal');
        const unlockBtn = document.getElementById('unlock-btn');
        const errorMsg = document.getElementById('error-msg');
        
        const monthSelect = document.getElementById('month-select');
        const daySelect = document.getElementById('day-select');

        // Check if elements exist
        if (!envelopeBtn || !modal || !monthSelect || !daySelect) {
            console.error("Critical elements missing for Envelope initialization");
            return;
        }

        // Populate Date Selectors
        const months = Array.from({length: 12}, (_, i) => i + 1);
        months.forEach(m => {
            const opt = document.createElement('option');
            opt.value = m;
            opt.innerText = m;
            if (m === new Date().getMonth() + 1) opt.selected = true;
            monthSelect.appendChild(opt);
        });

        const days = Array.from({length: 31}, (_, i) => i + 1);
        days.forEach(d => {
            const opt = document.createElement('option');
            opt.value = d;
            opt.innerText = d;
            if (d === new Date().getDate()) opt.selected = true;
            daySelect.appendChild(opt);
        });

        // Add event listener to wrapper for larger click area
        if (envelopeWrapper) {
            envelopeWrapper.addEventListener('click', (e) => {
                console.log("Envelope clicked");
                modal.classList.remove('hidden');
            });
        } else {
            // Fallback
            envelopeBtn.addEventListener('click', () => {
                console.log("Envelope btn clicked");
                modal.classList.remove('hidden');
            });
        }

        unlockBtn.addEventListener('click', checkDate);

        function checkDate() {
            const m = parseInt(monthSelect.value);
            const d = parseInt(daySelect.value);
            
            if (m === config.unlockDate.month && d === config.unlockDate.day) {
                // Success
                modal.classList.add('hidden');
                envelopeBtn.classList.add('open');
                errorMsg.classList.add('hidden');
                
                // Transition to Cake after animation
                setTimeout(() => {
                    initCake();
                    showPage('cake');
                }, 1500);
            } else {
                errorMsg.classList.remove('hidden');
                // Shake animation
                const card = document.querySelector('.calendar-card');
                card.style.animation = 'none';
                void card.offsetWidth;
                card.style.animation = 'shake 0.5s';
            }
        }
        
        // Inject shake keyframes if not exists
        if (!document.getElementById('shake-style')) {
            const style = document.createElement('style');
            style.id = 'shake-style';
            style.innerHTML = `
                @keyframes shake {
                    0% { transform: translateX(0); }
                    25% { transform: translateX(-10px); }
                    50% { transform: translateX(10px); }
                    75% { transform: translateX(-10px); }
                    100% { transform: translateX(0); }
                }
            `;
            document.head.appendChild(style);
        }
    }

    // --- 2. Cake Logic (Cinematic Lighting) ---
    function initCake() {
        startMeteorShower(); // Start meteors
        
        // Interactive Balloons
        const balloons = document.querySelectorAll('.balloon');
        balloons.forEach(b => {
            b.classList.remove('popped'); // Reset
            b.onclick = () => {
                SoundManager.playBalloonPop();
                b.classList.add('popped');
            };
        });

        document.getElementById('cake-title-en').innerText = config.cake.title;
        document.getElementById('cake-title-cn').innerText = config.cake.titleCn;
        
        const msgContainer = document.getElementById('cake-msg-container');
        msgContainer.innerHTML = '';
        config.cake.message.forEach(msg => {
            const p = document.createElement('p');
            p.innerText = msg;
            msgContainer.appendChild(p);
        });

        document.getElementById('blow-hint').innerText = ""; // Hide hint initially

        // Create Dark Overlay
        const cakeSection = document.getElementById('section-cake');
        let overlay = document.querySelector('.cake-overlay');
        if (!overlay) {
            overlay = document.createElement('div');
            overlay.className = 'cake-overlay';
            cakeSection.appendChild(overlay);
        }
        overlay.classList.remove('light-up'); // Ensure dark

        const flame = document.querySelector('.flame');
        flame.classList.remove('lit'); // Ensure unlit
        
        // Sequence: Dark -> Light Candle -> Light Up Room -> Hint
        setTimeout(() => {
            // 1. Light Candle
            flame.classList.add('lit');
            
            setTimeout(() => {
                // 2. Light Up Room
                overlay.classList.add('light-up');
                
                setTimeout(() => {
                    // 3. Show Hint
                    document.getElementById('blow-hint').innerText = config.cake.interactionHint;
                    enableBlowing();
                }, 2000);
            }, 1500);
        }, 3000); // 3s wait in dark

        function enableBlowing() {
            // Blow out candle interaction
            const candle = document.querySelector('.candle');
            let blown = false;

            function blowCandle() {
                if (blown) return;
                blown = true;
                flame.classList.remove('lit'); // Extinguish
                flame.style.opacity = '0';
                
                // Smoke effect
                createSmoke(candle.getBoundingClientRect());

                document.getElementById('blow-hint').innerText = "🎉 愿望已许下！ 🎉";
                
                setTimeout(() => {
                    initWish();
                    showPage('wish');
                }, 2000);
            }

            // Click to blow
            cakeSection.addEventListener('click', blowCandle);

            // Mic detection
            if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
                navigator.mediaDevices.getUserMedia({ audio: true })
                .then(stream => {
                    const audioContext = new (window.AudioContext || window.webkitAudioContext)();
                    const analyser = audioContext.createAnalyser();
                    const microphone = audioContext.createMediaStreamSource(stream);
                    microphone.connect(analyser);
                    analyser.fftSize = 256;
                    const bufferLength = analyser.frequencyBinCount;
                    const dataArray = new Uint8Array(bufferLength);

                    function checkAudio() {
                        if (blown) return;
                        analyser.getByteFrequencyData(dataArray);
                        let sum = 0;
                        for(let i = 0; i < bufferLength; i++) {
                            sum += dataArray[i];
                        }
                        const average = sum / bufferLength;
                        if (average > 50) { // Threshold
                            blowCandle();
                        }
                        requestAnimationFrame(checkAudio);
                    }
                    checkAudio();
                })
                .catch(e => console.log("Mic error", e));
            }
        }
    }

    function createSmoke(rect) {
        for(let i=0; i<5; i++) {
            const smoke = document.createElement('div');
            smoke.style.position = 'fixed';
            smoke.style.left = (rect.left + rect.width/2) + 'px';
            smoke.style.top = rect.top + 'px';
            smoke.style.width = '10px';
            smoke.style.height = '10px';
            smoke.style.background = 'rgba(200,200,200,0.5)';
            smoke.style.borderRadius = '50%';
            smoke.style.pointerEvents = 'none';
            smoke.style.transition = 'all 1s ease-out';
            document.body.appendChild(smoke);
            
            setTimeout(() => {
                smoke.style.transform = `translate(${Math.random()*20 - 10}px, -50px) scale(2)`;
                smoke.style.opacity = '0';
            }, 10);
            
            setTimeout(() => smoke.remove(), 1000);
        }
    }

    // --- 3. Wish Logic ---
    function initWish() {
        document.getElementById('wish-text').innerText = config.wish.text;
        document.getElementById('wish-subtext').innerText = config.wish.subText;
        
        setTimeout(() => {
                    initSlideshow();
                    showPage('slideshow');
                }, 4000);
    }

    // --- 4. Album Logic (Film Strip) ---
    /**
     * Initialize Album Section
     * Features:
     * - Auto-scrolling Film Strip Effect
     * - Tilted Layout (CSS)
     * - Interactive Lightbox
     */
    function initAlbum() {
        document.getElementById('album-title').innerText = config.album.title;
        document.getElementById('album-subtitle').innerText = config.album.subTitle;
        
        const track = document.getElementById('film-strip-track');
        track.innerHTML = ''; // Clear existing
        
        const images = config.album.images;
        
        // Double the images to create seamless loop
        // We add the full set twice. 
        // Animation moves from 0 to -50% (width of one set).
        const loopImages = [...images, ...images]; 
        
        loopImages.forEach((src, index) => {
            // Map index back to original images array for lightbox
            const realIndex = index % images.length;
            
            const frame = document.createElement('div');
            frame.className = 'film-frame';
            
            const img = document.createElement('img');
            img.src = src;
            img.loading = 'lazy';
            
            frame.appendChild(img);
            track.appendChild(frame);
            
            // Click to open lightbox
            frame.addEventListener('click', () => openLightbox(realIndex));
        });

        // Parallax Scroll Effect (Optional, keep subtle)
        const albumSection = document.getElementById('section-album');
        const albumBg = document.querySelector('.album-background');
        
        window.addEventListener('scroll', () => {
            if (!albumSection.classList.contains('hidden')) {
                const scrolled = window.scrollY;
                if(albumBg) albumBg.style.transform = `translateY(${scrolled * 0.2}px)`; 
            }
        });

        // Scroll Animation Observer (Header)
        const observer = new IntersectionObserver((entries) => {
            entries.forEach(entry => {
                if (entry.isIntersecting) {
                    entry.target.classList.add('visible');
                    observer.unobserve(entry.target);
                }
            });
        }, { threshold: 0.1 });

        const header = document.querySelector('.album-header-container');
        if (header) observer.observe(header);
        
        // Lightbox Logic
        const lightbox = document.getElementById('lightbox');
        const lbImg = document.getElementById('lightbox-img');
        const lbClose = document.getElementById('lightbox-close');
        const lbPrev = document.getElementById('lightbox-prev');
        const lbNext = document.getElementById('lightbox-next');
        const lbBackdrop = document.querySelector('.lightbox-backdrop');
        
        let currentIdx = 0;

        function openLightbox(index) {
            currentIdx = index;
            updateLightbox();
            lightbox.classList.remove('hidden');
            requestAnimationFrame(() => {
                lightbox.classList.add('active');
            });
        }

        function closeLightbox() {
            lightbox.classList.remove('active');
            setTimeout(() => {
                lightbox.classList.add('hidden');
            }, 500); 
        }

        function updateLightbox() {
            lbImg.style.opacity = '0';
            setTimeout(() => {
                lbImg.src = images[currentIdx];
                lbImg.onload = () => {
                    lbImg.style.opacity = '1';
                };
            }, 200);
        }

        lbClose.addEventListener('click', closeLightbox);
        lbBackdrop.addEventListener('click', closeLightbox);

        lbPrev.addEventListener('click', (e) => {
            e.stopPropagation();
            currentIdx = (currentIdx - 1 + images.length) % images.length;
            updateLightbox();
        });

        lbNext.addEventListener('click', (e) => {
            e.stopPropagation();
            currentIdx = (currentIdx + 1) % images.length;
            updateLightbox();
        });
        
        // Button to Ending
        if (!document.getElementById('to-ending-btn')) {
            const btn = document.createElement('button');
            btn.id = 'to-ending-btn';
            btn.innerText = "查看最后的惊喜 ➡️";
            btn.className = 'secondary-btn'; 
            btn.addEventListener('click', () => {
                initEnding();
                showPage('ending');
            });
            document.querySelector('.album-content-wrapper').appendChild(btn);
        }
    }

    // --- 5. Ending Logic (Redesigned) ---
    function initEnding() {
        document.getElementById('end-title').innerText = config.ending.title;
        
        // Render Poem
        const poemContainer = document.getElementById('poem-container');
        poemContainer.innerHTML = '';
        if (config.ending.poem) {
            config.ending.poem.forEach((line, index) => {
                const lineDiv = document.createElement('div');
                lineDiv.className = 'poem-line';
                
                // Split first char
                const headChar = line.charAt(0);
                const restText = line.substring(1);
                
                lineDiv.innerHTML = `<span class="head-char">${headChar}</span>${restText}`;
                poemContainer.appendChild(lineDiv);
                
                // Staggered Animation
                setTimeout(() => {
                    lineDiv.classList.add('visible');
                }, index * 600 + 500); // Start after page load
            });
        }

        // Render Card Body
        const cardBody = document.getElementById('card-body-text');
        cardBody.innerHTML = config.ending.cardBody.map(line => `<p>${line}</p>`).join('');
        
        document.getElementById('card-signature').innerText = config.ending.signature;
        
        // Show Card with delay
        setTimeout(() => {
            document.querySelector('.message-card').classList.add('visible');
        }, config.ending.poem.length * 600 + 1000);

        const restartBtn = document.getElementById('restart-btn');
        restartBtn.innerText = config.ending.finalButton;
        restartBtn.classList.remove('hidden');
        
        restartBtn.addEventListener('click', () => {
            location.reload();
        });

        // Trigger Fireworks
        startFireworks();
    }

    // --- Particle/Confetti System ---
    function startConfetti() {
        const canvas = document.getElementById('effects-canvas');
        const ctx = canvas.getContext('2d');
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

        const particles = [];
        const colors = ['#FF69B4', '#FFD700', '#87CEEB', '#FFB7B2'];

        for(let i=0; i<150; i++) {
            particles.push({
                x: Math.random() * canvas.width,
                y: Math.random() * canvas.height - canvas.height,
                w: Math.random() * 10 + 5,
                h: Math.random() * 10 + 5,
                color: colors[Math.floor(Math.random() * colors.length)],
                speed: Math.random() * 3 + 2,
                angle: Math.random() * 360,
                spin: Math.random() * 0.2 - 0.1
            });
        }

        function animate() {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            
            particles.forEach(p => {
                p.y += p.speed;
                p.angle += p.spin;
                
                if (p.y > canvas.height) {
                    p.y = -20;
                    p.x = Math.random() * canvas.width;
                }

                ctx.save();
                ctx.translate(p.x, p.y);
                ctx.rotate(p.angle);
                ctx.fillStyle = p.color;
                ctx.fillRect(-p.w/2, -p.h/2, p.w, p.h);
                ctx.restore();
            });

            requestAnimationFrame(animate);
        }
        animate();
    }

    // Initialize


    // --- 3.5 Slideshow Logic ---
    function initSlideshow() {
        stopMeteorShower();
        const wrapper = document.querySelector('.slideshow-wrapper');
        const textBox = document.getElementById('slideshow-text');
        const skipBtn = document.getElementById('skip-slideshow-btn');
        const images = config.album.images;
        const messages = config.slideshow.messages;
        let currentIndex = 0;
        let interval;

        wrapper.innerHTML = '';
        
        // Preload images
        images.forEach(src => {
            const img = new Image();
            img.src = src;
        });

        function showSlide(index) {
            // Remove previous active slide
            const prevSlide = wrapper.querySelector('.slide.active');
            if (prevSlide) {
                prevSlide.classList.remove('active');
                setTimeout(() => prevSlide.remove(), 1500); // Cleanup
            }

            // Create new slide
            const slide = document.createElement('div');
            slide.className = 'slide active ken-burns';
            slide.style.backgroundImage = `url(${images[index % images.length]})`;
            wrapper.appendChild(slide);

            // Update Text
            textBox.classList.remove('visible');
            setTimeout(() => {
                textBox.innerText = messages[index % messages.length] || "";
                textBox.classList.add('visible');
            }, 500);

            currentIndex = index;
        }

        // Initial Slide
        showSlide(0);

        // Auto Play
        interval = setInterval(() => {
            if (currentIndex >= images.length - 1 && currentIndex >= messages.length - 1) {
                // End of slideshow, go to album
                finishSlideshow();
            } else {
                showSlide(currentIndex + 1);
            }
        }, config.slideshow.duration);

        function finishSlideshow() {
            clearInterval(interval);
            initAlbum();
            showPage('album');
        }

        skipBtn.addEventListener('click', () => {
            finishSlideshow();
        });
    }

    // --- Shooting Stars Logic ---
    let meteorInterval;

    function startMeteorShower() {
        const container = document.querySelector('.meteor-shower');
        if (!container || meteorInterval) return;

        function createMeteor() {
            const meteor = document.createElement('div');
            meteor.className = 'shooting-star';
            
            // Random position and delay
            meteor.style.top = Math.random() * 50 + '%';
            meteor.style.left = Math.random() * 100 + '%';
            meteor.style.animationDuration = (Math.random() * 2 + 1) + 's';
            
            container.appendChild(meteor);

            // Remove after animation
            setTimeout(() => {
                meteor.remove();
            }, 3000);
        }

        // Create meteors periodically
        meteorInterval = setInterval(createMeteor, 2000);
    }

    function stopMeteorShower() {
        if (meteorInterval) {
            clearInterval(meteorInterval);
            meteorInterval = null;
        }
        const container = document.querySelector('.meteor-shower');
        if (container) container.innerHTML = '';
    }

    // --- Fireworks Logic ---
    function startFireworks() {
        const canvas = document.getElementById('effects-canvas');
        const ctx = canvas.getContext('2d');
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;

        let particles = [];

        function createFirework(x, y) {
            const count = 50;
            const color = `hsl(${Math.random() * 360}, 100%, 50%)`;
            for (let i = 0; i < count; i++) {
                particles.push({
                    x: x,
                    y: y,
                    vx: Math.cos(i * 2 * Math.PI / count) * Math.random() * 5,
                    vy: Math.sin(i * 2 * Math.PI / count) * Math.random() * 5,
                    alpha: 1,
                    color: color
                });
            }
        }

        function animate() {
            // Semi-transparent clear for trails
            ctx.globalCompositeOperation = 'destination-out';
            ctx.fillStyle = 'rgba(0, 0, 0, 0.1)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            ctx.globalCompositeOperation = 'source-over';

            particles.forEach((p, index) => {
                p.x += p.vx;
                p.y += p.vy;
                p.vy += 0.05; // Gravity
                p.alpha -= 0.01;

                ctx.globalAlpha = p.alpha;
                ctx.fillStyle = p.color;
                ctx.beginPath();
                ctx.arc(p.x, p.y, 2, 0, Math.PI * 2);
                ctx.fill();

                if (p.alpha <= 0) {
                    particles.splice(index, 1);
                }
            });

            if (particles.length > 0 || Math.random() < 0.05) { // Keep loop alive or restart randomly
                requestAnimationFrame(animate);
            }
        }

        // Random fireworks loop
        setInterval(() => {
            if (document.getElementById('section-ending').classList.contains('active')) {
                createFirework(
                    Math.random() * canvas.width,
                    Math.random() * canvas.height * 0.5
                );
                // 30% chance to play sound to avoid noise spam
                if (Math.random() < 0.3) SoundManager.playFirework();
                
                requestAnimationFrame(animate);
            }
        }, 800);
    }
});
