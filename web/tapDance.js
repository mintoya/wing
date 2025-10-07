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
  globalThis.tapDanceArr.forEach((element, _index) => {
    const container = document.createElement("div");
    container.innerHTML = tdItemModel;
    const tapsDiv = container.querySelector(".taps");
    const holdsDiv = container.querySelector(".holds");
    element.taps.forEach((tap) => {
      const tapButton = document.createElement("button");
      tapButton.innerText = lookupKey(tap,"name");
      tapsDiv.appendChild(tapButton);
    });
    element.holds.forEach((hold) => {
      const holdButton = document.createElement("button");
      holdButton.innerText = lookupKey(hold,"name");
      holdsDiv.appendChild(holdButton);
    });
    tapDancesContainer.appendChild(container);
  });
};
tapDanceContainerAddButton.onclick = async () => {
  globalThis.tapDanceArr.push(
    await globalThis.requestTapdancePopup(),
  );
  updateTapdancesDiv();
};
