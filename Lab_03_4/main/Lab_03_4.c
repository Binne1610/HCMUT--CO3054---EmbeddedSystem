#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static const char *TAG = "CPU_MONITOR";

void parse_cpu_usage(const char *stats)
{
    float idle0 = -1, idle1 = -1;
    char line[128];
    const char *ptr = stats;

    while (*ptr != 0) {
        int len = 0;
        while (ptr[len] != '\n' && ptr[len] != 0) len++;
        memcpy(line, ptr, len);
        line[len] = 0;

        if (strstr(line, "IDLE0")) {
            sscanf(line, "%*s %*u %f", &idle0);
        }
        if (strstr(line, "IDLE1")) {
            sscanf(line, "%*s %*u %f", &idle1);
        }

        ptr += len;
        if (*ptr == '\n') ptr++;
    }

    if (idle0 >= 0)
        ESP_LOGI(TAG, "Core0 Util: %.2f %%", 100.0 - idle0);
    if (idle1 >= 0)
        ESP_LOGI(TAG, "Core1 Util: %.2f %%", 100.0 - idle1);
}

void app_main(void)
{
    char *buf = malloc(4096);

    if (!buf) {
        ESP_LOGE(TAG, "Malloc failed!");
        return;
    }

    while (1) {
        memset(buf, 0, 4096);

        // Lấy thống kê runtime
        vTaskGetRunTimeStats(buf);

        // In bảng stats
        ESP_LOGI(TAG, "\n%s", buf);

        // Parse ra CPU load từng core
        parse_cpu_usage(buf);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    free(buf);
}
