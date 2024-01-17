#include "ws2812b.h"
#include "driver/gpio.h"
#include "driver/rmt_tx.h"

rmt_channel_handle_t install_tx_channel(void) {
    rmt_tx_channel_config_t tx_channel_config = {
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

    rmt_channel_handle_t tx_channel = NULL;
    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_config, &tx_channel));

    return tx_channel;
}

void on(void) {
    gpio_reset_pin(11);
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_level(11, 1);

    gpio_reset_pin(46);
    gpio_set_direction(46, GPIO_MODE_OUTPUT);
}
