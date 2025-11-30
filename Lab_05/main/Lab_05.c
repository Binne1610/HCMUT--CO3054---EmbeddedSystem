#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

static const char *TAG = "TIMER_LAB";

// 1. CẤU TRÚC DỮ LIỆU ĐỊNH DANH TIMER
typedef struct {
  int max_count;    
  int current_count;
  const char *message;
} TimerData_t;

static TimerData_t timer1_data;
static TimerData_t timer2_data;

static TimerHandle_t xTimer1 = NULL;
static TimerHandle_t xTimer2 = NULL;

// --- 2. HÀM CALLBACK CHIA SẺ (EXECUTED BY DAEMON TASK) ---
void SharedTimerCallback(TimerHandle_t xTimer) {
   TimerData_t *pData = (TimerData_t *)pvTimerGetTimerID(xTimer);
   if (pData == NULL) {
      return;
  }

  pData->current_count++;
   printf("[%s] Printing: %s. Count: %d/%d\n",
           pcTimerGetName(xTimer),
           pData->message,
           pData->current_count,
           pData->max_count);

  // Kiểm tra nếu đạt đến số lần in max thì dừng timer
  if (pData->current_count == pData->max_count) {
      xTimerStop(xTimer, 0);
  }
}

void app_main(void) {
   // KHỞI TẠO CẤU TRÚC DỮ LIỆU CHO 2 TIMER
  timer1_data.max_count = 10;
  timer1_data.current_count = 0;
  timer1_data.message = "ahihi";
   timer2_data.max_count = 5;
  timer2_data.current_count = 0;
  timer2_data.message = "ihaha";

  // TẠO TIMER1 CHO AHIHI
  xTimer1 = xTimerCreate(
      "Timer_Ahihi",
      pdMS_TO_TICKS(2000),
      pdTRUE,
      (void *)&timer1_data,
      SharedTimerCallback
  );

  // TẠO TIMER1 CHO IHAHA
  xTimer2 = xTimerCreate(
      "Timer_Ihaha",
      pdMS_TO_TICKS(3000),
      pdTRUE,
      (void *)&timer2_data,
      SharedTimerCallback
  );

  // KHỞI ĐỘNG TIMER
  if (xTimer1 != NULL && xTimer2 != NULL) {
      xTimerStart(xTimer1, 0);
      xTimerStart(xTimer2, 0);
  }
}
