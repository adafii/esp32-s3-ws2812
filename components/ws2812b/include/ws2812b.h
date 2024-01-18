#pragma once

#define GPIO_NUM 46
#define RESOLUTION_HZ (10 * 1000 * 1000)  // 10 MHz, 1 tick = 0.1 µs
#define MEM_BLOCK_SYMBOLS 48
#define TX_QUEUE_DEPTH 4
#define INTR_PRIORITY 0

#define BIT0H 4
#define BIT0L 8
#define BIT1H 8
#define BIT1L 4

void on(void);
