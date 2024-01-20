#include "ws2812b.h"

#define NUM_COLORS 3
static led_color_t const color_buffer[NUM_COLORS] = {{255, 255, 0}, {0, 255, 255}, {255, 0, 255}};

void app_main(void) {
    init();
    show_colors(color_buffer, NUM_COLORS, 3000);
}
