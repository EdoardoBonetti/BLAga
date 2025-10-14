document.addEventListener('mousemove', e => {
  const x = e.clientX / window.innerWidth - 0.5;
  const y = e.clientY / window.innerHeight - 0.5;
  document.body.style.backgroundPosition = `${50 + x*10}% ${50 + y*10}%`;
});
