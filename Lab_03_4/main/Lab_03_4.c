#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

/*
 * BÀI 4: Implement a program that use the idle task hook to monitor the CPU utilization.
 * Sử dụng Idle Task Hook để đếm số lần idle task được gọi trên mỗi core
 * Từ đó tính CPU utilization = 100% - (idle_time / total_time * 100%)
 */

static const char *TAG = "CPU_MONITOR";

// Biến đếm số lần idle hook được gọi cho mỗi core
static volatile uint32_t idle_count_core0 = 0;
static volatile uint32_t idle_count_core1 = 0;

// Biến lưu giá trị cũ để tính delta
static uint32_t prev_idle_core0 = 0;
static uint32_t prev_idle_core1 = 0;
static uint32_t prev_tick_count = 0;

/*
 * vApplicationIdleHook() - Idle Task Hook
 * Hàm này được gọi mỗi khi idle task chạy
 * Chạy trên core nào thì tăng counter của core đó
 */
void vApplicationIdleHook(void)
{
    // Lấy core ID hiện tại (0 hoặc 1)
    BaseType_t core_id = xPortGetCoreID();
    
    if (core_id == 0) {
        idle_count_core0++;
    } else {
        idle_count_core1++;
    }
}

/*
 * Task để tính toán và hiển thị CPU utilization
 */
void monitor_task(void *param)
{
    while (1) {
        // Lấy tick count hiện tại
        uint32_t current_tick = xTaskGetTickCount();
        
        // Tính delta ticks và delta idle counts
        uint32_t delta_ticks = current_tick - prev_tick_count;
        uint32_t delta_idle_core0 = idle_count_core0 - prev_idle_core0;
        uint32_t delta_idle_core1 = idle_count_core1 - prev_idle_core1;
        
        // Tính CPU utilization (%)
        // Utilization = 100% - (idle_count / total_ticks * 100%)
        float util_core0 = 0.0f;
        float util_core1 = 0.0f;
        
        if (delta_ticks > 0) {
            util_core0 = 100.0f - ((float)delta_idle_core0 / (float)delta_ticks * 100.0f);
            util_core1 = 100.0f - ((float)delta_idle_core1 / (float)delta_ticks * 100.0f);
        }
        
        // In kết quả
        ESP_LOGI(TAG, "=== CPU Utilization (via Idle Hook) ===");
        ESP_LOGI(TAG, "Core 0: %.2f%% (Idle count: %lu)", util_core0, delta_idle_core0);
        ESP_LOGI(TAG, "Core 1: %.2f%% (Idle count: %lu)", util_core1, delta_idle_core1);
        ESP_LOGI(TAG, "Total ticks: %lu", delta_ticks);
        ESP_LOGI(TAG, "========================================\n");
        
        // Lưu giá trị hiện tại cho lần tính toán sau
        prev_idle_core0 = idle_count_core0;
        prev_idle_core1 = idle_count_core1;
        prev_tick_count = current_tick;
        
        // Delay 2 giây trước khi tính lại
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

/*
 * Task giả lập tải CPU (để test)
 * Chạy trên core 0
 */
void workload_task_core0(void *param)
{
    volatile uint32_t counter = 0;
    
    for(;;) {
        uint32_t start = xTaskGetTickCount();
        while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(80)) {
                for (volatile int i = 0; i < 1000; i++) {
                counter += i * 2;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/*
 * Task giả lập tải CPU (để test)
 * Chạy trên core 1
 */
void workload_task_core1(void *param)
{
    volatile uint32_t counter = 0;
    
    for(;;) {
        uint32_t start = xTaskGetTickCount();
        while ((xTaskGetTickCount() - start) < pdMS_TO_TICKS(30)) {
            for (volatile int i = 0; i < 500; i++) {
                counter += i;
            }
        }
        
        // Delay 70ms (nghỉ)
        vTaskDelay(pdMS_TO_TICKS(70));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting CPU Utilization Monitor using Idle Hook...");
    ESP_LOGI(TAG, "ESP32 Dual-Core System");
    
    // Khởi tạo giá trị ban đầu
    prev_tick_count = xTaskGetTickCount();
    prev_idle_core0 = idle_count_core0;
    prev_idle_core1 = idle_count_core1;
    
    // Tạo monitor task (chạy trên core 0)
    xTaskCreatePinnedToCore(
        monitor_task,
        "Monitor",
        4096,
        NULL,
        5,  // Priority cao hơn workload
        NULL,
        0   // Core 0
    );
    
    // Tạo workload tasks để test
    xTaskCreatePinnedToCore(
        workload_task_core0,
        "Workload_C0",
        2048,
        NULL,
        3,
        NULL,
        0   // Core 0
    );
    
    xTaskCreatePinnedToCore(
        workload_task_core1,
        "Workload_C1",
        2048,
        NULL,
        3,
        NULL,
        1   // Core 1
    );
    
    ESP_LOGI(TAG, "Tasks created. Monitoring started...");
}
