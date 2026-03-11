globalThis.requestLayout = async () => {
  const r = await writeWithResponse("{command:get-layout}");
  console.log(r);
  
  const root = globalThis.Vason.parse(r);
  console.log(root);

  const keyboard = root.get("keyboard");
  if (!keyboard || !keyboard.data) {
    alert("couldnt find keyboard in response\n this is fine if it has no data on it");
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
    globalThis.tapDanceArr = tdNode.data._array.map(td => {
      // If tapdances are stored as pairs/objects in Vason, extract them here
      return td; 
    });
  }

  globalThis.extendValidKeys();
  globalThis.changeLayer();
  globalThis.updateTapdancesDiv();

  return root;
};
globalThis.saveLayout=async()=>{
  const keyboard={
    command:"set-layout",
    keyboard:{
      layers:globalThis.keyValueArrs.map(a=>Array.from(a,b=>b??"")),
      tapdances:globalThis.tapDanceArr,
    }
  };
  let msg=globalThis.Vason.parse( JSON.stringify(keyboard) ).stringify();
  console.log(msg);
};
