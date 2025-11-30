#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

/*
 * BÀI 2: Pre-emptive WITHOUT Time Slicing
 * - configUSE_PREEMPTION = 1
 * - configUSE_TIME_SLICING = 0
 *
 * Task:
 *   - Hai task cùng priority (A và B) nhưng chỉ Task A chạy mãi.
 *   - Task C có priority cao hơn → preempt được Task A.
 */

void TaskA(void *pvParameters)
{
    for(;;)
    {
        printf("Task A running...\n");

        // Tránh WDT reset 
        __asm__ __volatile__("nop");
    }
}

void TaskB(void *pvParameters)
{
    for(;;)
    {
        printf("Task B running...\n");
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void TaskC(void *pvParameters)
{
    for(;;)
    {
        printf(">>> Task C is running\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    // In không buffer để log hiện ngay
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("=== BAI 2: Pre-emptive WITHOUT Time Slicing ===\n");
    printf("configUSE_PREEMPTION = %d\n", configUSE_PREEMPTION);
    printf("configUSE_TIME_SLICING = %d\n", configUSE_TIME_SLICING);

    // Hai task cùng priority = 1
    xTaskCreate(TaskA, "TaskA", 2048, NULL, 1, NULL);
    xTaskCreate(TaskB, "TaskB", 2048, NULL, 1, NULL);

    // Task C priority cao hơn
    xTaskCreate(TaskC, "TaskC", 2048, NULL, 2, NULL);
}
