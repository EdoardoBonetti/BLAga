// === Neonpunk Cursor Glow + Click Explosion ===

document.addEventListener("mousemove", (e) => {
  const glow = document.createElement("div");
  glow.className = "cursor-glow";
  glow.style.left = `${e.pageX}px`;
  glow.style.top = `${e.pageY}px`;
  document.body.appendChild(glow);

  setTimeout(() => glow.remove(), 500);
});

document.addEventListener("click", (e) => {
  for (let i = 0; i < 15; i++) {
    const particle = document.createElement("div");
    particle.className = "click-particle";
    document.body.appendChild(particle);

    const size = Math.random() * 6 + 4;
    particle.style.width = `${size}px`;
    particle.style.height = `${size}px`;

    const x = e.pageX;
    const y = e.pageY;
    particle.style.left = `${x}px`;
    particle.style.top = `${y}px`;

    const angle = Math.random() * 2 * Math.PI;
    const distance = Math.random() * 100 + 20;
    const dx = Math.cos(angle) * distance;
    const dy = Math.sin(angle) * distance;

    particle.animate([
      { transform: `translate(0, 0)`, opacity: 1 },
      { transform: `translate(${dx}px, ${dy}px)`, opacity: 0 }
    ], {
      duration: 700 + Math.random() * 400,
      easing: "ease-out"
    });

    setTimeout(() => particle.remove(), 1000);
  }
});
