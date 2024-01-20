#include "ws2812_encoder.h"
#include "color.h"
#include "esp_check.h"

#define BIT0H 4
#define BIT0L 8
#define BIT1H 8
#define BIT1L 4

#define RETL 500  // low voltage time to end transfer (N x 0.1 µs), at least 50 µs

static const char* TAG = "ws2812_encoder";

typedef struct {
    rmt_encoder_t base;
    rmt_encoder_t* bytes_encoder;
    rmt_encoder_t* copy_encoder;
    rmt_symbol_word_t ws2812_ret_symbol;
    int state;
} rmt_ws2812_encoder_t;

static size_t rmt_encode_ws2812(rmt_encoder_t* encoder,
                                rmt_channel_handle_t channel,
                                const void* led_color,
                                size_t data_size,
                                rmt_encode_state_t* ret_state) {
    rmt_ws2812_encoder_t* ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    color_component_t* color = (color_component_t*)led_color;
    rmt_encoder_handle_t bytes_encoder = ws2812_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = ws2812_encoder->copy_encoder;

    switch (ws2812_encoder->state) {
        case 0:  // send color_component_t
            encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, color, data_size, &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                ws2812_encoder->state = 1;
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                break;
            }
            // fall through
        case 1:  // send ws2812_ret_symbol
            encoded_symbols += copy_encoder->encode(copy_encoder, channel, &ws2812_encoder->ws2812_ret_symbol,
                                                    sizeof(rmt_symbol_word_t), &session_state);
            if (session_state & RMT_ENCODING_COMPLETE) {
                ws2812_encoder->state = RMT_ENCODING_RESET;
            }
            if (session_state & RMT_ENCODING_MEM_FULL) {
                state |= RMT_ENCODING_MEM_FULL;
                break;
            }
    }

    *ret_state = state;
    return encoded_symbols;
}

static esp_err_t rmt_del_ws2812_encoder(rmt_encoder_t* encoder) {
    rmt_ws2812_encoder_t* ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    ESP_ERROR_CHECK(rmt_del_encoder(ws2812_encoder->bytes_encoder));
    ESP_ERROR_CHECK(rmt_del_encoder(ws2812_encoder->copy_encoder));
    free(ws2812_encoder);

    return ESP_OK;
}

static esp_err_t rmt_reset_ws2812_encoder(rmt_encoder_t* encoder) {
    rmt_ws2812_encoder_t* ws2812_encoder = __containerof(encoder, rmt_ws2812_encoder_t, base);
    ESP_ERROR_CHECK(rmt_encoder_reset(ws2812_encoder->bytes_encoder));
    ESP_ERROR_CHECK(rmt_encoder_reset(ws2812_encoder->copy_encoder));
    ws2812_encoder->state = RMT_ENCODING_RESET;

    return ESP_OK;
}

esp_err_t rmt_new_ws2812_encoder(const ws2812_encoder_config_t* config, rmt_encoder_handle_t* ret_encoder) {
    ESP_RETURN_ON_FALSE(config && ret_encoder, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    rmt_ws2812_encoder_t* ws2812_encoder = NULL;
    ws2812_encoder = calloc(1, sizeof(rmt_ws2812_encoder_t));  // Allocate & set to zero

    ESP_RETURN_ON_FALSE(ws2812_encoder, ESP_ERR_NO_MEM, TAG, "out of memory when creating ws2812 encoder");

    ws2812_encoder->base.encode = rmt_encode_ws2812;
    ws2812_encoder->base.del = rmt_del_ws2812_encoder;
    ws2812_encoder->base.reset = rmt_reset_ws2812_encoder;

    ws2812_encoder->ws2812_ret_symbol = (rmt_symbol_word_t){
        .level0 = 0,
        .duration0 = RETL,
        .level1 = 0,
        .duration1 = 0,
    };

    const rmt_bytes_encoder_config_t bytes_encoder_config = {
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
    esp_err_t bytes_enc_status = rmt_new_bytes_encoder(&bytes_encoder_config, &ws2812_encoder->bytes_encoder);

    const rmt_copy_encoder_config_t copy_encoder_config = {};
    esp_err_t copy_enc_status = rmt_new_copy_encoder(&copy_encoder_config, &ws2812_encoder->copy_encoder);

    if (copy_enc_status != ESP_OK || bytes_enc_status != ESP_OK) {
        if (ws2812_encoder->copy_encoder) {
            rmt_del_encoder(ws2812_encoder->copy_encoder);
        }
        if (ws2812_encoder->bytes_encoder) {
            rmt_del_encoder(ws2812_encoder->bytes_encoder);
        }
        free(ws2812_encoder);

        return ESP_FAIL;
    }

    *ret_encoder = &ws2812_encoder->base;
    return ESP_OK;
}