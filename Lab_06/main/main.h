#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"    
#include "esp_netif.h"    
#include "esp_mac.h"      
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>       

// CẤU HÌNH WIFI CẦN KẾT NỐI
#define AP_SSID      "702H6-KH@KTMT"
#define AP_PASS      "svkhktmt"
static const char *TAG = "WIFI_STA";

// HÀM XỬ LÝ SỰ KIỆN
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  // Xử lý cho các sự kiện liên quan đến wifi
  if (event_base == WIFI_EVENT) {
      switch (event_id) {
        
          // Hệ thống wifi đã khởi động và sẵn sàng
          case WIFI_EVENT_STA_START:
              printf("[%s] WiFi Station started. Attempting to connect to %s...\n", TAG, AP_SSID);
              ESP_ERROR_CHECK(esp_wifi_connect());
              break;
        
          // Đã kết nối với Access Point thành công
          case WIFI_EVENT_STA_CONNECTED:
              printf("[%s] Successfully connected to AP, waiting for IP...\n", TAG);
              break;
        
          // Đã ngắt kết nối khỏi Access Point
          case WIFI_EVENT_STA_DISCONNECTED:
              printf("[%s] Disconnected from AP. Retrying connection...\n", TAG);
              ESP_ERROR_CHECK(esp_wifi_connect());
              break;
        
          default:
              break;
      }
  }
}

void app_main(void)
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
   ESP_ERROR_CHECK(esp_netif_init());
   ESP_ERROR_CHECK(esp_event_loop_create_default());

  esp_netif_create_default_wifi_sta();

  // Đăng ký cho các sự kiện WIFI_EVENT
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
   // Khởi tạo và cấu hình wifi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   // Đặt chế độ hoạt động là Station (STA)
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Thiết lập cấu hình SSID và Password
  wifi_config_t sta_config = {
      .sta = {
          .ssid = AP_SSID,
          .password = AP_PASS,
          .bssid_set = 0
      },
  };
   // Áp dụng cấu hình Station
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));

  // Khởi động wifi
  ESP_ERROR_CHECK(esp_wifi_start());
}
