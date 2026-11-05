// file    : uart1.h
// author  : rb
// purpose : header file for uart1.c
// date    : 170813
// last    : 261017
//

// defines
// -- defines 
#define RX1_SIZE                            (128U) // receive buffer size  [character]
#define TX1_SIZE                            (128U) // transmit buffer size [character]

// -- prototypes
void init_uart1 (uint32_t baud);

void uart1_putc (int c);
void uart1_puts (const char *s);
int  uart1_getc (void);

