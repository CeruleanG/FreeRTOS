# FreeRTOS

The program sends the analog reading of the potentiometer from task 1, into a queue named "qAnalogRead". It does the same thing with numeric reading of the button from task 2, into a second queue named "qButton".
Task 3 takes both values and make the "valeurCapteurs" structure and send it into the "qSensor" while the task 4 displaying said structure with serial monitor(the mutex semaphore invervenes here).
Finally, the last task, task 5 converts the time in milliseconds into minutes and does another display.

## Files

### Jingwen_CHEN.ino
The code Arduino of the work.

### diagram.json
The schematic of the work.

### librairies.txt
Librairies used in this work.

### wokwi-project.txt
Thanks https://wokwi.com for sponso... Ahem, this work is simulated on https://wokwi.com
