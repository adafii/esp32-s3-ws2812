#pragma once
#include "driver/rmt_encoder.h"
#include <stdint.h>

#define RESOLUTION_HZ (10 * 1000 * 1000)  // 10 MHz, 1 tick = 0.1 µs
#define MEM_BLOCK_SYMBOLS 48
#define TX_QUEUE_DEPTH 4
#define INTR_PRIORITY 0

#define BIT0H 4
#define BIT0L 8
#define BIT1H 8
#define BIT1L 4

#define RETL 500 // low voltage time (N x 0.1 µs)

/**
 * @brief Single color component is 8 bits
 */
typedef uint8_t color_component_t;

/**
 * @brief Led color representation (24 bit GRB)
 */
typedef struct {
    color_component_t green;
    color_component_t red;
    color_component_t blue;
} led_color_t;

/**
 * @brief Type of ws2812 encoder configuration
 */
typedef struct {
} ws2812_encoder_config_t;

/**
 * @brief Create RMT encoder for encoding color_component_t for a single WS2812 led
 *
 * @param[in] config Encoder configuration
 * @param[out] ret_encoder Resulting encoder handle
 * @return
 *      - ESP_ERR_INVALID_ARG for any invalid arguments
 *      - ESP_ERR_NO_MEM out of memory when creating WS2812 encoder
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_ws2812_encoder(const ws2812_encoder_config_t* config, rmt_encoder_handle_t* ret_encoder);