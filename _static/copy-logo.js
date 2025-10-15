(function() {
  const logoSrc = "_static/dino.jpeg";  // your logo path

  let lastMouse = { x: window.innerWidth / 2, y: window.innerHeight / 2 };
  document.addEventListener("mousemove", e => {
    lastMouse = { x: e.pageX, y: e.pageY };
  });

  document.addEventListener("copy", () => {
    const { x, y } = lastMouse;
    const img = document.createElement("img");
    img.src = logoSrc;

    img.style.position = "absolute";
    img.style.left = `${x}px`;
    img.style.top = `${y}px`;
    img.style.width = "80px";  // starting size
    img.style.pointerEvents = "none";
    img.style.opacity = "1";
    img.style.transform = "translate(-50%, -50%) scale(1) rotate(0deg)";
    img.style.transition = "transform 3s cubic-bezier(0.2, 0.8, 0.2, 1), opacity 3s ease-out";
    img.style.zIndex = "99999";
    img.style.filter = "drop-shadow(0 0 10px #0ff) drop-shadow(0 0 20px #f0f)";

    document.body.appendChild(img);

    // animate explosion
    setTimeout(() => {
      img.style.transform = "translate(-50%, -50%) scale(8) rotate(1440deg)";
      img.style.opacity = "0";
    }, 10);

    // remove after animation
    setTimeout(() => img.remove(), 1600);
  });
})();
