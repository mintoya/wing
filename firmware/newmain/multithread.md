example of multithreaded programming for the esp or whatever

``` c
void setup() {
  Serial.begin(115200);

  // Create a task that will execute the "Task1Code" function
  xTaskCreatePinnedToCore(
    Task1Code,   /* Task function. */
    "Task1",     /* name of task. */
    10000,       /* Stack size in words */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    NULL,        /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
}

void Task1Code( void * pvParameters ){
  pinMode(LED_BUILTIN, OUTPUT);
  for(;;){ // Tasks must loop forever
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS); // Use vTaskDelay instead of delay()
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
```
```c
#include "esp_timer.h"

// This flag lets your main loop know the timer finished
volatile bool timerTriggered = false;

// This function runs automatically every 500 microseconds
void IRAM_ATTR onTimer(void* arg) {
    // DO NOT use Serial.print or delay() here!
    // Just toggle a pin or set a flag.
    timerTriggered = true; 
}

void setup() {
    Serial.begin(115200);

    const esp_timer_create_args_t timer_args = {
        .callback = &onTimer,
        .name = "500us-timer"
    };

    esp_timer_handle_t periodic_timer;
    esp_timer_create(&timer_args, &periodic_timer);

    // Start the timer to repeat every 500 microseconds
    esp_timer_start_periodic(periodic_timer, 500); 
}

void loop() {
    if (timerTriggered) {
        timerTriggered = false;
        // Run your time-sensitive logic here
    }

    // Other tasks work here perfectly without being blocked
}
```
