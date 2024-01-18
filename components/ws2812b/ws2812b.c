#include "ws2812b.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"

void get_tx_channel(rmt_channel_handle_t* tx_channel) {
    static rmt_tx_channel_config_t tx_channel_config = {
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
    static rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0.duration0 = BIT0H,
        .bit0.level0 = 1,
        .bit0.duration1 = BIT0L,
        .bit0.level1 = 0,

        .bit1.duration0 = BIT1H,
        .bit1.level0 = 1,
        .bit1.duration1 = BIT1L,
        .bit1.level1 = 0,

        .flags.msb_first = true,
    };

    ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_encoder_config, encoder));
}

void transmit(rmt_channel_handle_t* tx_channel, rmt_encoder_handle_t* encoder) {
    static rmt_transmit_config_t transmit_config = {
        .loop_count = 0,
        .flags.eot_level = 0,
    };

    uint8_t payload[3] = {0, 255, 0};

    ESP_ERROR_CHECK(rmt_enable(*tx_channel));
    ESP_ERROR_CHECK(rmt_transmit(*tx_channel, *encoder, payload, 3, &transmit_config));
}

void on(void) {
    gpio_reset_pin(11);
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_level(11, 1);

    gpio_reset_pin(46);
    gpio_set_direction(46, GPIO_MODE_OUTPUT);

    rmt_channel_handle_t tx_channel = NULL;
    rmt_encoder_handle_t encoder = NULL;

    get_tx_channel(&tx_channel);
    get_encoder(&encoder);

    transmit(&tx_channel, &encoder);
}