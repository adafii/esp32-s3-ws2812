#include "ws2812b.h"
#include "driver/gpio.h"
#include "esp_err.h"

void on(void) {
    gpio_reset_pin(11);
    gpio_set_direction(11, GPIO_MODE_OUTPUT);
    gpio_set_level(11, 1);

    gpio_reset_pin(46);
    gpio_set_direction(46, GPIO_MODE_OUTPUT);
}
