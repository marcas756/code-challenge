#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "freertos/queue.h"
#include <esp_system.h>
#include <stdio.h>
#include "itempool.h"
#include "buffer.h"
#include <esp_timer.h>

#define DEBUG 1

#if DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...)
#endif

#define failure_handler(id) for(;;)

// Task1 : This task has a buffer of size 100 holding the time interval for each execution measured by the integrated ESP timing functions
#define DT_BUFF_SZ      100
#define DT_BUFF_COUNT   2
BUFFER_TYPEDEF(dt_buff_t,int64_t,DT_BUFF_SZ);


ITEMPOOL_TYPEDEF(dt_pool_t,BUFFER_T(dt_buff_t),DT_BUFF_COUNT);
ITEMPOOL_T(dt_pool_t) dt_pool;  // shared resource
SemaphoreHandle_t dt_poollock;  // shared resource access mutex
QueueHandle_t dt_msg_queue;     // Messages are buffered in contrast to notifications, but are more complex


TaskHandle_t task1_handle;

void task1_scheduling_callback(TimerHandle_t xTimer)
{
    // Notify the task to perform its operation
    xTaskNotifyGive(task1_handle);
}


// Task1 : This task should run every 10 ms using the RTOS delay functions 
#define TASK1_SCHEDULING_TIME   (10/portTICK_PERIOD_MS)


// Task to be run on Core 0
void task1(void *pvParameters) {

    BUFFER_T(dt_buff_t) *dt_curr = NULL;

    // Task 1 : Create two RTOS tasks running on different cores
    DBG("Task 1 running on core %d\n", xPortGetCoreID());




    // get and initialize first free buffer instance
    while(!dt_curr)
    {
        if ( xSemaphoreTake(dt_poollock, portMAX_DELAY) != pdTRUE )
        {
            DBG("Lock request failed\n");
            failure_handler(1);   
        }

        dt_curr = ITEMPOOL_ALLOC(dt_pool);
        xSemaphoreGive(dt_poollock);  
    }

    BUFFER_INIT(*dt_curr);


     // Create the timer
    TimerHandle_t task1_scheduling_timer = xTimerCreate(
        "task1_scheduling_timer",           // Timer name
        TASK1_SCHEDULING_TIME,    // Timer period in ticks (10 ms)
        pdTRUE,                // Auto-reload (repeated)
        (void *)task1_handle,// Timer ID (task handle)
        task1_scheduling_callback);    // Callback function

    // Start the timer
    if ( (task1_scheduling_timer == NULL) || (xTimerStart(task1_scheduling_timer, 0) != pdPASS) )
    {
        DBG("Failed starting timer\n");
        failure_handler(1); 
    }




    // Start time measurement
    int64_t dt_start = esp_timer_get_time();
    
    while (1) 
    {  
        // Task1 : This task should run every ~10 ms using the RTOS delay functions      
        // Wait indefinitely for the task to be triggered
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // Calculate the time interval
        int64_t dt_stop = esp_timer_get_time();
        BUFFER_APPEND(*dt_curr,dt_stop - dt_start);
        dt_start = dt_stop;


        // After ~1s the task should send the second task a signal getting the content of the buffer        
        if( BUFFER_FULL(*dt_curr) )
        {
            // sent IPC message with buffer to other task here
            // Messages are buffered in contrast to notifications
            xQueueSend(dt_msg_queue, (void *)&dt_curr , portMAX_DELAY);
            dt_curr = NULL;

            // get next buffer
            while(!dt_curr)
            {
                if ( xSemaphoreTake(dt_poollock, portMAX_DELAY) != pdTRUE )
                {
                    DBG("Lock request failed\n");
                    failure_handler(1);   
                }

                dt_curr = ITEMPOOL_ALLOC(dt_pool);

                
                if ( xSemaphoreGive(dt_poollock)  != pdTRUE )  
                {
                    DBG("Lock release failed\n");
                    failure_handler(1);    
                } 

                if(!dt_curr)  
                {
                    DBG("Failed to allocate buffer. Retrying ...\n");
                }
            }

            BUFFER_INIT(*dt_curr);
        }
    }
}

// Task to be run on Core 1
void task2(void *pvParameters) {

    BUFFER_T(dt_buff_t) *dt_curr = NULL;


    // Task 2 : Create two RTOS tasks running on different cores
    DBG("Task 2 running on core %d\n", xPortGetCoreID());

    while (1) 
    {

        // wait for IPC message from Task1
        xQueueReceive(dt_msg_queue, (void *)&dt_curr, portMAX_DELAY);

        // If signalized, the task computes minimum, maximum and mean of the values from the buffer of the first task

        int64_t mean = 0;
        int64_t min = INT64_MAX;
        int64_t max = INT64_MIN;

        int64_t *curr;

        BUFFER_FOREACH(*dt_curr,curr)
        {
            if ( *curr > max )
            {
                max = *curr;
            }

            if ( *curr < min )
            {
                min = *curr;
            }

            mean+=*curr;
        }
        
        mean/=BUFFER_COUNT(*dt_curr);

        
        if ( xSemaphoreTake(dt_poollock, portMAX_DELAY) != pdTRUE )
        {
            DBG("Lock request failed\n");
            failure_handler(1);   
        }

        ITEMPOOL_FREE(dt_pool,dt_curr);

        if ( xSemaphoreGive(dt_poollock)  != pdTRUE )  
        {
            DBG("Lock release failed\n");
            failure_handler(1);    
        }   

        dt_curr = NULL;


        // After computation, print out the values for debugging purposes
        DBG("Task2 : min=%lld; max=%lld; mean=%lld\n", min, max, mean); 
    }
}

void app_main() {


    // Create the mutex before starting the tasks
    dt_poollock = xSemaphoreCreateMutex();

    if (dt_poollock  == NULL) {
        DBG("Mutex creation failed\n");
        failure_handler(1);     
    }



    // Create the message queue before starting the tasks
    dt_msg_queue = xQueueCreate(DT_BUFF_COUNT, sizeof(BUFFER_T(dt_buff_t) *));

    if ( dt_msg_queue  == NULL ) {
        DBG("Failed to create queue\n");
        failure_handler(1);
    }


    // Create task1 on Core 0
    if ( xTaskCreatePinnedToCore(task1, "Task1", 2048, NULL, 2, &task1_handle, 0) != pdPASS )
    {
        DBG("Failed to create Task1 on core 0\n");
        failure_handler(1);  
    }

    // Create task2 on Core 1
    if ( xTaskCreatePinnedToCore(task2, "Task2", 2048, NULL, 1, NULL, 1) != pdPASS )
    {
        DBG("Failed to create Task2 on core 1\n");
        failure_handler(1);  
    }
}