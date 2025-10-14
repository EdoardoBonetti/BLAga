function getRandomColor() {
    const r = Math.floor(Math.random() * 156 + 100);
    const g = Math.floor(Math.random() * 156 + 100);
    const b = Math.floor(Math.random() * 156 + 100);
    return `rgba(${r}, ${g}, ${b}, 0.7)`;
}

function getRandomSize(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

function createFlower(x, y) {
    const petalCount = 5 + Math.floor(Math.random() * 3); // 5–7 petals
    const petalSize = getRandomSize(20, 50); // size of each petal
    const flowerSize = petalSize * 2; // total container size

    // Create container for the flower
    const flower = document.createElement('div');
    flower.style.position = 'absolute';
    flower.style.width = `${flowerSize}px`;
    flower.style.height = `${flowerSize}px`;
    flower.style.left = `${x - flowerSize/2}px`;
    flower.style.top = `${y - flowerSize/2}px`;
    flower.style.pointerEvents = 'none';
    flower.style.zIndex = '9999';
    flower.style.transition = 'all 0.7s ease-out';
    flower.style.opacity = '1';

    // Add petals
    for (let i = 0; i < petalCount; i++) {
        const angle = (i / petalCount) * 2 * Math.PI;
        const petal = document.createElement('div');
        petal.style.position = 'absolute';
        petal.style.width = `${petalSize}px`;
        petal.style.height = `${petalSize}px`;
        petal.style.background = getRandomColor();
        petal.style.borderRadius = '50%';
        petal.style.left = `${flowerSize/2 + Math.cos(angle) * petalSize - petalSize/2}px`;
        petal.style.top = `${flowerSize/2 + Math.sin(angle) * petalSize - petalSize/2}px`;
        flower.appendChild(petal);
    }

    // Add center circle
    const centerSize = getRandomSize(20, 40); // random center size
    const center = document.createElement('div');
    center.style.position = 'absolute';
    center.style.width = `${centerSize}px`;
    center.style.height = `${centerSize}px`;
    center.style.background = getRandomColor();
    center.style.borderRadius = '50%';
    center.style.left = `${flowerSize/2 - centerSize/2}px`;
    center.style.top = `${flowerSize/2 - centerSize/2}px`;
    flower.appendChild(center);

    document.body.appendChild(flower);

    // Animate flower expanding and fading
    setTimeout(() => {
        const scale = getRandomSize(2, 4); // expand 2x–4x
        flower.style.transform = `scale(${scale})`;
        flower.style.opacity = '0';
    }, 10);

    setTimeout(() => document.body.removeChild(flower), 800);
}

// Initialize
setTimeout(() => {
    document.body.addEventListener('click', (e) => {
        createFlower(e.pageX, e.pageY);
    });
}, 500);
