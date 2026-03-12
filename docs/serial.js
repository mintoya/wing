let port;
let reader;
let writer;
let logDisabled = false;
globalThis.writeWithResponse = async function (msg) {
  if (!logDisabled) {
    await portWrite("{command:disable-log}\n");
    await sleep(100);
    logDisabled = true;
  }
  try {
    serialStream.beginRead();
    await portWrite(msg + "\n");
    await sleep(100);
    return serialStream.endRead();
  } catch (_) {
    return null;
  }
};
const serialStream = {
  data: "", //for streaming
  maxWidth: 2048, //buffer max, doesnt effect readdata
  readData: "",
  textarea: document.querySelector("#serialIn"),
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
      throw new Error("one read at a time");
    }
    this.readData = "";
    this.reading = true;
  },
  endRead: function () {
    this.reading = false;
    return this.readData;
  },
};
globalThis.initSerial = async function () {
  try {
    port = await navigator.serial.requestPort({});
    await port.open({ baudRate: 115200 });
    writer = port.writable.getWriter();
    reader = port.readable.getReader();
    console.log("Serial port opened");
    beginStream();
    // await sleep(500);
    // await requestLayout();
  } catch (err) {
    alert("Serial error:", err);
  }
};
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
    alert(e + "");
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
        serialStream.write(new TextDecoder().decode(value));
      }
    }
  } catch (err) {
    alert("Serial error:" + err);
  }
}
