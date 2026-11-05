// file    : spi.c
// author  : rb
// purpose : SPI port routines for STM32G474
// date    : 170810
// last    : 260923
//
// note    : on STM32G4, the SPI FIFO uses the CPU access width for data packing
//           so 32-bit DR write can result in multiple 8-bit frames being transmitted,
//           use '*(__IO uint8_t*)' cast to force an 8-bit write


#include "includes.h"

// set up SPI
void init_spi (void)
{
  spi4_init ();  // LCD display BMSTEST
}

// init SPI4
void spi4_init(void)
{
  // enable SPI4 clock
  RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
  
  // make sure SPI4 is disabled before configuration
  SPI4->CR1 &= ~SPI_CR1_SPE;
  
  // setup SPI4
  SPI4->CR2 = (0b0111 << SPI_CR2_DS_Pos)    // 8-bits data transfer
            |  SPI_CR2_FRXTH;

  // setup SPI4
  SPI4->CR1 =  SPI_CR1_SSM                  // slave select management (0: via SNN pin  / 1: software slave management)
            |  SPI_CR1_SSI                  // internal slave select   (0: SPI is slave / 1: SPI is master) 
            | (0b001 << SPI_CR1_BR_Pos)     // baud rate = PCLK/8  (= 25.00Mc) - seems to work
          //| (0b010 << SPI_CR1_BR_Pos)     // baud rate = PCLK/8  (= 12.50Mc) - seems to work
          //| (0b011 << SPI_CR1_BR_Pos)     // baud rate = PCLK/16 (=  6.25Mc) - working
            |  SPI_CR1_MSTR                 // SPI master
            | (0b0 << SPI_CR1_CPOL_Pos)     // SCK low when idle 
            | (0b0 << SPI_CR1_CPHA_Pos);    // capture on first clock edge 

  // enable SPI4
  SPI4->CR1 |= SPI_CR1_SPE;
}

// SPI4 8-bit read/write  
uint8_t spi4_rw_byte (uint8_t data)
{
  // wait until Tx buffer empty
  while (!(SPI4->SR & SPI_SR_TXE))
      continue;

  // data to send, cast DR address to 8-bit target 
  *(__IO uint8_t*)(&SPI4->DR) = data;

  // wait until Rx buffer filled
  while (!(SPI4->SR & SPI_SR_RXNE))
    continue;

  // return received data
  return ((uint8_t)SPI4->DR);
}

// SPI4 8-bit write
void spi4_write_byte (uint8_t data)
{
  spi4_rw_byte (data);
}

// SPI4 8-bit read
uint8_t spi4_read_byte (void)
{
  return spi4_rw_byte (0x00);
}

