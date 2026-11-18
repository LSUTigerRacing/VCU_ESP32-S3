#include <stdio.h>
#include <stdint.h>

#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

#define STM32_UART_PORT UART_NUM_1
#define STM32_UART_TX   GPIO_NUM_2
#define STM32_UART_RX   GPIO_NUM_1
#define STM32_BAUD_RATE 115200

#define AN3155_SYNC 0x7F
#define AN3155_ACK  0x79
#define AN3155_NACK 0x1F

static esp_err_t configure_uart(void) {
    const uart_config_t config = {
        .baud_rate = STM32_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    esp_err_t err = uart_driver_install(STM32_UART_PORT, 256, 0, 0, NULL, 0);
    if (err != ESP_OK) {
        return err;
    }

    err = uart_param_config(STM32_UART_PORT, &config);
    if (err != ESP_OK) {
        return err;
    }

    return uart_set_pin(
        STM32_UART_PORT,
        STM32_UART_TX,
        STM32_UART_RX,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    );
}

void app_main(void) {
    esp_err_t err = configure_uart();
    if (err != ESP_OK) {
        printf("uart setup failed: %s\n", esp_err_to_name(err));
        return;
    }

    printf("uart ready, waiting 5 seconds before sending sync byte 0x7F\n");
    vTaskDelay(pdMS_TO_TICKS(5000));

    uart_flush_input(STM32_UART_PORT);

    const uint8_t sync = AN3155_SYNC;
    int written = uart_write_bytes(STM32_UART_PORT, &sync, sizeof(sync));
    uart_wait_tx_done(STM32_UART_PORT, pdMS_TO_TICKS(100));

    if (written != sizeof(sync)) {
        printf("failed to transmit sync byte \n");
        return;
    }

    printf("sent sync byte, waiting 3 seconds for stm response");
    uint8_t response;
    int received = uart_read_bytes(STM32_UART_PORT, &response, 1, pdMS_TO_TICKS(3000));

    if (received == 1 && response == AN3155_ACK) {
        printf("stm responded ack!\n");
    } else if (received == 1 && response == AN3155_NACK) {
        printf("stm responded nack?!\n");
    } else if (received == 1) {
        printf("received unexpected %02X", response);
    } else {
        printf("no response\n");
    }

    printf("test finished");
}
