// FILE: eeprom.c 
#include "eeprom.h"
#include "delay.h"
#include "stm32f10x.h"
#include <stdint.h>

#define SDA_PIN     6
#define SCL_PIN     7
#define EEPROM_ADDR 0xA0

// Khoi tao I2C GPIO
static void i2c_gpio_init(void) {
    // Bat clock cho GPIOB
    RCC->APB2ENR |= (1 << 3);
    
    // Cau hinh PB6(SDA) va PB7(SCL) la output open-drain
    GPIOB->CRL &= ~((0xF << (SDA_PIN * 4)) | (0xF << (SCL_PIN * 4)));
    GPIOB->CRL |=  ((0x7 << (SDA_PIN * 4)) | (0x7 << (SCL_PIN * 4)));
    
    GPIOB->BSRR = (1 << SDA_PIN) | (1 << SCL_PIN);
}

static void SDA_OUT(void) {
    GPIOB->CRL &= ~(0xF << (SDA_PIN * 4));
    GPIOB->CRL |=  (0x7 << (SDA_PIN * 4)); // Open-drain output
}

static void SDA_IN(void) {
    GPIOB->CRL &= ~(0xF << (SDA_PIN * 4));
    GPIOB->CRL |=  (0x4 << (SDA_PIN * 4)); // Floating input
}

static void SDA_HIGH(void) { GPIOB->BSRR = (1 << SDA_PIN); }
static void SDA_LOW(void)  { GPIOB->BRR  = (1 << SDA_PIN); }
static void SCL_HIGH(void) { GPIOB->BSRR = (1 << SCL_PIN); }
static void SCL_LOW(void)  { GPIOB->BRR  = (1 << SCL_PIN); }
static int  SDA_READ(void) { return (GPIOB->IDR & (1 << SDA_PIN)) ? 1 : 0; }

static void i2c_delay(void) { delay_us(5); }

static void i2c_start(void) {
    SDA_OUT(); 
    SDA_HIGH(); 
    SCL_HIGH(); 
    i2c_delay();
    SDA_LOW(); 
    i2c_delay(); 
    SCL_LOW();
}

static void i2c_stop(void) {
    SDA_OUT(); 
    SDA_LOW(); 
    SCL_HIGH(); 
    i2c_delay();
    SDA_HIGH(); 
    i2c_delay();
}

static void i2c_write_bit(uint8_t bit) {
    SDA_OUT();
    if (bit) SDA_HIGH(); else SDA_LOW();
    i2c_delay(); 
    SCL_HIGH(); 
    i2c_delay(); 
    SCL_LOW();
}

static uint8_t i2c_read_bit(void) {
    uint8_t bit; 
    SDA_IN(); 
    i2c_delay();
    SCL_HIGH(); 
    i2c_delay(); 
    bit = SDA_READ(); 
    SCL_LOW();
    return bit;
}

static uint8_t i2c_write_byte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        i2c_write_bit((byte & 0x80) != 0);
        byte <<= 1;
    }
    return !i2c_read_bit(); // ACK = 0
}

static uint8_t i2c_read_byte(uint8_t ack) {
    uint8_t byte = 0; 
    SDA_IN();
    for (int i = 0; i < 8; i++) {
        byte <<= 1;
        if (i2c_read_bit()) byte |= 1;
    }
    i2c_write_bit(!ack);
    return byte;
}

// Khoi tao EEPROM
void eeprom_init(void) {
    i2c_gpio_init();
}

void eeprom_write_byte(int addr, uint8_t data) {
    i2c_start();
    if (!i2c_write_byte(EEPROM_ADDR)) {
        i2c_stop();
        return; 
    }
    if (!i2c_write_byte((uint8_t)addr)) {
        i2c_stop();
        return;
    }
    i2c_write_byte(data);
    i2c_stop();
    delay_ms(5);
}

uint8_t eeprom_read_byte(int addr) {
    uint8_t data;
    
    // Write phase
    i2c_start();
    if (!i2c_write_byte(EEPROM_ADDR)) {
        i2c_stop();
        return 0xFF;
    }
    if (!i2c_write_byte((uint8_t)addr)) {
        i2c_stop();
        return 0xFF;
    }
    
    // Read phase
    i2c_start(); // Repeated start
    if (!i2c_write_byte(EEPROM_ADDR | 1)) {
        i2c_stop();
        return 0xFF;
    }
    data = i2c_read_byte(0); // NACK
    i2c_stop();
    return data;
}

int eeprom_is_empty(void) {
    for (int i = 0; i < MAX_PASSWORD_LEN + 1; i++) {
        if (eeprom_read_byte(i) != 0xFF) return 0;
    }
    return 1;
}

void eeprom_write_password(const char *pass) {
    int i = 0;
    for (; i < MAX_PASSWORD_LEN && pass[i] != '\0'; i++) {
        eeprom_write_byte(i, pass[i]);
    }
    eeprom_write_byte(i, '\0');
}

void eeprom_read_password(char *pass) {
    for (int i = 0; i < MAX_PASSWORD_LEN + 1; i++) {
        pass[i] = eeprom_read_byte(i);
        if (pass[i] == '\0') break;
    }
    pass[MAX_PASSWORD_LEN] = '\0'; 
}