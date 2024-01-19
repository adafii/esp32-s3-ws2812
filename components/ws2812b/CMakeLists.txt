idf_component_register(
        SRCS "ws2812b.c" "ws2812_encoder.c"
        INCLUDE_DIRS "include"
        PRIV_INCLUDE_DIRS "."
        REQUIRES "driver")