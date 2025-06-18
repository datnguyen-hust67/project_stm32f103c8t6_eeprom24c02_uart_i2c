// FILE: main.c 
#include "uart.h"
#include "lcd.h"
#include "delay.h"
#include "eeprom.h"
#include "password.h"
#include <string.h>
#include <stdio.h>  

#define MAX_PASSWORD_LEN 6

static char buffer[32];
static int buf_index = 0;
static int wrong_attempts = 0;
static int waiting_yesno = 0;
static int entering_new_pass = 0;

// ham hien thi mat khau duoi dang dau *
static void display_password_hidden(int length) {
    lcd_set_cursor(1, 0);
    for (int i = 0; i < length; i++) {
        lcd_write_char('*');
    }
    for (int i = length; i < 16; i++) {
        lcd_write_char(' ');
    }
}

void xu_ly_du_lieu() {
    buffer[buf_index] = '\0';
    
    // Debug qua UART
    uart_send_string("\r\n[DEBUG] Processing: ");
    uart_send_string(buffer);
    uart_send_string("\r\n");
    
    // xoa man hinh truoc khi xu ly
    lcd_clear();
    delay_ms(10);
    
    if (waiting_yesno) {
        uart_send_string("[DEBUG] Processing yes/no\r\n");
        if (strcmp(buffer, "yes") == 0) {
            entering_new_pass = 1;
            waiting_yesno = 0;
            lcd_set_cursor(0, 0);
            lcd_write_string("Nhap pass moi:");
            uart_send_string("[DEBUG] Entering new password mode\r\n");
        } else {
            waiting_yesno = 0;
            lcd_set_cursor(0, 0);
            lcd_write_string("Pass or 'update'");
            uart_send_string("[DEBUG] Back to normal mode\r\n");
        }
    } 
    else if (entering_new_pass) {
        uart_send_string("[DEBUG] Processing new password\r\n");
        if (strlen(buffer) > 0 && strlen(buffer) <= MAX_PASSWORD_LEN) {
            password_update(buffer);
            lcd_set_cursor(0, 3);
            lcd_write_string("MK da doi!");
            delay_ms(2000);
            uart_send_string("[DEBUG] Password updated successfully\r\n");
        } else {
            lcd_set_cursor(0, 0);
            lcd_write_string("Sai dinh dang!");
            delay_ms(1500);
            uart_send_string("[DEBUG] Invalid password format\r\n");
        }
        entering_new_pass = 0;
        lcd_clear();
        delay_ms(10);
        lcd_set_cursor(0, 0);
        lcd_write_string("Pass or 'update'");
    } 
    else if (strcmp(buffer, "update") == 0) {
        uart_send_string("[DEBUG] Update command received\r\n");
        lcd_set_cursor(0, 0);
        lcd_write_string("Thay doi pass?");
        lcd_set_cursor(1, 0);
        lcd_write_string("Nhap: yes/no");
        waiting_yesno = 1;
    } 
    else {
        // Kiem tra mat khau
        uart_send_string("[DEBUG] Checking password\r\n");
        if (password_check(buffer)) {
            lcd_set_cursor(0, 0);
            lcd_write_string("Mat khau dung!");
            wrong_attempts = 0;
            delay_ms(2000);
            lcd_clear();
            delay_ms(10);
            lcd_set_cursor(0, 0);
            lcd_write_string("Pass or 'update'");
        } else {
            lcd_set_cursor(0, 0);
            lcd_write_string("Mat khau sai!");
            wrong_attempts++;
            delay_ms(1500);
            
            if (wrong_attempts >= 3) {
                // Khoa 15s
                for (int i = 15; i >= 0; i--) {
                    lcd_clear();
                    delay_ms(10);
                    lcd_set_cursor(0, 0);
                    lcd_write_string("Sai 3 lan!");
                    lcd_set_cursor(1, 0);
                    char countdown[16];
                    sprintf(countdown, "Khoa: %ds", i);
                    lcd_write_string(countdown);
                    delay_ms(1000);
                }
                wrong_attempts = 0;
            }
            
            lcd_clear();
            delay_ms(10);
            lcd_set_cursor(0, 0);
            lcd_write_string("Pass or 'update'");
        }
    }
    
    buf_index = 0;
}

int main(void) {
    // Khoi tao cac module
    uart_init();
    delay_init();
    eeprom_init();
    lcd_init();
    
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_write_string("Nhap mat khau:");
    
    // thiet lap mk mac dinh neu EPROM trong
    if (eeprom_is_empty()) {
        password_update("1234");
        uart_send_string("Password initialized to: 1234\r\n");
    }
    
    while (1) {
        if (uart_available()) {
            char c = uart_receive_char();
            uart_send_char(c);  
            
            if (c == '\r' || c == '\n') {
                uart_send_string("\r\n[DEBUG] Enter pressed, processing...\r\n");
                xu_ly_du_lieu();
            } 
            else if (buf_index < sizeof(buffer) - 1) {
                buffer[buf_index++] = c;
                
                if (entering_new_pass) {
                    // hien thi ky tu khi nhap mk moi
                    lcd_set_cursor(1, 0);
                    for (int i = 0; i < buf_index; i++) {
                        lcd_write_char(buffer[i]);
                    }
                    for (int i = buf_index; i < 16; i++) {
                        lcd_write_char(' ');
                    }
                } 
                else if (waiting_yesno) {
                    // Hien thi yes/no
                    lcd_set_cursor(1, 0);
                    for (int i = 0; i < buf_index; i++) {
                        lcd_write_char(buffer[i]);
                    }
                    for (int i = buf_index; i < 16; i++) {
                        lcd_write_char(' ');
                    }
                } 
                else {
                    // kiem tra neu nhap lenh "update"
                    if (strncmp(buffer, "update", buf_index) == 0) {
                        // Hien thi chuoi ro rang khi nhap "update"
                        lcd_set_cursor(1, 0);
                        for (int i = 0; i < buf_index; i++) {
                            lcd_write_char(buffer[i]);
                        }
                        for (int i = buf_index; i < 16; i++) {
                            lcd_write_char(' ');
                        }
                    } else {
                        // Hien thi dau * khi nhap mk thuong
                        display_password_hidden(buf_index);
                    }
                }
            }
        }
    }
}