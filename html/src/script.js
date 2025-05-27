const rows = document.querySelector("#rows div")
const cols = document.querySelector("#cols div")
const keys = document.querySelector("#keys")
const keyboardDiv = document.querySelector('#keyReferences div');

let layout = {
  cols: [1,2],
  rows: [3,4],
  keys2d: [[1,2], [3,4]],
  selected:[0,0],
  locked: true,
}

// kinds correspond to modes, 0 is the main one, the rest are modes (1-9)
let sendLayout = {
  cols:[],
  rows:[],
  keys:[],
  kind:1,
}

const qwertyKeys = [
  ['`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'bksp'],
  ['tab', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\'],
  ['caps', 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'enter'],
  ['shift', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'shift'],
  ['ctrl', 'meta', 'alt', 'space', 'alt', 'meta', 'ctrl']
];
const functionKeys = [
  ['F1', 'F2', 'F3', 'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10'],
  ['F11', 'F12', 'F13', 'F14', 'F15', 'F16', 'F17', 'F18', 'F19', 'F20'],
  ['F21', 'F22', 'F23', 'F24']
];
const arrowKeys = [['up'],['left','down','right']]
const otherKeys = [['M(1)', 'M(2)', 'M(3)', 'M(4)', 'M(5)', 'M(6)', 'M(7)', 'M(8)', 'M(9)', 'M(10)']];
quertyDiv = document.createElement('div')
arrowDiv = document.createElement('div')
functionDiv = document.createElement('div')
otherDiv = document.createElement('div')
const keyDivs = [quertyDiv,arrowDiv,functionDiv,otherDiv]
var selectedDiv = 0;
qwertyKeys.forEach(row => {
  const rowDiv = document.createElement('div');
  rowDiv.className = 'keyBoardRow';
  row.forEach(key => {
    const btn = document.createElement('button');
    btn.className = 'keyBoardKey';
    btn.textContent = key;
    btn.setAttribute('data-key', key)
    rowDiv.appendChild(btn);
  });
  quertyDiv.appendChild(rowDiv);
});
arrowKeys.forEach(row => {
  const rowDiv = document.createElement('div');
  rowDiv.className = 'keyBoardRow';
  row.forEach(key => {
    const btn = document.createElement('button');
    btn.className = 'keyBoardKey';
    btn.textContent = key;
    btn.setAttribute('data-key', key)
    rowDiv.appendChild(btn);
  });
  arrowDiv.appendChild(rowDiv);
});
otherKeys.forEach(row => {
  const rowDiv = document.createElement('div');
  rowDiv.className = 'keyBoardRow';
  row.forEach(key => {
    const btn = document.createElement('button');
    btn.className = 'keyBoardKey';
    btn.textContent = key;
    btn.setAttribute('data-key', key)
    rowDiv.appendChild(btn);
  });
  otherDiv.appendChild(rowDiv);
});
functionKeys.forEach(row => {
  const rowDiv = document.createElement('div');
  rowDiv.className = 'keyBoardRow';
  row.forEach(key => {
    const btn = document.createElement('button');
    btn.className = 'keyBoardKey';
    btn.textContent = key;
    btn.setAttribute('data-key', key)
    rowDiv.appendChild(btn);
  });
  functionDiv.appendChild(rowDiv);
});
function nextKeys(){
  selectedDiv+=1
  selUpdate()
}
function prevKeys(){
  selectedDiv-=1
  selUpdate()
}
function selUpdate(){
  keyboardDiv.innerHTML = "";
  keyboardDiv.appendChild(keyDivs[Math.abs(selectedDiv%keyDivs.length)])
}

function lock(){
  updateData()
  layout.locked = !(layout.locked)
  if(layout.locked){
    document.documentElement.style.setProperty('--lockColor', 'rgb(150,150,150)');
    document.documentElement.style.setProperty('--lockVisible', 'visible');
    document.documentElement.style.setProperty('--unlockVisible', 'collapse');
  }else{
    document.documentElement.style.setProperty('--lockColor', 'white');
    document.documentElement.style.setProperty('--lockVisible', 'collapse');
    document.documentElement.style.setProperty('--unlockVisible', 'visible');
  }
}
function addRow() {
  if(layout.locked){return}
  let defaultValue = prompt("enter pin #:");
  defaultValue = parseInt(defaultValue, 10);
  layout.rows.push(defaultValue)
  const newRow = Array(layout.cols.length).fill(0)
  layout.keys2d.push(newRow)
  updateVisual()
}
function addCol() {
  if(layout.locked){return}
  let defaultValue = prompt("enter pin #:");
  defaultValue = parseInt(defaultValue, 10);
  layout.cols.push(defaultValue)
  for (let i = 0; i < layout.keys2d.length; i++) {
    layout.keys2d[i].push(0)
  }
  updateVisual()
}
function updateVisual() {
  keys.innerHTML = ""
  rows.innerHTML = ""
  cols.innerHTML = ""
  document.documentElement.style.setProperty('--rw',`calc(var(--widths)*${layout.rows.length*1.2+3}`);
  for (var j = 0; j < layout.cols.length; j++) {
    var c = document.createElement("button")
    c.className = "key col"
    c.textContent = layout.cols[j]
    cols.appendChild(c)
  }
  for (var i = 0; i < layout.rows.length; i++) {
    var r = document.createElement("div")
    r.className = "keyRow"
    var ref = document.createElement("button")
    ref.className = "key row"
    ref.textContent = layout.rows[i]
    rows.appendChild(ref)

    for (var j = 0; j < layout.cols.length; j++) {
      var b = document.createElement("button")
      b.className = "key"
      b.textContent = layout.keys2d[i][j]
      b.setAttribute("i", i)
      b.setAttribute("j", j)
      r.appendChild(b)

      b.addEventListener("click", function (e) {
        var clicked = e.target
        var coord = [parseInt(e.target.getAttribute("i")),parseInt(e.target.getAttribute("j"))]

        document.querySelectorAll('.key[selected="true"]').forEach(el => {
          el.removeAttribute('selected');
        })
        clicked.setAttribute("selected","true")

        layout.selected = coord
        console.log(layout.selected)
      })
      b.addEventListener("contextmenu", function (e) {
        e.preventDefault()
        console.log("Right click on:", b.textContent)
      });
    }
    keys.appendChild(r)
  }
  updateData()
}
function updateData(){
  sendLayout.keys = layout.cols;
  sendLayout.rows = layout.rows;
  sendLayout.keys = layout.keys2d.flat();
}
function removeRow() {
  if(layout.locked){return}
  if (layout.rows.length > 0) {
    layout.rows.pop()
    layout.keys2d.pop()
  }
  updateVisual()
}

function removeCol() {
  if(layout.locked){return}
  if (layout.cols.length > 0) {
    layout.cols.pop()
    for (let i = 0; i < layout.keys2d.length; i++) {
      layout.keys2d[i].pop()
    }
  }
  updateVisual()
}

//const text = JSON.stringify(layout)

// make sure the style is synced
updateVisual()
selUpdate()
lock()
lock()
