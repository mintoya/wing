    let port;
    let writer;

    async function connect() {
      port = await navigator.serial.requestPort();
      await port.open({ baudRate: 115200 });
      writer = port.writable.getWriter();
      alert("Connected to ESP32");
    }

    async function send() {
      const text = document.getElementById("json").value;
      const encoder = new TextEncoder();
      await writer.write(encoder.encode(text + "\n"));
      alert("Sent!");
    }
