document.querySelector(".top #U").setAttribute("current", "false");
function min(a, b) {
  return a < b ? a : b;
}
function levenshteinDistance(str1, str2) {
  let bias = 0;
  if (!str1 || !str2) {
    return 999;
  }
  const len1 = str1.length;
  const len2 = str2.length;
  if (!len1 || !len2) {
    return 999;
  }
  if (str1.includes(str2) || str2.includes(str1)) {
    bias -= 100 * min(str1.length, str2.length);
  }
  let matrix = Array(len1 + 1)
    .fill(null)
    .map(() => Array(len2 + 1).fill(0));
  for (let i = 0; i <= len1; i++) {
    matrix[i][0] = i;
  }
  for (let j = 0; j <= len2; j++) {
    matrix[0][j] = j;
  }
  for (let i = 1; i <= len1; i++) {
    for (let j = 1; j <= len2; j++) {
      const cost = str1[i - 1] === str2[j - 1] ? 0 : 1;
      matrix[i][j] = Math.min(
        matrix[i - 1][j] + 1, // Deletion
        matrix[i][j - 1] + 1, // Insertion
        matrix[i - 1][j - 1] + cost, // Substitution
      );
    }
  }
  return matrix[len1][len2] + bias;
}
function fuzzySearch(query, data, threshold = 10) {
  if (query == "" || query == null) {
    query = "A";
  }
  const results = [];
  const lowerCaseQuery = query.toLowerCase();
  for (const item of data) {
    const lowerCaseItem = item.name.toLowerCase();
    const lowerCaseItem2 = item.value.toLowerCase();
    const distance = min(
      levenshteinDistance(lowerCaseQuery, lowerCaseItem),
      levenshteinDistance(lowerCaseQuery, lowerCaseItem2),
    );
    if (distance <= threshold) {
      results.push({
        item: item,
        distance: distance,
      });
    }
  }
  results.sort((a, b) => a.distance - b.distance);
  return results;
}
function lookupKey(input, type) {
  for (const [i, key] of validKeyValues.entries()) {
    if (key[type == "name" ? "value" : "name"] == input) {
      return key[type];
    }
  }
  return type == "value" ? validKeyValues[0].value : validKeyValues[0].name;
}
let currentSelectedKey = null;
let currentLayer = 0;
let keys = [
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
let keyValueArrs = [
  [],
];
keys.forEach((selector, index) => {
  if (selector != null) {
    var key = document.querySelector(selector);
    key.textContent = validKeyValues[0].name;
    keys[index] = key;
  }
});
keys.forEach((key, index) => {
  if (key != null) {
    key.onclick = () => {
      if (keys[currentSelectedKey]) {
        keys[currentSelectedKey].removeAttribute("selected", "false");
      }
      if (currentSelectedKey != index) {
        currentSelectedKey = index;
        keys[currentSelectedKey].setAttribute("selected", "true");
      } else {
        currentSelectedKey = null;
      }
    };
  }
});
function search(word) {
  var container = document.querySelector(
    " #keyItem-selector .bottom #search-results",
  );
  container.innerHTML = "";
  var vkv = fuzzySearch(word, validKeyValues);
  vkv.forEach((map) => {
    var button = document.createElement("button");
    button.textContent = map.item.name;
    button.onclick = () => {
      if (currentSelectedKey) {
        keys[currentSelectedKey].textContent = button.textContent;
        keys[currentSelectedKey].setAttribute("keyValue", button.textContent);
        keyValueArrs[currentLayer][currentSelectedKey] = button.textContent;
        document.querySelector(".top #U").setAttribute("current", "false");
      }
    };
    container.appendChild(button);
  });
  return vkv;
}
function searchAndSelect(word) {
  var vkv = search(word);
  if (vkv[0]) {
    if (keys[currentSelectedKey]) {
      keys[currentSelectedKey].textContent = vkv[0].item.name;
      keys[currentSelectedKey].setAttribute("keyValue", vkv[0].item.name);
      keyValueArrs[currentLayer][currentSelectedKey] = vkv[0].item.name;
      document.querySelector(".top #U").setAttribute("current", "false");
    }
  }
}
function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}
async function changeLayer(layerN) {
  currentLayer = layerN;
  if (!keyValueArrs[currentLayer]) {
    keyValueArrs[currentLayer] = [];
  }
  for (const [index, key] of keys.entries()) {
    if (key) {
      keys[index].textContent = keyValueArrs[currentLayer][index] ||
        validKeyValues[0].name;
      keys[index].setAttribute("keyValue", keys[index].textContent);
      await sleep(1);
    }
  }
  // keys.forEach((key, index) => {
  // });
}
changeLayer(0);
search();
function getLayoutAsKml() {
  var msg = "";
  msg += " keyboard:{ layers:[ ";
  for (const [i, keyValueArr] of keyValueArrs.entries()) {
    msg += "[";
    if (keyValueArr) {
      for (const [ii, key] of keyValueArr.entries()) {
        if (key) {
          msg += lookupKey(key, "value");
        } else {
          msg += "_";
        }
        msg += ",";
        if (!((ii + 1) % 12)) msg += "\n";
      }
    }
    msg += "],";
  }
  msg += "]}";
  msg += "request:setLayout;";
  msg = "requestLength:" + msg.length + ";" + msg;
  return msg;
}
async function requestLayout() {
  showSpinner();
  const r = await writeWithResponse("request:getLayout;");
  const parsedObj = globalThis.parse(r);
  console.log(parsedObj);
  if (!parsedObj.keyboard || !parsedObj.keyboard.layers) {
    hideSpinner();
    alert("couldnt find keyboard in response\n this is fine if it has no data on it");
    return null;
  }
  for (const [i, layer] of parsedObj.keyboard.layers.entries()) {
    for (const [ii, key] of layer.entries()) {
      if (!keyValueArrs[i]) keyValueArrs[i] = [];
      keyValueArrs[i][ii] = lookupKey(key, "name");
    }
  }
  changeLayer(currentLayer);
  document.querySelector(".top #U").setAttribute("current", "true");
  hideSpinner();
  return parsedObj;
}
async function saveLayout() {
  showSpinner();
  const response = await writeWithResponse(getLayoutAsKml());
  const p = globalThis.parse(response);
  hideSpinner();
  if (p.status == "fail") {
    alert("upload failed, try again!!");
  }
}
const buffer = {
  data: "",
  readData: "",
  textarea: document.querySelector("#serialIn"),
  maxWidth: 2048,
  reading: false,
  write: function (msg) {
    if (this.reading) {
      this.readData += msg;
    }
    this.data += msg;
    if (this.data.length > this.maxWidth) {
      this.data = this.data.substring(this.data.length - this.maxWidth);
    }
  },
  beginRead: function () {
    if (this.reading) {
      console.error("one read at a time for now");
      return;
    }
    this.readData = "";
    this.reading = true;
  },
  endRead: function () {
    this.reading = false;
    return this.readData;
  },
};
let port;
let reader;
let writer;
async function writeWithResponse(msg) {
  buffer.beginRead();
  await sleep(250);
  await portWrite(msg);
  await sleep(250);
  return buffer.endRead();
}
async function initSerial() {
  try {
    port = await navigator.serial.requestPort({});
    await port.open({ baudRate: 115200 });
    writer = port.writable.getWriter();
    reader = port.readable.getReader();
    console.log("Serial port opened");
    beginStream();
    await sleep(500);
    await requestLayout();
  } catch (err) {
    console.error("Serial error:", err);
  }
}
async function portWrite(msg) {
  if (!writer) {
    alert("Port not open, call connect the board first");
    return;
  }
  try {
    console.log("writer");
    console.log(msg);
    await writer.write(new TextEncoder().encode(msg));
  } catch (e) {
    console.error(e);
  }
}
async function beginStream() {
  if (!reader) {
    alert("Port not open, call connect the board first");
    return;
  }
  try {
    while (true) {
      const { value, done } = await reader.read();
      if (done) {
        console.log("serial comms ended");
        break;
      }
      if (value) {
        buffer.write(new TextDecoder().decode(value));
      }
    }
  } catch (err) {
    console.error("Serial error:", err);
  }
}
function showSpinner() {
  // Check if the overlay already exists to avoid duplication
  if (document.getElementById("loading-overlay")) {
    return;
  }
  // --- 1. Create the Styles (CSS) ---
  const style = document.createElement("style");
  style.id = "spinner-styles";
  style.textContent = `
        /* Overlay to cover the screen and block clicks */
        #loading-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-color: rgba(0, 0, 0, 0.5); /* Semi-transparent black */
            z-index: 9999;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        /* The Spinner itself */
        .spinner {
            border: 8px solid #f3f3f3;
            border-top: 8px solid #3498db;
            border-radius: 50%;
            width: 60px;
            height: 60px;
            animation: spin 1s linear infinite;
        }
        /* The spinning animation */
        @keyframes spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
    `;
  document.head.appendChild(style);
  // --- 2. Create the HTML Elements ---
  const overlay = document.createElement("div");
  overlay.id = "loading-overlay";
  const spinner = document.createElement("div");
  spinner.className = "spinner";
  // Assemble the elements
  overlay.appendChild(spinner);
  document.body.appendChild(overlay);
}
function hideSpinner() {
  const overlay = document.getElementById("loading-overlay");
  const styles = document.getElementById("spinner-styles");
  if (overlay) {
    overlay.remove(); // Remove the HTML overlay element
  }
  if (styles) {
    styles.remove(); // Remove the injected CSS styles
  }
}
