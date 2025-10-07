// Create modal markup
const selectorModal = document.getElementById("modalKeySelector");
selectorModal.innerHTML = `
  <div class="modal-box">
    <input type="text" id="searchInput" placeholder="Search..." />
    <div id="results"></div>
  </div>
`;
const searchInput = selectorModal.querySelector("#searchInput");
const resultsContainer = selectorModal.querySelector("#results");
let resolveSelection = null;
globalThis.requestKeyPopup = function () {
  selectorModal.style.display = "flex";
  searchInput.value = "";
  resultsContainer.innerHTML = "";
  searchInput.focus();
  return new Promise((resolve) => {
    resolveSelection = resolve;
  });
};
// Close and cleanup modal
function closeModal() {
  selectorModal.style.display = "none";
  resolveSelection = null;
}
// Render results
function renderResults(results) {
  resultsContainer.innerHTML = "";
  results.forEach((item) => {
    const div = document.createElement("div");
    div.className = "result-item";
    div.textContent = item;
    div.onclick = () => {
      if (resolveSelection) {
        const value = globalThis.lookupKey(
          item,
          "value",
        );
        resolveSelection({ name: item, value: value });
        closeModal();
      }
      closeModal();
    };
    resultsContainer.appendChild(div);
  });
}
// Handle input
searchInput.addEventListener("input", () => {
  const query = searchInput.value.trim();
  if (query === "") {
    resultsContainer.innerHTML = "";
    return;
  }
  const results = fuzzySearch(query, 10).map((v) => {
    return v.item.name;
  });
  renderResults(results);
});
// Optional: ESC key to cancel modal
document.addEventListener("keydown", (e) => {
  if (e.key === "Escape") {
    closeModal();
  } else if (e.key === "Enter") {
    const firstResult = resultsContainer.querySelector(".result-item");
    if (firstResult) {
      firstResult.click();
    }
  }
});
