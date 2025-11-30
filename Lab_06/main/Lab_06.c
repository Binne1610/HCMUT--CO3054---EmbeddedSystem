#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"    
#include "esp_netif.h"    
#include "esp_mac.h"      
#include "nvs_flash.h"
#include <stdio.h>
#include <string.h>

// CẤU HÌNH CHO AP
#define EXAMPLE_AP_SSID      "ESP32_GT"
#define EXAMPLE_AP_PASS      "password123"
#define MAX_CONNECTIONS_AP   4
static const char *TAG = "AP_SETUP";

// HÀM XỬ LÝ SỰ KIỆN (EVENT HANDLER)
static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  // Xử lý cho các sự kiện liên quan tới wifi
  if (event_base == WIFI_EVENT) {
      switch (event_id) {
        
          // 1. AP đã khởi động thành công
          case WIFI_EVENT_AP_START:
              printf("[%s] Access Point started successfully. SSID: %s | Password: %s\n", TAG, EXAMPLE_AP_SSID, EXAMPLE_AP_PASS);
              break;
        
          // 2. Station kết nối
          case WIFI_EVENT_AP_STACONNECTED: {
              wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
              printf("[%s] Station connected. Client ID: %d\n", TAG, event->aid);
              break;
          }
        
          // 3. Station ngắt kết nối
          case WIFI_EVENT_AP_STADISCONNECTED: {
              wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
              printf("[%s] Station disconnected. Client ID: %d\n", TAG, event->aid);
              break;
          }
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
   // 1. Khởi tạo Event Loop  định
  ESP_ERROR_CHECK(esp_event_loop_create_default());
   esp_netif_create_default_wifi_ap();


  // 2. Đăng ký hàm handler cho các sự kiện WIFI_EVENT
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));


  // 3. Khởi tạo và ccấu hình wifi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
   // 4. Đặt chế độ hoạt động Access Point
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

  // 5. Thiết lập cấu hình cho AP
  wifi_config_t ap_config = {
      .ap = {
          .ssid = EXAMPLE_AP_SSID,
          .password = EXAMPLE_AP_PASS,
          .ssid_len = strlen(EXAMPLE_AP_SSID),
          .channel = 1,
          .authmode = WIFI_AUTH_WPA2_PSK,
          .ssid_hidden = 0,
          .max_connection = MAX_CONNECTIONS_AP,
          .beacon_interval = 100
      },
  };
   ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));

  ESP_ERROR_CHECK(esp_wifi_start());  //khởi động wifi
}
