// FILE: lcd.c
#include "lcd.h"
#include "delay.h"
#include "stm32f10x.h"
#include <stdint.h>

static void lcd_pulse_enable(void) {
    GPIOB->BSRR = (1 << 11);  
    delay_us(1);
    GPIOB->BRR = (1 << 11);  
    delay_us(100);
}

static void lcd_send_4bit(uint8_t data) {
    // Xoa 4 bit cu
    GPIOB->BRR = (0xF << 12);
    
    // Ghi 4 bit moi
    if (data & 1) GPIOB->BSRR = (1 << 12);
    if (data & 2) GPIOB->BSRR = (1 << 13);
    if (data & 4) GPIOB->BSRR = (1 << 14);
    if (data & 8) GPIOB->BSRR = (1 << 15);
}

static void lcd_send_cmd(uint8_t cmd) {
    GPIOB->BRR = (1 << 10);   // RS = 0 (command)
    lcd_send_4bit(cmd >> 4);  // Gui 4 bit cao
    lcd_pulse_enable();
    lcd_send_4bit(cmd & 0x0F); // Gui 4 bit thap
    lcd_pulse_enable();
    delay_us(40);             // Thoi gian thuc thi lenh
}

static void lcd_send_data(uint8_t data) {
    GPIOB->BSRR = (1 << 10);  // RS = 1 (data)
    lcd_send_4bit(data >> 4); // Gui 4 bit cao
    lcd_pulse_enable();
    lcd_send_4bit(data & 0x0F); // Gui 4 bit thap
    lcd_pulse_enable();
    delay_us(40);             // Thoi gian ghi data
}

void lcd_init(void) {
    // Bat clock cho GPIOB
    RCC->APB2ENR |= (1 << 3);
    
    // Cau hinh PB10-PB15 la output push-pull 2MHz
    // PB10=RS, PB11=EN, PB12-15=D4-D7
    GPIOB->CRH &= ~(0xFFFFFFFF);
    GPIOB->CRH |=  (0x22222222);
    
    // Khoi tao LCD theo chuan HD44780
    delay_ms(20);           
    
    // Sequence khoi tao 4-bit mode
    lcd_send_4bit(0x03); 
    lcd_pulse_enable();
    delay_ms(5);
    
    lcd_send_4bit(0x03); 
    lcd_pulse_enable();
    delay_us(100);
    
    lcd_send_4bit(0x03); 
    lcd_pulse_enable();
    delay_us(100);
    
    lcd_send_4bit(0x02);    
    lcd_pulse_enable();
    
    // Cau hinh LCD
    lcd_send_cmd(0x28);      // 4-bit, 2 lines, 5x8 font
    lcd_send_cmd(0x0C);      // Display ON, cursor OFF, blink OFF
    lcd_send_cmd(0x06);      // Entry mode: increment, no shift
    lcd_send_cmd(0x01);      // Clear display
    delay_ms(2);
}

void lcd_clear(void) {
    lcd_send_cmd(0x01);
    delay_ms(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    uint8_t address;
    if (row == 0) {
        address = 0x80 + col;   
    } else {
        address = 0xC0 + col;  
    }
    lcd_send_cmd(address);
}

void lcd_write_char(char c) {
    lcd_send_data(c);
}

void lcd_write_string(const char *str) {
    while (*str) {
        lcd_write_char(*str++);
    }
}