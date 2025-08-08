#include <bluefruit.h>

BLEDfu bledfu;
BLEUart bleuart;

void blueCentralConnectCallback(uint16_t connectionHandle){
  char peerName[32] = {0};
  BLEConnection* connection = Bluefruit.Connected(connectionHandle);
  connection->getPeerName(peerName,sizeof(peerName));
  Serial.printf("Connected to: %.*s",sizeof(peerName),peerName);
}
void blueCentralDisConnectCallback(uint16_t connectionHandle,uint8_t reason){
  (void) connectionHandle;
  (void) reason;
  Serial.println("Disconnected?");
}

void bleuartRxCallback(uint16_t connectionHandle){
  (void) connectionHandle;

  char str[20] = {0};
  bleuart.read(str,20);

  Serial.printf("[Prph] Rx: %.*s\n",20,str);
}
void scanCallback(ble_gap_evt_adv_report_t* report){
  // Since we configure the scanner with filterUuid()
  // Scan callback only invoked for device with bleuart service advertised  
  // Connect to the device with bleuart service in advertising packet  
  Bluefruit.Central.connect(report);
}

void setup(){
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 TransMitter Example");
  Serial.println("-------------------------------------\n");
  // bool begin(uint8_t prph_count = 1, uint8_t central_count = 0); 
  //count as number of connections
  Bluefruit.begin(0,1);
    // - nRF52840: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +2dBm, +3dBm, +4dBm, +5dBm, +6dBm, +7dBm and +8dBm.


  Bluefruit.setTxPower(4);
  Bluefruit.Central.setConnectCallback(blueCentralConnectCallback);
  Bluefruit.Central.setDisconnectCallback(blueCentralDisConnectCallback);

  bledfu.begin();
  bleuart.begin();
  bleuart.setRxCallback(bleuartRxCallback);

  Bluefruit.Scanner.setRxCallback(scanCallback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.filterUuid(bleuart.uuid);
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
                                              startAdv();
}
void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   *
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

