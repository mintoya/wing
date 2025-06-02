#include "BleKeyboard.h"
#include <SPIFFS.h>
#include <stdlib.h>
#include "KBState.h"
#include <Adafruit_CH9328.h>

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


//#include <bluemicro_hid.h>
#include "list.hpp"
BleKeyboard bleKeyboard("ESP32 BLE Keyboard", "YourManufacturer", 100);
Adafruit_CH9328 hidKeyboard;
String getS(){
    String input = Serial.readStringUntil('\n').c_str();
    Serial.println("Received: " + input);
    return input;
}
void panic(String message){
  Serial.println(message);
  delay(10000);
  exit(0);
}

static char* location = "/dat.txt";
static kbs KeyboardStateHolder;
void kbsSave(){
  Serial.println("saving Keyboard");
  fatPointer bits = bytesKb(KeyboardStateHolder);
  Serial.println("got char*, opening file:");
  File file;
  if(not (file = SPIFFS.open(location,FILE_WRITE))){
    panic("couldnt open file");
  }else{
    Serial.println("opening file");
  }
  file.write((uint8_t*)bits.ptr,bits.len);
  file.close();
  free(bits.ptr);
  Serial.println("saved keyboard");
}
void kbsDefaultLoad(){
    unsigned int a_array[] = {13};
    unsigned int b_array[] = {14};
    unsigned int* b = b_array;
    unsigned int* a = a_array;
    Serial.println("kbData init");
    keyAction ka[] = {{.kind = 0,.data = 'a'}};
    KeyboardStateHolder = kbs_init((unsigned int)1,(unsigned int)1,a,b,(keyAction*)ka);
    kbsSave();
}
void kbsLoad(){
  Serial.println("loading Keyboard");
  //List* L = List_new(sizeof(char));
  ListPlux<char> L;
  if(SPIFFS.exists(location)){

    File file = SPIFFS.open(location);
    if (file) {
      while (file.available()) {
        char c = file.read();
        //List_append(L,&c);
        L.push_back(c);
      }
      file.close();
    }else{
      panic("couldnt open existing file");
    }
    //readKb(L->head,&KeyboardStateHolder );
    readKb(L.raw_list()->head,&KeyboardStateHolder);
    Serial.println("Loaded keyboard Binds");
  }else{
    kbsDefaultLoad();
    kbsSave();
  }
  kbsPinsInit(KeyboardStateHolder);
}
void setup() {
  Serial.begin(115200);
  delay(2000);
  print_wakeup_reason();
  Serial.println("testing testing 123");
  SPIFFS.begin(true);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();

  Serial.println("Starting HID work!");
  hidKeyboard.begin(&Serial1);


  Serial.println("Ble init");
  //kbsDefaultLoad();
  kbsLoad();
  kbsPinsInit(KeyboardStateHolder);
  Serial.println("initialized");
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
}
void loop() {
  kbs_Update(&KeyboardStateHolder);
  kbs_compareAndSend(&KeyboardStateHolder,bleKeyboard,hidKeyboard);
  if(Serial.available()){
    digitalWrite(2,HIGH);
    delay(100);
    digitalWrite(2,LOW);
    String input = Serial.readStringUntil('\n');
    DynamicJsonDocument doc(1024);
    if(deserializeJson(doc,input)){
      Serial.println("invalid Json?");
    }else{
      readSerial(&KeyboardStateHolder,doc);
      kbsSave();
      kbsPinsInit(KeyboardStateHolder);
    }
  }
}
