function levenshteinDistance(str1, str2) {
  let bias = 0;
  if(!str1 || !str2){
    return 999;
  }
  const len1 = str1.length;
  const len2 = str2.length;

  if(!len1 || !len2){
    return 999;
  }
  if(str1.includes(str2) || str2.includes(str1)){
    bias-=100;
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
        matrix[i - 1][j - 1] + cost // Substitution
      );
    }
  }
  return matrix[len1][len2] + bias;
}
function fuzzySearch(query, data, threshold = 10) {
  if(query=="" || query==null){
    query = "A"
  }
  const results = [];
  const lowerCaseQuery = query.toLowerCase();

  for (const item of data) {
    const lowerCaseItem = item.toLowerCase();
    const distance = levenshteinDistance(lowerCaseQuery, lowerCaseItem);
    if (distance <= threshold) {
      results.push({ item: item, distance: distance });
    }
  }
  results.sort((a, b) => a.distance - b.distance);
  return results;
}

const validKeyValues = 
  [
    "KEY_NONE" ,
    "KEY_MOD_LCTRL"  ,
    "KEY_MOD_LSHIFT" ,
    "KEY_MOD_LALT"   ,
    "KEY_MOD_LMETA"  ,
    "KEY_MOD_RCTRL"  ,
    "KEY_MOD_RSHIFT" ,
    "KEY_MOD_RALT"   ,
    "KEY_MOD_RMETA"  ,
    "KEY_ERR_OVF" ,
    "KEY_A" ,
    "KEY_B" ,
    "KEY_C" ,
    "KEY_D" ,
    "KEY_E" ,
    "KEY_F" ,
    "KEY_G" ,
    "KEY_H" ,
    "KEY_I" ,
    "KEY_J" ,
    "KEY_K" ,
    "KEY_L" ,
    "KEY_M" ,
    "KEY_N" ,
    "KEY_O" ,
    "KEY_P" ,
    "KEY_Q" ,
    "KEY_R" ,
    "KEY_S" ,
    "KEY_T" ,
    "KEY_U" ,
    "KEY_V" ,
    "KEY_W" ,
    "KEY_X" ,
    "KEY_Y" ,
    "KEY_Z" ,
    "KEY_1" ,
    "KEY_2" ,
    "KEY_3" ,
    "KEY_4" ,
    "KEY_5" ,
    "KEY_6" ,
    "KEY_7" ,
    "KEY_8" ,
    "KEY_9" ,
    "KEY_0" ,
    "KEY_ENTER" ,
    "KEY_ESC" ,
    "KEY_BACKSPACE" ,
    "KEY_TAB" ,
    "KEY_SPACE" ,
    "KEY_MINUS" ,
    "KEY_EQUAL" ,
    "KEY_LEFTBRACE" ,
    "KEY_RIGHTBRACE" ,
    "KEY_BACKSLASH" ,
    "KEY_HASHTILDE" ,
    "KEY_SEMICOLON" ,
    "KEY_APOSTROPHE" ,
    "KEY_GRAVE" ,
    "KEY_COMMA" ,
    "KEY_DOT" ,
    "KEY_SLASH" ,
    "KEY_CAPSLOCK" ,
    "KEY_F1" ,
    "KEY_F2" ,
    "KEY_F3" ,
    "KEY_F4" ,
    "KEY_F5" ,
    "KEY_F6" ,
    "KEY_F7" ,
    "KEY_F8" ,
    "KEY_F9" ,
    "KEY_F10" ,
    "KEY_F11" ,
    "KEY_F12" ,
    "KEY_SYSRQ" ,
    "KEY_SCROLLLOCK" ,
    "KEY_PAUSE" ,
    "KEY_INSERT" ,
    "KEY_HOME" ,
    "KEY_PAGEUP" ,
    "KEY_DELETE" ,
    "KEY_END" ,
    "KEY_PAGEDOWN" ,
    "KEY_RIGHT" ,
    "KEY_LEFT" ,
    "KEY_DOWN" ,
    "KEY_UP" ,
    "KEY_NUMLOCK" ,
    "KEY_KPSLASH" ,
    "KEY_KPASTERISK" ,
    "KEY_KPMINUS" ,
    "KEY_KPPLUS" ,
    "KEY_KPENTER" ,
    "KEY_KP1" ,
    "KEY_KP2" ,
    "KEY_KP3" ,
    "KEY_KP4" ,
    "KEY_KP5" ,
    "KEY_KP6" ,
    "KEY_KP7" ,
    "KEY_KP8" ,
    "KEY_KP9" ,
    "KEY_KP0" ,
    "KEY_KPDOT" ,
    "KEY_102ND" ,
    "KEY_COMPOSE" ,
    "KEY_POWER" ,
    "KEY_KPEQUAL" ,
    "KEY_F13" ,
    "KEY_F14" ,
    "KEY_F15" ,
    "KEY_F16" ,
    "KEY_F17" ,
    "KEY_F18" ,
    "KEY_F19" ,
    "KEY_F20" ,
    "KEY_F21" ,
    "KEY_F22" ,
    "KEY_F23" ,
    "KEY_F24" ,
    "KEY_OPEN" ,
    "KEY_HELP" ,
    "KEY_PROPS" ,
    "KEY_FRONT" ,
    "KEY_STOP" ,
    "KEY_AGAIN" ,
    "KEY_UNDO" ,
    "KEY_CUT" ,
    "KEY_COPY" ,
    "KEY_PASTE" ,
    "KEY_FIND" ,
    "KEY_MUTE" ,
    "KEY_VOLUMEUP" ,
    "KEY_VOLUMEDOWN" ,
    "KEY_KPCOMMA" ,
    "KEY_RO" ,
    "KEY_KATAKANAHIRAGANA" ,
    "KEY_YEN" ,
    "KEY_HENKAN" ,
    "KEY_MUHENKAN" ,
    "KEY_KPJPCOMMA" ,
    "KEY_HANGEUL" ,
    "KEY_HANJA" ,
    "KEY_KATAKANA" ,
    "KEY_HIRAGANA" ,
    "KEY_ZENKAKUHANKAKU" ,
    "KEY_KPLEFTPAREN" ,
    "KEY_KPRIGHTPAREN" ,
    "KEY_LEFTCTRL" ,
    "KEY_LEFTSHIFT" ,
    "KEY_LEFTALT" ,
    "KEY_LEFTMETA" ,
    "KEY_RIGHTCTRL" ,
    "KEY_RIGHTSHIFT" ,
    "KEY_RIGHTALT" ,
    "KEY_RIGHTMETA" ,
    "KEY_MEDIA_PLAYPAUSE" ,
    "KEY_MEDIA_STOPCD" ,
    "KEY_MEDIA_PREVIOUSSONG" ,
    "KEY_MEDIA_NEXTSONG" ,
    "KEY_MEDIA_EJECTCD" ,
    "KEY_MEDIA_VOLUMEUP" ,
    "KEY_MEDIA_VOLUMEDOWN" ,
    "KEY_MEDIA_MUTE" ,
    "KEY_MEDIA_WWW" ,
    "KEY_MEDIA_BACK" ,
    "KEY_MEDIA_FORWARD" ,
    "KEY_MEDIA_STOP" ,
    "KEY_MEDIA_FIND" ,
    "KEY_MEDIA_SCROLLUP" ,
    "KEY_MEDIA_SCROLLDOWN" ,
    "KEY_MEDIA_EDIT" ,
    "KEY_MEDIA_SLEEP" ,
    "KEY_MEDIA_COFFEE" ,
    "KEY_MEDIA_REFRESH" ,
    "KEY_MEDIA_CALC"
  ]
// append KEY_ to front before sending


var currentSelectedKey = null;
var currentLayer = 0;

var keys = [
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
  null,null,null,//padding 

  "#left-half .thumb       .three",
  "#left-half .thumb       .two",
  "#left-half .thumb       .one",

  "#right-half .thumb       .one",
  "#right-half .thumb       .two",
  "#right-half .thumb       .three",

  null,null,null,//padding 
];
var keyValueArrs = [[]]

keys.forEach((selector, index) => {
  if(selector!=null){
    var key = document.querySelector(selector);
    key.textContent = validKeyValues[0];
    keys[index] = key;
  }
});
keys.forEach((key,index) => {
  if(key!=null){
    key.onclick = ()=>{
      if(keys[currentSelectedKey]){
        keys[currentSelectedKey].removeAttribute("selected","false")
      }
      if(currentSelectedKey!=index){
        currentSelectedKey = index;
        keys[currentSelectedKey].setAttribute("selected","true")
      }else{
        currentSelectedKey = null;
      }
    }
  }
});
function search(word){
  var container = document.querySelector(" #keyItem-selector .bottom #search-results")
  container.innerHTML = "";
  var vkv = fuzzySearch(word, validKeyValues );
  vkv.forEach(map => {
    var button = document.createElement("button");
    button.textContent = map.item;
    button.onclick = ()=>{
      if(currentSelectedKey){
        keys[currentSelectedKey].textContent = button.textContent;
        keys[currentSelectedKey].setAttribute("keyValue",button.textContent)
        keyValueArrs[currentLayer][currentSelectedKey] = button.textContent;
      }
    }
    container.appendChild(button);
  });
  return vkv
}
function searchAndSelect(word){
  var vkv = search(word);
  if(vkv[0]) {
    keys[currentSelectedKey].textContent = vkv[0].item
    keys[currentSelectedKey].setAttribute("keyValue",vkv[0].item)
    keyValueArrs[currentLayer][currentSelectedKey] = vkv[0].item;
  }
}
function changeLayer(layerN){
  currentLayer = layerN;
  if(!keyValueArrs[currentLayer]){
    keyValueArrs[currentLayer] = [];
  }
  keys.forEach(( key,index ) => {
    if(key){
      keys[index].textContent = keyValueArrs[currentLayer][index] || validKeyValues[0];
      keys[index].setAttribute("keyValue",keys[index].textContent)
    }
  });
}

changeLayer(0);
search();



let port;

async function initSerial() {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });
  console.log("Serial connected");
}

async function sendmsg(msg) {
  console.log(msg);
  if (!port) {
    console.error("Port not open, call initSerial() first");
    return;
  }
  const writer = port.writable.getWriter();
  await writer.write(new TextEncoder().encode(msg));
  writer.releaseLock();
}
function getLayoutAsKml(){
  var msg = "";
  msg+=" keyboard:{ layers:[ ";
  for(const[i,keyValueArr] of keyValueArrs.entries()){
    msg+="[";
    if(keyValueArr){
      for(const[ii,key] of keyValueArr.entries()){
        if(key){
          msg+=key; msg+=',';
        }else{
          msg+='0'; msg+=',';
        }
      }
    }
    msg+="]";

  }
  msg+="]}"
  msg+="OVERRIDE:y;"//no confirmation
  return msg;
}
