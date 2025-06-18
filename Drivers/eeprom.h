// FILE: eeprom.h 
#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdint.h>

#define EEPROM_SIZE 64
#define MAX_PASSWORD_LEN 6

void eeprom_init(void);

void eeprom_write_byte(int addr, uint8_t data);
uint8_t eeprom_read_byte(int addr);
int eeprom_is_empty(void);
void eeprom_write_password(const char *pass);
void eeprom_read_password(char *pass);

#endif