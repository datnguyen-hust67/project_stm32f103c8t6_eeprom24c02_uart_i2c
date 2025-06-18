#include "uart.h"
#include "stm32f10x.h"

void uart_init(void) {
    // Bat clock cho GPIOA và USART1
    RCC->APB2ENR |= (1 << 2) | (1 << 14);  // GPIOA + USART1
    
    // Cau hình GPIO
    // PA9 = TX (Alternate function push-pull)
    // PA10 = RX (Floating input)
    GPIOA->CRH &= ~((0xF << 4) | (0xF << 8));  // Clear PA9, PA10
    GPIOA->CRH |=  ((0xB << 4) | (0x4 << 8));  // PA9=AF_PP, PA10=Float_IN
    
    // Cau hinh USART1
    // BRR = f_PCLK2 / baudrate = 72MHz / 9600 = 7500
    USART1->BRR = 7500; 
    
    // Bat RX và TX
    USART1->CR1 |= (1 << 2) | (1 << 3);  // RE=1, TE=1
    
    // Bat USART
    USART1->CR1 |= (1 << 13);  // UE=1
}

void uart_send_char(char c) {
    while (!(USART1->SR & (1 << 7)));
    USART1->DR = c;
}

void uart_send_string(const char *str) {
    while (*str) {
        uart_send_char(*str++);
    }
}

char uart_receive_char(void) {
    while (!(USART1->SR & (1 << 5)));
    return (char)USART1->DR;
}

int uart_available(void) {
    return (USART1->SR & (1 << 5)) ? 1 : 0;
}