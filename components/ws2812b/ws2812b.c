#include "ws2812b.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ws2812_encoder.h"

#define RESOLUTION_HZ (10 * 1000 * 1000)  // 10 MHz, 1 tick = 0.1 µs
#define MEM_BLOCK_SYMBOLS 64
#define TX_QUEUE_DEPTH 4
#define INTR_PRIORITY 0

void get_tx_channel(rmt_channel_handle_t* tx_channel) {
    static const rmt_tx_channel_config_t tx_channel_config = {
        .gpio_num = GPIO_NUM,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = RESOLUTION_HZ,
        .mem_block_symbols = MEM_BLOCK_SYMBOLS,
        .trans_queue_depth = TX_QUEUE_DEPTH,
        .intr_priority = INTR_PRIORITY,

        .flags.invert_out = false,
        .flags.io_loop_back = false,
        .flags.io_od_mode = false,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_config, tx_channel));
}

void get_encoder(rmt_encoder_handle_t* encoder) {
    const ws2812_encoder_config_t encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_ws2812_encoder(&encoder_config, encoder));
}

void transmit(rmt_channel_handle_t* tx_channel,
              rmt_encoder_handle_t* encoder,
              led_color_t const* color_buffer,
              size_t num_colors,
              uint16_t delay_ms) {
    static rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags.eot_level = 0,
    };

    ESP_ERROR_CHECK(rmt_enable(*tx_channel));

    for (size_t i = 0; i < num_colors; ++i) {
        led_color_t const* current_color = color_buffer + i;
        ESP_ERROR_CHECK(rmt_transmit(*tx_channel, *encoder, current_color, sizeof(led_color_t), &transmit_config));
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
    }
}

void init(void) {
    // GPIO pin 11 has to be set to power the led, this is devboard specific setting
    gpio_reset_pin(11);
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_level(11, 1);

    gpio_reset_pin(GPIO_NUM);
    gpio_set_direction(GPIO_NUM, GPIO_MODE_OUTPUT);
}

esp_err_t show_colors(led_color_t const* color_buffer, size_t num_colors, uint16_t delay_ms) {
    rmt_channel_handle_t tx_channel = NULL;
    rmt_encoder_handle_t encoder = NULL;

    get_tx_channel(&tx_channel);
    get_encoder(&encoder);

    if (delay_ms < 50) {
        delay_ms = 50;
    }

    transmit(&tx_channel, &encoder, color_buffer, num_colors, delay_ms);

    return ESP_OK;
}