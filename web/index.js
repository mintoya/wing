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
    query = "a"
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
    "NONE" ,
    "MOD_LCTRL"  ,
    "MOD_LSHIFT" ,
    "MOD_LALT"   ,
    "MOD_LMETA"  ,
    "MOD_RCTRL"  ,
    "MOD_RSHIFT" ,
    "MOD_RALT"   ,
    "MOD_RMETA"  ,
    "ERR_OVF" ,
    "A" ,
    "B" ,
    "C" ,
    "D" ,
    "E" ,
    "F" ,
    "G" ,
    "H" ,
    "I" ,
    "J" ,
    "K" ,
    "L" ,
    "M" ,
    "N" ,
    "O" ,
    "P" ,
    "Q" ,
    "R" ,
    "S" ,
    "T" ,
    "U" ,
    "V" ,
    "W" ,
    "X" ,
    "Y" ,
    "Z" ,
    "1" ,
    "2" ,
    "3" ,
    "4" ,
    "5" ,
    "6" ,
    "7" ,
    "8" ,
    "9" ,
    "0" ,
    "ENTER" ,
    "ESC" ,
    "BACKSPACE" ,
    "TAB" ,
    "SPACE" ,
    "MINUS" ,
    "EQUAL" ,
    "LEFTBRACE" ,
    "RIGHTBRACE" ,
    "BACKSLASH" ,
    "HASHTILDE" ,
    "SEMICOLON" ,
    "APOSTROPHE" ,
    "GRAVE" ,
    "COMMA" ,
    "DOT" ,
    "SLASH" ,
    "CAPSLOCK" ,
    "F1" ,
    "F2" ,
    "F3" ,
    "F4" ,
    "F5" ,
    "F6" ,
    "F7" ,
    "F8" ,
    "F9" ,
    "F10" ,
    "F11" ,
    "F12" ,
    "SYSRQ" ,
    "SCROLLLOCK" ,
    "PAUSE" ,
    "INSERT" ,
    "HOME" ,
    "PAGEUP" ,
    "DELETE" ,
    "END" ,
    "PAGEDOWN" ,
    "RIGHT" ,
    "LEFT" ,
    "DOWN" ,
    "UP" ,
    "NUMLOCK" ,
    "KPSLASH" ,
    "KPASTERISK" ,
    "KPMINUS" ,
    "KPPLUS" ,
    "KPENTER" ,
    "KP1" ,
    "KP2" ,
    "KP3" ,
    "KP4" ,
    "KP5" ,
    "KP6" ,
    "KP7" ,
    "KP8" ,
    "KP9" ,
    "KP0" ,
    "KPDOT" ,
    "102ND" ,
    "COMPOSE" ,
    "POWER" ,
    "KPEQUAL" ,
    "F13" ,
    "F14" ,
    "F15" ,
    "F16" ,
    "F17" ,
    "F18" ,
    "F19" ,
    "F20" ,
    "F21" ,
    "F22" ,
    "F23" ,
    "F24" ,
    "OPEN" ,
    "HELP" ,
    "PROPS" ,
    "FRONT" ,
    "STOP" ,
    "AGAIN" ,
    "UNDO" ,
    "CUT" ,
    "COPY" ,
    "PASTE" ,
    "FIND" ,
    "MUTE" ,
    "VOLUMEUP" ,
    "VOLUMEDOWN" ,
    "KPCOMMA" ,
    "RO" ,
    "KATAKANAHIRAGANA" ,
    "YEN" ,
    "HENKAN" ,
    "MUHENKAN" ,
    "KPJPCOMMA" ,
    "HANGEUL" ,
    "HANJA" ,
    "KATAKANA" ,
    "HIRAGANA" ,
    "ZENKAKUHANKAKU" ,
    "KPLEFTPAREN" ,
    "KPRIGHTPAREN" ,
    "LEFTCTRL" ,
    "LEFTSHIFT" ,
    "LEFTALT" ,
    "LEFTMETA" ,
    "RIGHTCTRL" ,
    "RIGHTSHIFT" ,
    "RIGHTALT" ,
    "RIGHTMETA" ,
    "MEDIA_PLAYPAUSE" ,
    "MEDIA_STOPCD" ,
    "MEDIA_PREVIOUSSONG" ,
    "MEDIA_NEXTSONG" ,
    "MEDIA_EJECTCD" ,
    "MEDIA_VOLUMEUP" ,
    "MEDIA_VOLUMEDOWN" ,
    "MEDIA_MUTE" ,
    "MEDIA_WWW" ,
    "MEDIA_BACK" ,
    "MEDIA_FORWARD" ,
    "MEDIA_STOP" ,
    "MEDIA_FIND" ,
    "MEDIA_SCROLLUP" ,
    "MEDIA_SCROLLDOWN" ,
    "MEDIA_EDIT" ,
    "MEDIA_SLEEP" ,
    "MEDIA_COFFEE" ,
    "MEDIA_REFRESH" ,
    "MEDIA_CALC"
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
