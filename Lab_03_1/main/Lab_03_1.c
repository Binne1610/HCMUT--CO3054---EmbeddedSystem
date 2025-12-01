#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "stdio.h"

/*
 * Bài số 1: Pre-emptive Scheduling WITH Time Slicing
 * - configUSE_PREEMPTION = 1
 * - configUSE_TIME_SLICING = 1
 *
 * Task:
 *   Task1 (prio 2)      → đọc nút, mỗi 1 giây in "ESP32"
 *   task_printf (prio 1) → in CPU usage
 *   Task2 (prio 0)      → vòng lặp vô hạn chiếm CPU
 *   Idle Task (prio 0)   → tăng idle_count
 */

volatile uint32_t ulIdleCycleCount = 0UL;
volatile uint32_t task2_count = 0;

// Idle hook ghi nhận CPU rảnh
void vApplicationIdleHook(void)
{
    ulIdleCycleCount++;
}

// Task 1: đọc nút và in ra
void Task1(void *pvParameters)
{
    const gpio_num_t BUTTON_GPIO = GPIO_NUM_0;

    esp_rom_gpio_pad_select_gpio(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    for (;;)
    {
        if (gpio_get_level(BUTTON_GPIO) == 0)
        {
            printf(">>> ESP32\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task 2: vòng lặp bận chiếm CPU
void Task2(void *pvParameters)
{
    for (;;)
    {
        task2_count++;
    }
}

// Task in số liệu CPU
void Task_Print(void *pvParameters)
{
    for (;;)
    {
        printf("Idle Count: %lu\n", ulIdleCycleCount);
        printf("Task2 Count: %lu\n", task2_count);
        printf("---------------------------\n");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
    printf("=== BAI 1: Pre-emptive WITH Time Slicing ===\n");
    printf("configUSE_PREEMPTION = %d\n", configUSE_PREEMPTION);
    printf("configUSE_TIME_SLICING = %d\n", configUSE_TIME_SLICING);

    xTaskCreate(Task1, "Task1", 2048, NULL, 2, NULL);
    xTaskCreate(Task2, "Task2", 2048, NULL, 0, NULL);
    xTaskCreate(Task_Print, "Print", 2048, NULL, 1, NULL);
}
