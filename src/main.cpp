#include <Arduino.h>
#include <STM32FreeRTOS.h>

// Handle for a Queue or Semaphore used for synchronization
QueueHandle_t xSensorQueue;
SemaphoreHandle_t xPrintMutex;

// Task 1: Produces data periodically
void vSensorTask(void *pvParameters) {
    int sampleValue = 0;
    while (1) {
        sampleValue++;
        // Send data to the queue (blocks if queue is full)
        xQueueSend(xSensorQueue, &sampleValue, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Task 2: Processes the data
void vProcessingTask(void *pvParameters) {
    int receivedValue;
    while (1) {
        // Wait until data arrives from Task 1
        if (xQueueReceive(xSensorQueue, &receivedValue, portMAX_DELAY)) {
            // Process data (e.g., multiply or filter)
            int processedValue = receivedValue * 2;
            //  uint32_t cpid = SCB->CPUID;
            // Pass along or print safely using a Mutex
            if (xSemaphoreTake(xPrintMutex, portMAX_DELAY) == pdTRUE) {
                // Serial.printf("  CPUID Register:  0x%08lX\r\n", cpid);
                // Serial.print("  CPUID Register:  0x");
                // Serial.println(cpid, HEX);
                Serial.print("T2-Processed Value: ");
                Serial.println(processedValue);
                xSemaphoreGive(xPrintMutex);
            }
        }
    }
}

// Task 3:  LED blink
void vBlinkTask(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT);
    while (1) {
        digitalWrite(LED_BUILTIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(1000));
        digitalWrite(LED_BUILTIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void setup() {
    Serial.begin(115200);
    
    // Create synchronization tools
    xSensorQueue = xQueueCreate(5, sizeof(int)); // Queue holding up to 5 integers
    xPrintMutex = xSemaphoreCreateMutex();       // Protects serial printing

    // Create the 3 tasks
    xTaskCreate(vSensorTask, "Sensor", 128, NULL, 2, NULL);      // Higher priority
    xTaskCreate(vProcessingTask, "Process", 128, NULL, 2, NULL); // Medium priority
    xTaskCreate(vBlinkTask, "Blink", 128, NULL, 1, NULL);        // Lower priority

    vTaskStartScheduler();
}

void loop() {}

// #include <Arduino.h>
// #include <STM32FreeRTOS.h>

// // Task 1: Controls the onboard LED blinky
// void vBlinkTask(void *pvParameters) {
//     pinMode(LED_BUILTIN, OUTPUT);
    
//     while (1) {
//         digitalWrite(LED_BUILTIN, HIGH);
//         vTaskDelay(pdMS_TO_TICKS(500)); // LED ON for 500ms
//         digitalWrite(LED_BUILTIN, LOW);
//         vTaskDelay(pdMS_TO_TICKS(500)); // LED OFF for 500ms
//     }
// }

// // Task 2: Prints messages to the serial port
// void vSerialTask(void *pvParameters) {
//     uint32_t ulMessageCount = 0;
    
//     while (1) {
//         Serial.print("FreeRTOS Two-Task Project - Message Count: ");
//         Serial.println(ulMessageCount++);
        
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Print every 1 second
//     }
// }

// void setup() {
//     // Initialize serial communication
//     Serial.begin(115200);
//     while (!Serial) {
//         // Wait for serial port connection
//     }

//     // Create Task 1: LED Blinker
//     xTaskCreate(
//         vBlinkTask,         /* Pointer to the task function */
//         "BlinkTask",        /* Descriptive name for debugging */
//         128,                /* Stack depth in words */
//         NULL,               /* Task parameters */
//         1,                  /* Priority (1 = low priority) */
//         NULL                /* Task handle */
//     );

//     // Create Task 2: Serial Printer
//     xTaskCreate(
//         vSerialTask,        /* Pointer to the task function */
//         "SerialTask",       /* Descriptive name for debugging */
//         128,                /* Stack depth in words */
//         NULL,               /* Task parameters */
//         1,                  /* Priority */
//         NULL                /* Task handle */
//     );

//     // Start the FreeRTOS scheduler
//     vTaskStartScheduler();
// }

// void loop() {
//     // Empty when using FreeRTOS tasks. 
// }
