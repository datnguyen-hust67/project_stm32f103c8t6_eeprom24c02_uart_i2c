// FILE: delay.c
#include "delay.h"
#include "stm32f10x.h"

void delay_init(void) {
    SysTick->CTRL = 0;
    SysTick->LOAD = 72000 - 1; 
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        while (!(SysTick->CTRL & (1 << 16)));
    }
}

void delay_us(uint32_t us) {
    for (uint32_t i = 0; i < us; i++) {
        for (volatile int j = 0; j < 9; j++) __NOP();
    }
}
