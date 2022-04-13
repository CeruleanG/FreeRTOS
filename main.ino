#include <Arduino_FreeRTOS.h>
#include <semphr.h>

struct valeurCapteurs {
    int analogique;
    int numerique;
    double tempsEnMillisecondes;
};

//RTOS 
SemaphoreHandle_t xSerialSemaphore = NULL; 
QueueHandle_t qAnalogRead; //analog reading of the potentiometer
QueueHandle_t qButton; //digital sum of 2 push buttons
QueueHandle_t qSensor; //structure "valeurCapteurs" 
QueueHandle_t qSensor4to5; //structure "valeurCapteurs" used by task 4 in order to send data

//global virable
const TickType_t xDelay = 100 / portTICK_PERIOD_MS;

//pins
const int analogPin = A0;
const int button1Pin = 3;
const int button2Pin = 4;




void setup() {
  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }

  if ( xSerialSemaphore == NULL )  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );  // Make the Serial Port available for use, by "Giving" the Semaphore.
  }

  qAnalogRead = xQueueCreate(5,sizeof(uint32_t));
  qButton = xQueueCreate(5,sizeof(uint32_t));
  qSensor = xQueueCreate(5,sizeof(valeurCapteurs));
  qSensor4to5 = xQueueCreate(5,sizeof(valeurCapteurs));

  xTaskCreate(Task1,"AnalogRead",128,NULL,1,NULL);
  xTaskCreate(Task2,"ButtonRead",128,NULL,1,NULL);
  xTaskCreate(Task3,"ConstructData",128,NULL,1,NULL);
  xTaskCreate(Task4,"SerialTest",1000,NULL,1,NULL);
  xTaskCreate(Task5,"SetDataTime",1000,NULL,1,NULL);
}

void loop() {
  // no code is loop, all is done in tasks

}

void Task1(void *pvParameters)
{
  int reading = 0;
  pinMode(analogPin, INPUT);
  while(1)
  {
    reading = analogRead(analogPin);

    /* //remove this for debug purpose
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      Serial.print ("AnalogReading => ");
      Serial.println(reading);

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }*/ //remove this for debug purpose
    xQueueSend(qAnalogRead,&reading,0);
    
    vTaskDelay( xDelay );// Delay the Task for 500ms before resume another cycle.
  }
}

void Task2(void *pvParameters)  // This is a task.
{
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);
  int reading;
  while(1){
    reading = digitalRead(button1Pin)+digitalRead(button2Pin);

    /* //remove this for debug purpose
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      Serial.print("Button status:");
      Serial.println(reading);

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }*/
    xQueueSend(qButton,&reading,0);

    vTaskDelay( xDelay  );// Delay the Task for 500ms before resume another cycle.
  }
  
}

void Task3(void *pvParameters)
{
  int analogReading;
  int buttonReading;
  while(1)
  {
    valeurCapteurs sensor = {-1,-1,-1.0};
    
    xQueueReceive(qAnalogRead,&analogReading,0);
    xQueueReceive(qButton,&buttonReading,0);

    sensor.analogique = analogReading;
    sensor.numerique = buttonReading;
    sensor.tempsEnMillisecondes = millis();
    /* //remove this for debug purpose
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      Serial.print("Structure in Task 4 is registered as: { ");
      Serial.print(sensor.analogique);
      Serial.print(" , ");
      Serial.print(sensor.numerique);
      Serial.print(" , ");
      Serial.print(sensor.tempsEnMillisecondes);
      Serial.println(" }");

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }*/
    xQueueSend(qSensor,&sensor,0);

    vTaskDelay( xDelay  );

  }
}

void Task4(void *pvParameters)
{
  valeurCapteurs sensor;
  while(1)
  {
    xQueueReceive(qSensor,&sensor,0);
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      Serial.print("Structure in Task 4 is registered as: { ");
      Serial.print(sensor.analogique);
      Serial.print(" , ");
      Serial.print(sensor.numerique);
      Serial.print(" , ");
      Serial.print(sensor.tempsEnMillisecondes);
      Serial.println(" }");

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    xQueueSend(qSensor4to5,&sensor,0);

    vTaskDelay( xDelay  );
  }
}

void Task5(void *pvParameters)
{
  valeurCapteurs sensor;
  while(1)
  {
    xQueueReceive(qSensor4to5,&sensor,0);
    valeurCapteurs sensorMn = {sensor.analogique,sensor.numerique,sensor.tempsEnMillisecondes/1000/60};
    
    // See if we can obtain or "Take" the Serial Semaphore.
    // If the semaphore is not available, wait 5 ticks of the Scheduler to see if it becomes free.
    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      // We were able to obtain or "Take" the semaphore and can now access the shared resource.
      Serial.print("Structure in Task 5 is registered as: { ");
      Serial.print(sensorMn.analogique);
      Serial.print(" , ");
      Serial.print(sensorMn.numerique);
      Serial.print(" , ");
      Serial.print(sensorMn.tempsEnMillisecondes);
      Serial.println(" }");

      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }

    vTaskDelay( xDelay  );
  }
}
