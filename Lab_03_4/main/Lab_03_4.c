#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "esp_timer.h"

volatile uint64_t idle_count_core0 = 0;
volatile uint64_t idle_count_core1 = 0;

const float tick_rate =  configTICK_RATE_HZ;

// Idle hook cho core 0
bool idle_hook_core0(void)
{
    idle_count_core0++;
    return true;
}

// Idle hook cho core 1
bool idle_hook_core1(void)
{
    idle_count_core1++;
    return true;
}

// Task hiển thị CPU utilization
void cpu_monitor_task(void *arg)
{
    uint64_t prev_idle0 = 0, prev_idle1 = 0;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));

        uint64_t idle0 = idle_count_core0;
        uint64_t idle1 = idle_count_core1;

        uint64_t diff0 = idle0 - prev_idle0;
        uint64_t diff1 = idle1 - prev_idle1;

        prev_idle0 = idle0;
        prev_idle1 = idle1;

        float cpu0 = 100.0f * (1.0f - (float)diff0 / tick_rate);
        float cpu1 = 100.0f * (1.0f - (float)diff1 / tick_rate);

        if (cpu0 < 0) cpu0 = 0;
        if (cpu1 < 0) cpu1 = 0;

        printf("CPU Utilization: Core0 = %.2f%%, Core1 = %.2f%%\n",
                cpu0, cpu1);
    }
}

void app_main(void)
{
    // Đăng ký Idle Hook
    esp_register_freertos_idle_hook_for_cpu(idle_hook_core0, 0);
    esp_register_freertos_idle_hook_for_cpu(idle_hook_core1, 1);

    xTaskCreatePinnedToCore(cpu_monitor_task,
                            "cpu_monitor",
                            4096,
                            NULL,
                            1,
                            NULL,
                            0);
}
