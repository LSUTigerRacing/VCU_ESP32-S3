#include "lora_setup.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LORA_MOSI 1 
#define LORA_MISO 2
#define LORA_SCLK 6
#define GPIO_PIN_1 7

static spi_device_handle_t lora_spi;

void lora_spi_init(void) {
  // config bus
  spi_bus_config_t bus_config = {
    .iocfg = { LORA_MOSI, LORA_MISO, LORA_SCLK, -1, -1, -1, -1, -1, -1 }
  };

  ESP_ERROR_CHECK(
    spi_bus_initialize(
      SPI2_HOST,
      &bus_config,
      SPI_DMA_CH_AUTO
    )
  );

  // config sx1262
  spi_device_interface_config_t sx1262_config = { 
    // will need more config later after seeing what i need from our lora module
    .mode = 0,
    .flags = SPI_DEVICE_HALFDUPLEX,
    .spics_io_num = GPIO_PIN_1,
  };

  ESP_ERROR_CHECK(
    spi_bus_add_device(
      SPI2_HOST,
      &sx1262_config,
      &lora_spi
    )
  );
}
