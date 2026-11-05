// file    : spi.h
// author  : rb
// purpose : header file for spi.c
// date    : 170820
// last    : 260925
//

#ifndef _SPI_H_
#define _SPI_H_

// -- prototypes
void init_spi (void);

void spi4_init (void);

void spi4_write_word (int data);

uint8_t spi4_rw_byte    (uint8_t data);
void    spi4_write_byte (uint8_t data);
uint8_t spi4_read_byte  (void);


#endif


