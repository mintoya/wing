let currentLayer = 0;
const keys = [
  "#left-half .pinky-extra .one",
  "#left-half .pinky       .one",
  "#left-half .ring        .one",
  "#left-half .middle      .one",
  "#left-half .index       .one",
  "#left-half .index-extra .one",
  "#right-half .index-extra .one",
  "#right-half .index       .one",
  "#right-half .middle      .one",
  "#right-half .ring        .one",
  "#right-half .pinky       .one",
  "#right-half .pinky-extra .one",
  "#left-half .pinky-extra .two",
  "#left-half .pinky       .two",
  "#left-half .ring        .two",
  "#left-half .middle      .two",
  "#left-half .index       .two",
  "#left-half .index-extra .two",
  "#right-half .index-extra .two",
  "#right-half .index       .two",
  "#right-half .middle      .two",
  "#right-half .ring        .two",
  "#right-half .pinky       .two",
  "#right-half .pinky-extra .two",
  "#left-half .pinky-extra  .three",
  "#left-half .pinky        .three",
  "#left-half .ring         .three",
  "#left-half .middle       .three",
  "#left-half .index        .three",
  "#left-half .index-extra  .three",
  "#right-half .index-extra .three",
  "#right-half .index       .three",
  "#right-half .middle      .three",
  "#right-half .ring        .three",
  "#right-half .pinky       .three",
  "#right-half .pinky-extra .three",
  null,
  null,
  "#left-half .thumb       .three",
  "#left-half .thumb       .two",
  "#left-half .thumb       .one",
  null,
  null,
  "#right-half .thumb       .one",
  "#right-half .thumb       .two",
  "#right-half .thumb       .three",
  null,
  null,
];
keys.forEach((selector, index) => {
  if (selector != null) {
    const key = document.querySelector(selector);
    const name = validKeyValues[0].name;
    key.textContent = name;
    key.setAttribute("keyValue", name);
    keys[index] = key;
  }
});
keys.forEach((key, index) => {
  if (key !== null) {
    key.onclick = async () => {
      const name = await globalThis.setLayerKey(currentLayer, index);
      key.textContent = name;
      key.setAttribute("keyValue", name);
    };
  }
});
globalThis.changeLayer = function (layerN) {
  currentLayer;
  currentLayer = layerN;
  keys.forEach((key, index) => {
    if (globalThis.keyValueArrs[currentLayer] === undefined) {
      globalThis.keyValueArrs[currentLayer] = [];
    }
    const name = lookupKey(globalThis.keyValueArrs[currentLayer][index],"name");
    if (key) {
      key.textContent = name;
      key.setAttribute("keyValue", name);
    }
  });
};
