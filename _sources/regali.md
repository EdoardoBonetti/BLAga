```{raw} html
<!-- Secret Santa Widget -->
<h2>Secret Santa — Inserisci il tuo codice</h2>

<style>
input,button { font-size: 18px; padding: 6px; }
#result { margin-top: 20px; font-size: 20px; font-weight: bold; }
#status { margin-top: 10px; font-style: italic; color: gray; }
</style>

<input id="codeInput" type="text" placeholder="Es. AB10">
<button onclick="checkCode()">Mostra destinatario</button>

<div id="result"></div>
<div id="status"></div>

<script>
// Load JSON once at page load
let assignments = {};

fetch("_static/assignments.json")  // adjust path if needed
    .then(r => r.json())
    .then(data => {
        console.log("Loaded assignments:", data);
        assignments = data;
        document.getElementById("status").innerHTML = "Assignments loaded ✅";
    })
    .catch(err => {
        console.error("Failed to load JSON", err);
        document.getElementById("status").innerHTML = "<span style='color:red'>Failed to load assignments!</span>";
    });

function decodeBase64(str) {
    return atob(str);
}

function checkCode() {
    const code = document.getElementById("codeInput").value.trim();
    const result = document.getElementById("result");
    const status = document.getElementById("status");

    if (!assignments[code]) {
        result.innerHTML = "<span style='color:red'>Codice non valido 😢</span>";
        status.innerHTML = `Il codice "${code}" non è stato trovato.`;
        return;
    }

    const person = assignments[code]["name"];
    const giftee = decodeBase64(assignments[code]["giftee"]);

    result.innerHTML = `Ciao <b>${person}</b>! 🎁 Devi fare un regalo a <b style="color:green">${giftee}</b>!`;
    status.innerHTML = `Il codice "${code}" è stato trovato correttamente.`;
}
</script>
```
