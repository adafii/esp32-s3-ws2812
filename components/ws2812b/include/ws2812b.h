#pragma once

#define GPIO_NUM            46
#define RESOLUTION_HZ       (10 * 1000 * 1000)        // 10 MHz, 1 tick = 0.1 µs
#define MEM_BLOCK_SYMBOLS   64
#define TX_QUEUE_DEPTH      4
#define INTR_PRIORITY       0

void on(void);
