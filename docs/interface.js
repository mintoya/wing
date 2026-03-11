globalThis.requestLayout = async () => {
  const r = await writeWithResponse("{command:get-layout}");
  console.log(r);
  const root = globalThis.Vason.parse(r);
  console.log(root);
  const keyboard = root.get("keyboard");
  if (!keyboard || !keyboard.data) {
    alert(
      "couldnt find keyboard in response\n this is fine if it has no data on it",
    );
    return null;
  }
  const layersNode = keyboard.get("layers");
  if (layersNode && layersNode.data && layersNode.data._array) {
    layersNode.data._array.forEach((layerNode, i) => {
      if (!globalThis.keyValueArrs[i]) globalThis.keyValueArrs[i] = [];
      if (layerNode._array) {
        layerNode._array.forEach((key, ii) => {
          globalThis.keyValueArrs[i][ii] = key;
        });
      }
    });
  }
  // 4. Access "tapdances"
  const tdNode = keyboard.get("tapdances");
  if (tdNode && tdNode.data && tdNode.data._array) {
    // Map the Vason AST back to your internal JS objects if necessary
    globalThis.tapDanceArr = tdNode.data._array.map((td) => {
      // If tapdances are stored as pairs/objects in Vason, extract them here
      return td;
    });
  }
  globalThis.extendValidKeys();
  globalThis.changeLayer();
  globalThis.updateTapdancesDiv();
  return root;
};
function toVasonAST(val) {
  // 1. Handle Strings/Primitives
  if (typeof val !== "object" || val === null) {
    return String(val);
  }
  // 2. Check if it's already a VASON AST node (e.g., your _pair objects)
  if (val._pair || val._array) {
    return val;
  }
  // 3. Handle Arrays (Standard JS Array -> VASON _array)
  if (Array.isArray(val)) {
    return { _array: val.map(toVasonAST) };
  }
  // 4. Handle Objects (Standard JS Object -> VASON _array of _pairs)
  return {
    _array: Object.entries(val).map(([k, v]) => ({
      _pair: [k, toVasonAST(v)],
    })),
  };
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
  const vasonAST = toVasonAST(keyboard);
  const msg = new globalThis.Vason(vasonAST).stringify();
  await writeWithResponse(msg);
  console.log(msg);
};
