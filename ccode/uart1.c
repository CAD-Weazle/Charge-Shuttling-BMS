// file    : uart1.c
// author  : ao/rb
// purpose : UART1 routines
// date    : 170811
// last    : 261017
//

#include "includes.h"

// input/output FIFOs 
static uint8_t uart1_rx_buf[RX1_SIZE];
static uint8_t uart1_tx_buf[TX1_SIZE];

// FIFO indexes 
volatile static uint32_t tx_tail = 0U;
volatile static uint32_t rx_head = 0U;
volatile static uint32_t tx_head = 0U;
volatile static uint32_t rx_tail = 0U;

// USART1 ISR
void USART1_IRQHandler (void)
{
  uint32_t isr = USART1->ISR;

  // read character
  if ((isr & USART_ISR_RXNE_RXFNE) != 0U)
  {
    uint8_t c = (uint8_t)USART1->RDR;

    if ((uint32_t)(rx_head - rx_tail) < (uint32_t)RX1_SIZE)
      uart1_rx_buf[rx_head++ % RX1_SIZE] = (uint8_t)c;
  }

  // write character
  if ((isr & USART_ISR_TXE_TXFNF) != 0U)
  {
    if (tx_head == tx_tail)
      USART1->CR1 &= ~USART_CR1_TXEIE;
    else
      USART1->TDR = (uint8_t)uart1_tx_buf[tx_tail++ % TX1_SIZE];
  }
}

// init USART1
void init_uart1 (uint32_t baud)
{
  // sanity check, fall back to default baud rate
  if (baud == 0U)
    baud = 9600U;

  // enable USART1 clock
  RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

  USART1->CR1 = 0U;                              // disable USART during configuration
  USART1->CR3 = 0U;                              // disable flow control and DMA
  USART1->CR2 = 0U;                              // select 1 stop bit

  USART1->BRR = ((APB2CLK << 4) / baud) >> 4;    // set baud rate for oversampling by 16, use fixed point
//USART1->BRR = (APB2CLK + (baud / 2U)) / baud;  // set baud rate for oversampling by 16, nearest-integer rounding 
	
  // enable RX/TX and RX interrupt, then enable USART
  USART1->CR1 = USART_CR1_TE             
              | USART_CR1_RE             
              | USART_CR1_RXNEIE_RXFNEIE 
              | USART_CR1_UE;

  // enable USART1 interrupt in NVIC
  NVIC_EnableIRQ (USART1_IRQn);
}

// write character to USART1
void uart1_putc (int c)
{
  if (c == '\n')
    uart1_putc ('\r');

  while ((uint32_t)(tx_head - tx_tail) >= (uint32_t)TX1_SIZE)
    ;

  uart1_tx_buf[tx_head++ % TX1_SIZE] = (char)c;

  USART1->CR1 |= USART_CR1_TXEIE;
}

// write string to USART1
void uart1_puts (const char *s)
{
  while (*s)
    uart1_putc (*s++);
}

// get character from USART1
int uart1_getc (void)
{
  if (rx_head == rx_tail)
    return (-1);

  return (uint8_t)uart1_rx_buf[rx_tail++ % RX1_SIZE];
}

