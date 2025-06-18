// FILE: uart.h
#ifndef __UART_H
#define __UART_H
void uart_init(void);
void uart_send_char(char c);
void uart_send_string(const char *str);
char uart_receive_char(void);
int uart_available(void);
#endif