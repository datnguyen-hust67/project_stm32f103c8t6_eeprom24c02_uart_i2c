// FILE: lcd.h
#ifndef __LCD_H
#define __LCD_H

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_write_char(char c);
void lcd_write_string(const char *str);

#endif
