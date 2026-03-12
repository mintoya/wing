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
tapDanceContainerAddButton.onclick = async () => {
  globalThis.tapDanceArr.push(
    await globalThis.requestTapdancePopup(),
  );
  updateTapdancesDiv();
};
globalThis.saveTapdances = (index) => {
  globalThis.keyValueArrs.forEach((layer, i) => {
    layer.forEach((key, j) => {
      if (key._pair && key._pair[0] == "T") {
        const kn = parseInt(key._pair[1], 10);
        if (kn > index) {
          keyValueArrs[i][j] = { _pair: ["T", "" + (kn - 1)] };
        } else if (kn == index) {
          keyValueArrs[i][j] = globalThis.validKeyValues[0].value;
        }
      }
    });
  });
  // const uniqueLayers = new Set(globalThis.keyValueArrs);
  // uniqueLayers.forEach((layer) => {
  //   layer.forEach((key, layerIndex) => {
  //     if (!key || typeof key !== "object" || !key._pair) return;
  //     if (key._pair[0] === "T") {
  //       let i = typeof key._pair[1] === "string"
  //         ? parseInt(key._pair[1], 10)
  //         : key._pair[1];
  //       if (i > index) {
  //         layer[layerIndex] = { _pair: ["T", (i - 1).toString()] };
  //       } else if (i === index) {
  //         layer[layerIndex] = globalThis.validKeyValues[0].value;
  //       }
  //     }
  //   });
  // });
};
globalThis.updateTapdancesDiv = function () {
  tapDancesContainer.innerHTML = "";
  // Safety check: Make sure tapDanceArr itself exists
  if (!globalThis.tapDanceArr) return;
  globalThis.tapDanceArr.forEach((element, index) => {
    const container = document.createElement("div");
    container.innerHTML = tdItemModel;
    const tapsDiv = container.querySelector(".taps");
    const holdsDiv = container.querySelector(".holds");
    // Use ?.forEach or (element.taps || []).forEach
    element.taps?.forEach((tap) => {
      const tapButton = document.createElement("button");
      tapButton.innerText = lookupKey(tap, "name");
      tapsDiv.appendChild(tapButton);
    });
    element.holds?.forEach((hold) => {
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
