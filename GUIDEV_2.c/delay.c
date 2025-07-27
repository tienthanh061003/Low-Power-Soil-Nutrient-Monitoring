#include <stdint.h>
#include "delay.h"
void delay_ms(unsigned int ms) {
    while (ms--) {
        __delay_cycles(1000);  // 1ms = 1000 cycles (1 cycle = 1µs at 1MHz)
    }
}

// Delay in microseconds
void delay_us(unsigned int us) {
    while (us--) {
        __delay_cycles(100);  // 1µs = 1 cycle (1 cycle = 1µs at 1MHz)
    }
}