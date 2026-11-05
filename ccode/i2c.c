// file    : i2c.c
// author  : rb
// purpose : I2C routines for STM32G474
// date    : 190701
// last    : 260925
//

#include "includes.h"

// init I2C 
void init_i2c (void)
{
  // init I2C modules
  i2c4_init ();

  // dev only
//i2c4_dump_registers ();
}

// init I2C4 module
void i2c4_init (void)
{
  // enable I2C4 clock
  RCC->APB1ENR2 |= RCC_APB1ENR2_I2C4EN;

  // reset I2C4 block
  RCC->APB1RSTR2 |=  RCC_APB1RSTR2_I2C4RST;
  RCC->APB1RSTR2 &= ~RCC_APB1RSTR2_I2C4RST;

  // disable I2C4 before changing settings
  I2C4->CR1 = 0;

  // setup bus timing - 100kHz clock
  I2C4->TIMINGR = (0x0b << 28)              // PRESC
                | (0x04 << 20)              // SCLDEL
                | (0x02 << 16)              // SDADEL
                | (0x25 << 8)               // SCLH
                |  0x2b;                    // SCLL

  // enable I2C4 again, analog filter on, no interrupts, no DMA
  I2C4->CR1 |= I2C_CR1_PE;
}

// I2C4 write byte
void i2c4_write_byte (uint8_t adr, uint8_t dat)
{
  // setup Tx mode: 7-bit, automatic end, no reload 
  I2C4->CR2 =  I2C_CR2_AUTOEND              // automatic end mode
            | (1 << I2C_CR2_NBYTES_Pos)     // number of bytes
          //|  I2C_CR2_RD_WRN               // 0: write / 1: read
            |  ((uint32_t)adr<<1);          // slave address

  // generate START condition
  I2C4->CR2 |= I2C_CR2_START;               // set this bit separately

  // wait TXDR empty
  while ((I2C4->ISR & I2C_ISR_TXIS) == 0)
  {
    // bail on NACK
    if ((I2C4->ISR & I2C_ISR_NACKF) != 0)
    {
      printf ("ERROR: NACK found, bail Tx\n");
      return;
    }
  }
    
  // write TXDR
  I2C4->TXDR = (uint32_t)dat; 

  // wait for STOP condition detected
  while (!(I2C4->ISR & I2C_ISR_STOPF))
    ;

  // clear STOP condition flag
  I2C4->ICR = I2C_ICR_STOPCF;
}

// I2C4 read byte
uint8_t i2c4_read_byte (uint8_t adr)
{
  // setup Rx mode: 7-bit, automatic end, no reload 
  I2C4->CR2 =  I2C_CR2_AUTOEND              // automatic end mode
            | (1 << I2C_CR2_NBYTES_Pos)     // number of bytes
            |  I2C_CR2_RD_WRN               // 0: write / 1: read
            |  ((uint32_t)adr<<1);          // slave address

  // generate START condition
  I2C4->CR2 |= I2C_CR2_START;               // set this bit separately

  // wait RXDR not empty
  while ((I2C4->ISR & I2C_ISR_RXNE) == 0)
    ;
 
  // read TXDR
  return (I2C4->RXDR);
}

// 12C3 read 8-bit register
uint8_t i2c4_read_reg8 (uint8_t adr, uint8_t reg)
{
  uint8_t res;
  
  // write register address
  i2c4_write_byte (adr, reg);

  // & read back register data
  res = i2c4_read_byte (adr);

  return (res);
}

// dump I2C1 registers
void i2c4_dump_registers (void)
{
  printf("I2C4_CR1     : 0x%08lx\n", I2C4->CR1);
  printf("I2C4_CR2     : 0x%08lx\n", I2C4->CR2);
  printf("I2C4_TIMINGR : 0x%08lx\n", I2C4->TIMINGR);
  printf("I2C4_TIMEOUTR: 0x%08lx\n", I2C4->TIMEOUTR);
  printf("I2C4_ISR     : 0x%08lx\n", I2C4->ISR);
  printf("I2C4_ICR     : 0x%08lx\n", I2C4->ICR);
  printf("I2C4_PECR    : 0x%08lx\n", I2C4->PECR);
  printf("I2C4_RXDR    : 0x%08lx\n", I2C4->RXDR);
  printf("I2C4_TXDR    : 0x%08lx\n", I2C4->TXDR);
  printf("I2C4_OAR1    : 0x%08lx\n", I2C4->OAR1);
  printf("I2C4_OAR2    : 0x%08lx\n", I2C4->OAR2);
  printf("\n");
}



