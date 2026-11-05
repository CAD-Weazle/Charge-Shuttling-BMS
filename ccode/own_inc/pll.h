// file    : pll.h
// author  : rb
// purpose : header file for pll.c
// date    : 171120
// last    : 260902
//

#ifndef _PLL_H_
#define _PLL_H_

#define Mc                             (1000000UL) // 1 Mc

// -- PLL defines
// VCOCLK must be between 64 and 344Mc
// PLLCLK must not exceed 170Mc
// HSICLK is 16Mc
// VCOCLK = HSICLK * (PLLN/PLLM) = 16 * (50/4) = 200Mc
// PLLCLK = VCOCLK / 2 = 100Mc
#define PLLM                                  (3U) // encoded clock divider: /4
#define PLLM_DIV                              (4U) // actuall PLLM divider
#define PLLN                                 (50U) // clock multiplier
#define PLLR                                  (0U) // encoded clock divider: /2
#define PLLR_DIV                              (2U) // actual PLLR divider

#define MCO_DIV                               (4U) // MCO divider from PLLCLK

#define HSICLK                             (16*Mc) // HSI clock = 16 Mc
#define MSICLK                         (1024000UL) // MSI clock = 1.024 Mc

#define VCOCLK            ((HSICLK*PLLN)/PLLM_DIV) // VCO clock = 200 Mc ((16*50)/4)
#define PLLCLK                   (VCOCLK/PLLR_DIV) // PLL clock = 100 Mc (200/2)
#define SYSCLK                            (PLLCLK) // system clock = 100 Mc
#define MCOCLK                  (PLLCLK / MCO_DIV) // MCO output = 25 Mc (100/4)

#define APB1DIV                              (1UL) // APB1 clock divider
#define APB1CLK                   (SYSCLK/APB1DIV) // APB1 clock
                                                   
#define APB2DIV                              (1UL) // APB2 clock divider
#define APB2CLK                   (SYSCLK/APB2DIV) // APB2 clock


// -- prototypes
void init_pll (void);

void pll_dump (void);

#endif



