# esp32-s3-ws2812
Example of controlling single WS2812B RGB led with ESP32-S3 Remote Control Transceiver (RMT) and General Purpose Timer (GPTimer). Building the project requires ESP-IDF version v5.1.2 or newer.

The goal of this project was to learn to use hardware peripherals to generate accurate output signal. WS2812B has only ±150 ns tolerance for pulse delay.

Note that there exists a ready-made component for driving addressable LEDs like WS2812: [https://github.com/espressif/idf-extra-components/tree/master/led_strip](https://github.com/espressif/idf-extra-components/tree/master/led_strip).

![gradient](https://github.com/adafii/esp32-s3-ws2812b/assets/136560098/b34b1961-4059-4409-96fa-ca5856d05f90)
