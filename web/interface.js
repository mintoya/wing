globalThis.requestLayout = async ()=>{
  // showSpinner();
  const r = await writeWithResponse("request:getLayout;");
  console.log(r);
  const parsedObj = globalThis.fromKML(r);
  console.log(parsedObj);
  if (!parsedObj.keyboard || !parsedObj.keyboard.layers) {
    alert(
      "couldnt find keyboard in response\n this is fine if it has no data on it",
    );
    // hideSpinner();
    return null;
  }
  for (const [i, layer] of parsedObj.keyboard.layers.entries()) {
    for (const [ii, key] of layer.entries()) {
      if (!globalThis.keyValueArrs[i]) globalThis.keyValueArrs[i] = [];
      globalThis.keyValueArrs[i][ii] = key;
    }
  }

  globalThis.tapDanceArr = parsedObj.keyboard.tapdances

  globalThis.changeLayer();
  globalThis.updateTapdancesDiv()
  // document.querySelector(".top #U").setAttribute("current", "true");
  // hideSpinner();
  return parsedObj;
}
globalThis.saveLayout = async ()=>{
  const keyboard = {
    request:"setLayout",
    keyboard : {
      layers : globalThis.keyValueArrs,
      tapdances : globalThis.tapDanceArr,
    }
  }
  let msg = toKML(keyboard);
  msg= msg.substring(1,msg.length-2); // returns with {} by default
  
  const response = await writeWithResponse(msg);
  console.log(response);
}

