function getRandomColor() {
    // soft pastel colors for hearts
    const colors = ['#ff5e5e', '#ff95a1', '#ffb3c1', '#ff7eb9', '#ff4d94'];
    return colors[Math.floor(Math.random() * colors.length)];
}

function createHeart(x, y) {
    const heart = document.createElement('div');
    heart.innerText = '❤️'; // you can also use '<3' if you prefer
    heart.style.position = 'absolute';
    heart.style.left = `${x}px`;
    heart.style.top = `${y}px`;
    heart.style.fontSize = `${Math.floor(Math.random() * 20 + 15)}px`; // random size 15–35px
    heart.style.color = getRandomColor();
    heart.style.pointerEvents = 'none';
    heart.style.userSelect = 'none';
    heart.style.opacity = '1';
    heart.style.transition = 'all 1s ease-out';
    heart.style.zIndex = '9999';

    document.body.appendChild(heart);

    // Animate the heart: float up and fade out
    setTimeout(() => {
        const offsetX = Math.random() * 40 - 20; // drift left/right
        const offsetY = -Math.random() * 100 - 50; // float upward
        heart.style.transform = `translate(${offsetX}px, ${offsetY}px) scale(1.5) rotate(${Math.random()*30-15}deg)`;
        heart.style.opacity = '0';
    }, 10);

    // Remove after animation
    setTimeout(() => document.body.removeChild(heart), 1000);
}

// Initialize cursor hearts
setTimeout(() => {
    document.addEventListener('mousemove', (e) => {
        createHeart(e.pageX, e.pageY);
    });
}, 500);
