| Supported Targets | ESP32-S3 |
| ----------------- | -------- | 

# code-challenge

## Coding Challenge
Prepare following tools:
- Visual Studio Code with ESP-IDF 5 plugin

![Visual Studio Code with ESP-IDF 5 plugin.](vscode_espidf.png)


Provide following code functionality:
- Create two RTOS tasks running on different cores
- Task one functionality:
o This task should run every 10 ms using the RTOS delay functions
o This task has a buffer of size 100 holding the time interval for each execution
measured by the integrated ESP timing functions
o After 1 s the task should send the second task a signal getting the content of
the buffer
- Task two functionality:
o This task waits for the signal of the first task
o If signalized, the task computes minimum, maximum and mean of the values
from the buffer of the first task
o After computation, print out the values for debugging purposes
Please make sure:
- Tasks are not blocking CPU time
- Use as little as possible global variables
- Avoid threading issues

All other decisions are made by you how you would implement the functionality.
