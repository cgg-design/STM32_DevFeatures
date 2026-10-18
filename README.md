## Project Objective
### Onboarding STM32F446RE
This project serves as an introduction to developing and executing multi-task applications with FreeRTOS. For the onboarding, it implements a simple two-task structure consisting of:
- Task 1: Controls the onboard LED blinky routine.
- Task 2: Periodically prints status messages to the serial port.

### Build a robust 3-task system (Synchronized Tasks)
- Task 1 (High Priority - Sensor/Input): Captures data or triggers an event.
- Task 2 (Medium Priority - Processing): Processes the data received from Task 1.
- Task 3 (Low Priority - Output/Logging): Prints the final results to the Serial monitor or toggles an LED.

### PlatformIO Core & Jenkins
- 
### STM32F446RE CPU Feature