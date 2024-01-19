#include "ws2812b.h"

void app_main(void) {
    init();

    led_color_t const color_buffer[3] = {{255, 0, 0},
                                         {0, 255, 0},
                                         {0, 0, 255}};

    show_colors(color_buffer, 3, 1000);
}
