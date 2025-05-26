  let layout = {
    cols: [1,2],
    rows: [3,4],
    keys: [1,2,3,4],
    keys2d: [[1,2], [3,4]],
  }

  function addRow(defaultValue = 0) {
    layout.rows.push(0)
    const newRow = Array(layout.cols.length).fill(defaultValue)
    layout.keys2d.push(newRow)
    updateVisual()
  }

  function addCol(defaultValue = 0) {
    layout.cols.push(0)
    for (let i = 0; i < layout.keys2d.length; i++) {
      layout.keys2d[i].push(defaultValue)
    }
    updateVisual()
  }
  function updateVisual() {
    keys.innerHTML = ""
    rows.innerHTML = ""
    cols.innerHTML = ""
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
        r.appendChild(b)
      }
      keys.appendChild(r)
    }
  }
  function removeRow() {
    if (layout.rows.length > 0) {
      layout.rows.pop()
      layout.keys2d.pop()
    }
    console.log(layout)
    updateVisual()
  }

  function removeCol() {
    if (layout.cols.length > 0) {
      layout.cols.pop()
      for (let i = 0; i < layout.keys2d.length; i++) {
        layout.keys2d[i].pop()
      }
    }
    updateVisual()
  }

  //const text = JSON.stringify(layout)

  const rows = document.querySelector("#rows div")
  const cols = document.querySelector("#cols div")
  const keys = document.querySelector("#keys")
  updateVisual()
  // Helper to create a new button
