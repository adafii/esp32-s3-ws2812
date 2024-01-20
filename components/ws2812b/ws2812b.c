#include "ws2812b.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/rmt_tx.h"
#include "esp_check.h"
#include "ws2812_encoder.h"

#define CHANNEL_RESOLUTION_HZ (10 * 1000 * 1000)  // 10 MHz, 1 tick = 0.1 µs
#define MEM_BLOCK_SYMBOLS 64
#define TX_QUEUE_DEPTH 8
#define INTR_PRIORITY 0

#define GPTIMER_RESOLUTION_HZ (1 * 1000 * 1000)  // 1 MHz, 1 tick = 1 µs
#define TICKS_PER_MS (GPTIMER_RESOLUTION_HZ / (1 * 1000))

static const char* TAG = "ws2812b_component";

// Init RMT TX channel
void get_tx_channel(rmt_channel_handle_t* tx_channel) {
    const rmt_tx_channel_config_t tx_channel_config = {
        .gpio_num = GPIO_NUM,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = CHANNEL_RESOLUTION_HZ,
        .mem_block_symbols = MEM_BLOCK_SYMBOLS,
        .trans_queue_depth = TX_QUEUE_DEPTH,
        .intr_priority = INTR_PRIORITY,

        .flags.invert_out = false,
        .flags.io_loop_back = false,
        .flags.io_od_mode = false,
        .flags.with_dma = false,
    };

    ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_channel_config, tx_channel));
    ESP_ERROR_CHECK(rmt_enable(*tx_channel));
}

// Init encoder
void get_encoder(rmt_encoder_handle_t* encoder) {
    const ws2812_encoder_config_t encoder_config = {};
    ESP_ERROR_CHECK(rmt_new_ws2812_encoder(&encoder_config, encoder));
}

// Init timer
void get_timer(uint32_t delay_ms, gptimer_handle_t* gptimer) {
    const gptimer_config_t gptimer_config = {.clk_src = GPTIMER_CLK_SRC_DEFAULT,
                                             .direction = GPTIMER_COUNT_UP,
                                             .resolution_hz = GPTIMER_RESOLUTION_HZ,
                                             .intr_priority = 0};

    ESP_ERROR_CHECK(gptimer_new_timer(&gptimer_config, gptimer));
    ESP_ERROR_CHECK(gptimer_set_raw_count(*gptimer, delay_ms * TICKS_PER_MS));  // Trigger alarm immediately on start

    const gptimer_alarm_config_t alarm_config = {
        .alarm_count = delay_ms * TICKS_PER_MS, .reload_count = 0, .flags.auto_reload_on_alarm = true};

    ESP_ERROR_CHECK(gptimer_set_alarm_action(*gptimer, &alarm_config));
}

typedef struct {
    rmt_channel_handle_t tx_channel;
    rmt_encoder_handle_t encoder;
    rmt_transmit_config_t transmit_config;
    led_color_t const* color_buffer;
    size_t num_colors;
    size_t current_color_index;
} transmit_context_t;

bool transmit_cb(gptimer_handle_t timer,
                 __attribute__((unused)) const gptimer_alarm_event_data_t* edata,
                 void* user_ctx) {
    if (user_ctx == NULL) {
        return false;
    }

    transmit_context_t* transmit_context = (transmit_context_t*)user_ctx;

    rmt_channel_handle_t tx_channel = transmit_context->tx_channel;
    rmt_encoder_handle_t encoder = transmit_context->encoder;
    rmt_transmit_config_t* transmit_config = &transmit_context->transmit_config;
    led_color_t const* color_buffer = transmit_context->color_buffer;
    size_t* current_color_index = &(transmit_context->current_color_index);
    size_t num_colors = transmit_context->num_colors;

    led_color_t const* current_color = color_buffer + *current_color_index;

    ESP_ERROR_CHECK(rmt_transmit(tx_channel, encoder, current_color, sizeof(led_color_t), transmit_config));

    *current_color_index += 1;
    if (*current_color_index >= num_colors) {  // last color was transmitted
        ESP_ERROR_CHECK(gptimer_stop(timer));
        ESP_ERROR_CHECK(gptimer_disable(timer));
        ESP_ERROR_CHECK(gptimer_del_timer(timer));
        free(transmit_context);
    }

    return false;
}

void init(void) {
    // GPIO pin 11 has to be set to power the LED, this is dev-board specific setting
    gpio_reset_pin(11);
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_level(11, 1);

    gpio_reset_pin(GPIO_NUM);
    gpio_set_direction(GPIO_NUM, GPIO_MODE_OUTPUT);
}

esp_err_t show_colors(led_color_t const* color_buffer, size_t num_colors, uint32_t delay_ms) {
    rmt_channel_handle_t tx_channel = NULL;
    rmt_encoder_handle_t encoder = NULL;
    gptimer_handle_t gptimer = NULL;

    get_tx_channel(&tx_channel);
    get_encoder(&encoder);
    get_timer(delay_ms, &gptimer);

    transmit_context_t* transmit_context = calloc(1, sizeof(transmit_context_t));
    ESP_RETURN_ON_FALSE(transmit_context, ESP_ERR_NO_MEM, TAG, "out of memory when creating transmit context");

    *transmit_context = (transmit_context_t){.tx_channel = tx_channel,
                                             .encoder = encoder,
                                             .transmit_config =
                                                 (rmt_transmit_config_t){
                                                     .loop_count = 0,
                                                     .flags.eot_level = 0,
                                                 },
                                             .color_buffer = color_buffer,
                                             .num_colors = num_colors,
                                             .current_color_index = 0};

    const gptimer_event_callbacks_t gptimer_cb = {
        .on_alarm = transmit_cb,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &gptimer_cb, transmit_context));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    return ESP_OK;
}