#include <bluefruit.h>

BLEDfu bledfu;
BLEClientUart clientUart;
void scanCallback(ble_gap_evt_adv_report_t* report){
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet  
  Bluefruit.Central.connect(report);
}
void centralBlueRxCallback(BLEClientUart& centralUart){
  char str[21] = { 0 };
  centralUart.read(str, 20);

  Serial.printf("[Cent] Rx: %.*s\n",20,str);
  // if ( bleuart.notifyEnabled() )
  // {
  //   // Forward data from our peripheral to Mobile
  //   bleuart.print( str );
  // }else
  // {
  //   // response with no prph message
  //   clientUart.println("[Cent] Peripheral role not connected");
  // }  
}

void bluePeripheralConectCallback(uint16_t handle){
  BLEConnection* connection = Bluefruit.Connection(handle);

  char peer_name[32] = { 0 };
  connection->getPeerName(peer_name, sizeof(peer_name));

  Serial.printf("[Prph] Connected to %.*s\n",32,peerName);
}
bluePeripheralDisConectCallback(uint16_t handle,uint8_t reason){
  (void) handle;
  (void) reason;
  Serial.println("Disconnected");
}

void setup(){
  Serial.begin(115200);
  while ( !Serial ) delay(10);

  Serial.println("Bluefruit52 Reciever Example");
  Serial.println("-------------------------------------\n");

  Bluefruit.begin(1,0);
  // - nRF52840: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +2dBm, +3dBm, +4dBm, +5dBm, +6dBm, +7dBm and +8dBm.
  Bluefruit.setTxPower(4);

  Bluefruit.Periph.setConnectCallback(bluePeripheralConectCallback);
  Bluefruit.Periph.setDisconnectCallback(bluePeripheralDisConectCallback);
  bledfu.begin();

  clientUart.begin();
  clientUart.setRxCallback(centralBlueRxCallback);
  Bluefruit.Scanner.setRxCallback(scanCallback);

  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds

  // Set up and start advertising
  // only the sender part sends info
  // startAdv();

  
}

