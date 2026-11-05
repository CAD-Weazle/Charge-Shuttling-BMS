// file    : i2c.h
// author  : rb
// purpose : header file i2c.c
// date    : 181010
// last    : 260925

#ifndef __I2C_H__
#define __I2C_H__

// -- prototypes
void init_i2c  (void);

void i2c4_init (void);

void    i2c4_write_byte (uint8_t adr, uint8_t dat);
uint8_t i2c4_read_byte  (uint8_t adr);
uint8_t i2c4_read_reg8  (uint8_t adr, uint8_t reg);

void i2c4_dump_registers (void);

#endif


