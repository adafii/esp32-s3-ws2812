#include "demo_colors.h"
#include "ws2812b.h"

void app_main(void) {
    init();
    show_colors(color_buffer, NUM_COLORS, 1);
}
