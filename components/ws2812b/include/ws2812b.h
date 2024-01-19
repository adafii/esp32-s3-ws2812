#pragma once
#include "color.h"
#include "esp_err.h"
#include <stdint.h>

#define GPIO_NUM 46

/**
 * @brief Initialize GPIO
 */
void init(void);

/**
 * @brief Cycle trough colors in color buffer
 * @param[in] color_buffer Colors to be shown
 * @param[in] num_colors Number of colors in the buffer
 * @param[in] delay_ms Delay between switching color in ms
 * @return
 */
esp_err_t show_colors(led_color_t const* color_buffer, size_t num_colors, uint16_t delay_ms);