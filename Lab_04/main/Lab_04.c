#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG_R = "Reception";
static const char *TAG_A = "TaskA";
static const char *TAG_B = "TaskB";
static const char *TAG_E = "Error";

// Cấu trúc request
typedef struct { 
    int type;     // 1 → Task A, 2 → Task B
    int value;    // dữ liệu kèm theo
} Request_t;

// Queue dùng chung
static QueueHandle_t requestQueue;


// Reception Task
void reception_task(void *param)
{
    int counter = 0;

    while (1) {
        Request_t req;
        counter++;

        // Giả lập nhận request
        req.type = counter % 3;  // 1, 2, rồi 0 (loại không hợp lệ)
        req.value = counter;

        ESP_LOGI(TAG_R, "New request: type=%d value=%d", req.type, req.value);

        xQueueSend(requestQueue, &req, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task A
void task_A(void *param)
{
    Request_t req;

    while (1) {
        if (xQueueReceive(requestQueue, &req, portMAX_DELAY)) {

            if (req.type == 1) {
                ESP_LOGI(TAG_A, "Handling request value=%d", req.value);
                // xử lý...
            } else {
                // không phải request của mình → trả lại queue
                xQueueSendToFront(requestQueue, &req, 0);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
    }
}

// Task B
void task_B(void *param)
{
    Request_t req;

    while (1) {

        if (xQueueReceive(requestQueue, &req, portMAX_DELAY)) {

            if (req.type == 2) {
                ESP_LOGI(TAG_B, "Handling request value=%d", req.value);
                // xử lý...
            } else {
                // không phải của mình → trả lại queue
                xQueueSendToFront(requestQueue, &req, 0);
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
    }
}

// Error Task
void error_task(void *param)
{
    Request_t req;

    while (1) {

        // xem trước request
        if (xQueuePeek(requestQueue, &req, portMAX_DELAY)) {
            if (req.type != 1 && req.type != 2) {
                // Request không hợp lệ → lấy ra và báo lỗi
                xQueueReceive(requestQueue, &req, 0);
                ESP_LOGE(TAG_E, "Unknown request type=%d → IGNORED!", req.type);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void app_main(void)
{
    requestQueue = xQueueCreate(10, sizeof(Request_t));

    xTaskCreate(reception_task, "Reception", 4096, NULL, 3, NULL);
    xTaskCreate(task_A,         "TaskA",     4096, NULL, 2, NULL);
    xTaskCreate(task_B,         "TaskB",     4096, NULL, 2, NULL);
    xTaskCreate(error_task,     "ErrorTask", 4096, NULL, 1, NULL);
}
