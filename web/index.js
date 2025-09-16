const wrapper = document.querySelector('#wrap-container');
const children = wrapper.querySelectorAll('#wrap-container .finger');
let rect = null;

children.forEach(child => {
  const r = child.getBoundingClientRect();
  rect = rect ? {
    left: Math.min(rect.left, r.left),
    top: Math.min(rect.top, r.top),
    right: Math.max(rect.right, r.right),
    bottom: Math.max(rect.bottom, r.bottom),
  } : r;
});

if (rect) {
  wrapper.style.width = rect.right - rect.left + "px";
  wrapper.style.height = rect.bottom - rect.top + "px";
}


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




keys.forEach((selector, index) => {
  if(selector!=null){
    var key = document.querySelector(selector);
    key.textContent = index;
  }
});

console.log(keys);
