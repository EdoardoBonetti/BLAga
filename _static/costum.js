// _static/custom.js
function setTheme(theme) {
    document.body.dataset.theme = theme;
    localStorage.setItem("theme", theme);
}

// Load saved theme on page load
document.addEventListener("DOMContentLoaded", () => {
    const theme = localStorage.getItem("theme") || "light";
    setTheme(theme);
});

// Add your custom button to switch themes
const switchButton = document.createElement("button");
switchButton.innerText = "Insane Mode";
switchButton.onclick = () => setTheme("insane");
document.body.appendChild(switchButton);
