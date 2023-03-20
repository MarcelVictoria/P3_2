#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

const int led1 = 2; // Pin of the LED

SemaphoreHandle_t interruptSemaphore;


long debouncing_time = 150; 
volatile unsigned long last_micros;

void debounceInterrupt() 
{
  if((long)(micros() - last_micros) >= debouncing_time * 1000) 
  {
    interruptHandler();
    last_micros = micros();
  }
}

void interruptHandler() 
{
  xSemaphoreGiveFromISR(interruptSemaphore, NULL);
}

void TaskLed(void *pvParameters)
{
  (void) pvParameters;
  pinMode(8, OUTPUT);
  while(1) 
  {
    if (xSemaphoreTake(interruptSemaphore, portMAX_DELAY) == pdPASS) 
    {
      digitalWrite(8, !digitalRead(8));
    }  
  }
}

void TaskLed1(void *pvParameters)
{
  (void) pvParameters;
  pinMode(7, OUTPUT);
  while(1) {
    digitalWrite(7, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS);
    digitalWrite(7, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}


void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  xTaskCreate(TaskLed,  "Led", 128, NULL, 0, NULL );
  xTaskCreate(TaskBlink,  "LedBlink", 128, NULL, 0, NULL );
  interruptSemaphore = xSemaphoreCreateBinary();
  if (interruptSemaphore != NULL) 
  {
    attachInterrupt(digitalPinToInterrupt(2), debounceInterrupt, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}