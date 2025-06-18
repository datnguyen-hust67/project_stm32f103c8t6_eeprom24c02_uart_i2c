// FILE: password.c
#include "password.h"
#include "eeprom.h"
#include "uart.h"
#include <string.h>

int password_check(const char *input) {
    char saved[MAX_PASSWORD_LEN + 1] = {0};
    eeprom_read_password(saved);

    uart_send_string("\r\nInput: ");
    uart_send_string(input);
    uart_send_string("\r\nSaved: ");
    uart_send_string(saved);
    uart_send_string("\r\n");

    return strncmp(saved, input, MAX_PASSWORD_LEN) == 0;
}

void password_update(const char *new_password) {
    eeprom_write_password(new_password);
}