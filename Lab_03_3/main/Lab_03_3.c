#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static TaskHandle_t task1_handle = NULL;
static TaskHandle_t task2_handle = NULL;

// Task1 – High priority
void task1(void *parameter)
{
    for(;;)
    {
        printf("Task1 RUNNING\n");
        vTaskDelay(pdMS_TO_TICKS(300));

        printf("Task1 BLOCKED (simulate semaphore wait)\n");
        vTaskSuspend(NULL);    // mô phỏng block
    }
}

// Task2 – Medium priority
void task2(void *parameter)
{
    for(;;)
    {
        printf("Task2 RUNNING\n");
        vTaskDelay(pdMS_TO_TICKS(300));

        printf("Task2 BLOCKED (simulate queue wait)\n");
        vTaskSuspend(NULL);    // mô phỏng block
    }
}

// Task3 – Low priority
void task3(void *parameter)
{
    for(;;)
    {
        printf("Task3 RUNNING\n");
        vTaskDelay(pdMS_TO_TICKS(300));

        // t2 — Task2 Ready nhưng không preempt
        printf("Task3 → Unblock Task2 (Task2 READY)\n");
        vTaskResume(task2_handle);

        vTaskDelay(pdMS_TO_TICKS(300));

        // t3 — Task1 Ready nhưng không preempt
        printf("Task3 → Unblock Task1 (Task1 READY)\n");
        vTaskResume(task1_handle);

        vTaskDelay(pdMS_TO_TICKS(300));

        // t4 — Task3 tự yield → Task1 chạy
        printf("Task3 → taskYIELD() → chuyển CPU cho Task1\n");
        taskYIELD();   // lúc này Task1 (priority cao nhất) chạy
    }
}

void app_main(void)
{
    // Tạo task
    xTaskCreate(task1, "Task1", 2048, NULL, 3, &task1_handle);
    xTaskCreate(task2, "Task2", 2048, NULL, 2, &task2_handle);
    xTaskCreate(task3, "Task3", 2048, NULL, 1, NULL);

    // Ban đầu Task1 và Task2 ở trạng thái Blocked (như trong hình)
    vTaskSuspend(task1_handle);
    vTaskSuspend(task2_handle);
}
