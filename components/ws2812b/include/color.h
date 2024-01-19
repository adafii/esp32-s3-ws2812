#pragma once
#include <stdint.h>

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
