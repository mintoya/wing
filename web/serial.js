let port;
let reader;
let writer;
globalThis.writeWithResponse = async function (msg) {
  serialStream.beginRead();
  await sleep(250);
  await portWrite("requestLength:" + msg.length + ";" + msg);
  await sleep(250);
  return serialStream.endRead();
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
    console.error("Serial error:", err);
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
        serialStream.write(new TextDecoder().decode(value));
      }
    }
  } catch (err) {
    console.error("Serial error:", err);
  }
}
