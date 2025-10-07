const tdModal = document.getElementById("modalTapdanceEditor");
tdModal.innerHTML = `
<div class="modal-box"> 
  <div id="holds">
    Holds
    <div class="inner"></div> 
    <button class = "addButton" id="addHold">+</button>
  </div>
  <div id="taps">
    Taps
    <div class="inner"></div>
    <button class = "addButton" id="addTap">+</button>
  </div>
  <button id="confirm">Confirm</button>
</div>
`;
tdModal.style.display = "none";
const holds = tdModal.querySelector("#holds");
const taps = tdModal.querySelector("#taps");
const holdInner = holds.querySelector(".inner");
const tapsInner = taps.querySelector(".inner");
let result = { taps: [], holds: [] };
let resolveTDSelection = null;
globalThis.requestTapdancePopup = function () {
  tdModal.style.display = "flex";
  result = { taps: [], holds: [] };
  return new Promise((resolve) => {
    resolveTDSelection = resolve;
  });
};
function closeTDModal() {
  holdInner.innerHTML = "";
  tapsInner.innerHTML = "";
  result = {
    taps: [],
    holds: [],
  };
  tdModal.style.display = "none";
}
function createKeyButton(name, list, container) {
  const btn = document.createElement("button");
  btn.textContent = name;
  btn.className = "keyButton";
  btn.onclick = () => {
    const idx = list.indexOf(btn._value);
    if (idx !== -1) list.splice(idx, 1);
    btn.remove();
  };
  container.appendChild(btn);
  return btn;
}
// handle tap addition
taps.querySelector("#addTap").onclick = async () => {
  const key = await globalThis.requestKeyPopup();
  result.taps.push(key.value);
  const btn = createKeyButton(key.name, result.taps, tapsInner);
  btn._value = key.value;
};
// handle hold addition
holds.querySelector("#addHold").onclick = async () => {
  const key = await globalThis.requestKeyPopup();
  result.holds.push(key.value);
  const btn = createKeyButton(key.name, result.holds, holdInner);
  btn._value = key.value;
};
// confirm button
tdModal.querySelector("#confirm").onclick = () => {
  if (resolveTDSelection) resolveTDSelection(result);
  closeTDModal();
};
