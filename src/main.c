#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "lora_setup.h"

static const char *TAG = "MAIN";

void app_main(void) {
    /* hardware initialize stuff */
    ESP_LOGI(TAG, "Fresh ESP-IDF Project Started on ESP32-S3!");

    lora_spi_init();
  
    /* tasks here */
    while (1) {
        ESP_LOGI(TAG, "FreeRTOS IDLE loop ticking...");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
