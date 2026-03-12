globalThis.requestLayout = async () => {
  const r = await writeWithResponse("{command:get-layout}");
  console.log("response :", r);
  const root = globalThis.Vason.parse(r);
  console.log(root);
  const keyboard = root.get("keyboard");
  if (!keyboard.data) {
    alert(
      "couldnt find keyboard in response\n this is fine if it has no data on it",
    );
    return null;
  }
  const layersNode = keyboard.get("layers");
  if (layersNode.data && layersNode.data._array) {
    layersNode.data._array.forEach((layerNode, i) => {
      if (!globalThis.keyValueArrs[i]) globalThis.keyValueArrs[i] = [];
      if (layerNode._array) {
        layerNode._array.forEach((key, ii) => {
          globalThis.keyValueArrs[i][ii] = key;
        });
      }
    });
  }
  const tapdances = keyboard.get("tapdances");
  if (tapdances.data && tapdances.data._array) {
    globalThis.tapDanceArr = tapdances.data._array.map((td) => {
      td = new globalThis.Vason(td);
      return {
        holds: td.get("holds").data._array,
        taps: td.get("taps").data._array,
      };
    });
  }
  globalThis.extendValidKeys();
  globalThis.changeLayer();
  globalThis.updateTapdancesDiv();
  return root;
};
function complicate(val) {
  if (typeof val !== "object" || val === null) {
    return String(val);
  }
  if (val._pair || val._array) {
    return val;
  }
  if (Array.isArray(val)) {
    return { _array: val.map(complicate) };
  } else {
    return {
      _array: Object.entries(val).map(([k, v]) => ({
        _pair: [k, complicate(v)],
      })),
    };
  }
}
globalThis.saveLayout = async () => {
  const keyboard = {
    command: "set-layout",
    args: {
      layout: {
        keyboard: {
          layers: globalThis.keyValueArrs.map((a) =>
            Array.from(a, (b) => b ?? "")
          ),
          tapdances: globalThis.tapDanceArr,
        },
      },
    },
  };
  const vasonAST = complicate(keyboard);
  const msg = new globalThis.Vason(vasonAST).stringify();
  await writeWithResponse(msg);
  console.log(msg);
};
