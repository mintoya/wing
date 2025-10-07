const tapDancesContainer = document.querySelector("#tapdanceDiv");
const tapDanceContainerAddButton = document.querySelector("#addTD");
const tdItemModel = `
  <div class="tapdance">
    <div class = "cflex">
      <div class = "taps"></div>
      <div class = "holds"></div>
    </div>
    <button class = "remove">-</>
  </div>
`;
globalThis.updateTapdancesDiv = function () {
  tapDancesContainer.innerHTML = "";
  globalThis.tapDanceArr.forEach((element, index) => {
    const container = document.createElement("div");
    container.innerHTML = tdItemModel;
    const tapsDiv = container.querySelector(".taps");
    const holdsDiv = container.querySelector(".holds");
    element.taps.forEach((tap) => {
      const tapButton = document.createElement("button");
      tapButton.innerText = lookupKey(tap, "name");
      tapsDiv.appendChild(tapButton);
    });
    element.holds.forEach((hold) => {
      const holdButton = document.createElement("button");
      holdButton.innerText = lookupKey(hold, "name");
      holdsDiv.appendChild(holdButton);
    });
    container.querySelector(".remove").onclick = () => {
      saveTapdances(index);
      globalThis.tapDanceArr.splice(index, 1);
      updateTapdancesDiv();
      globalThis.changeLayer();
    };
    tapDancesContainer.appendChild(container);
  });
  globalThis.extendValidKeys();
};
tapDanceContainerAddButton.onclick = async () => {
  globalThis.tapDanceArr.push(
    await globalThis.requestTapdancePopup(),
  );
  updateTapdancesDiv();
};
globalThis.saveTapdances = (index) => {
  const regex = /^T\(\s*(\d+)\s*\)$/;
  globalThis.keyValueArrs.forEach((layer) => {
    layer.forEach((keyString, layerIndex) => {
      const match = keyString.match(regex);
      if (match) {
        let i = parseInt(match[1], 10);
        if (i > index) {
          i -= 1;
          layer[layerIndex] = `T( ${i} )`;
        } else if (i === index) {
          layer[layerIndex] = globalThis.validKeyValues[0].value;
        }
      }
    });
  });
};
