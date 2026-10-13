#include <Arduino.h>
#include <STM32FreeRTOS.h>

// Task 1: Controls the onboard LED blinky
void vBlinkTask(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);
    
    while (1) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(500)); // LED ON for 500ms
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(500)); // LED OFF for 500ms
    }
}

// Task 2: Prints messages to the serial port
void vSerialTask(void *pvParameters) {
    uint32_t ulMessageCount = 0;
    
    while (1) {
        Serial.print("FreeRTOS Two-Task Project - Message Count: ");
        Serial.println(ulMessageCount++);
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // Print every 1 second
    }
}

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    while (!Serial) {
        // Wait for serial port connection
    }

    // Create Task 1: LED Blinker
    xTaskCreate(
        vBlinkTask,         /* Pointer to the task function */
        "BlinkTask",        /* Descriptive name for debugging */
        128,                /* Stack depth in words */
        NULL,               /* Task parameters */
        1,                  /* Priority (1 = low priority) */
        NULL                /* Task handle */
    );

    // Create Task 2: Serial Printer
    xTaskCreate(
        vSerialTask,        /* Pointer to the task function */
        "SerialTask",       /* Descriptive name for debugging */
        128,                /* Stack depth in words */
        NULL,               /* Task parameters */
        1,                  /* Priority */
        NULL                /* Task handle */
    );

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();
}

void loop() {
    // Empty when using FreeRTOS tasks. 
}
