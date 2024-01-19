/**
 * @file Interface for ws2812 encoder
 */

#pragma once
#include "driver/rmt_encoder.h"

/**
 *  @brief Encoder configuration
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
 *      - ESP_FAIL if creating encoder failed because of other error
 *      - ESP_OK if creating encoder successfully
 */
esp_err_t rmt_new_ws2812_encoder(const ws2812_encoder_config_t* config, rmt_encoder_handle_t* ret_encoder);