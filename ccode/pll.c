// file    : pll.c
// author  : rb
// purpose : STM32G4xx RCC module routines 
// date    : 260902
// last    : 260902
//

#include "includes.h"

// setup PLL & system clock
void init_pll (void)
{
  // enable PWR peripheral clock to access voltage scaling
  RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
  
  // set voltage scaling to Range 1 for 100 MHz operation (= default POR value)
  PWR->CR1 &= ~PWR_CR1_VOS;                      // clear VOS bits 
  PWR->CR1 |= PWR_CR1_VOS_0;                     // set Range 1 

  // & wait voltage scaling finished
  while ((PWR->SR2 & PWR_SR2_VOSF) != 0U)    
    ;
	  
  // set FLASH latency before increasing clock 
  FLASH->ACR &= ~FLASH_ACR_LATENCY;              // clear LATENCY bits
  FLASH->ACR |= FLASH_ACR_LATENCY_4WS;           // set 4 wait states

  // & wait latency set
  while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_4WS)
    ;

  // enable caches 
  FLASH->ACR |= FLASH_ACR_PRFTEN                 // enable prefetch cache
             |  FLASH_ACR_ICEN                   // enable instruction cache
			 |  FLASH_ACR_DCEN;                  // enable data cache

  // enable HSI
  RCC->CR |= RCC_CR_HSION;

  // & wait HSI ready 
  while ((RCC->CR & RCC_CR_HSIRDY) == 0U)    
    ;

  // disable PLL before setup
  RCC->CR &= ~RCC_CR_PLLON;
  
  // & wait PLL stopped
  while ((RCC->CR & RCC_CR_PLLRDY) != 0U)    
    ;
  
  // setup PLL & select clock source 
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI          // PLL source = HSI
               | (PLLM << RCC_PLLCFGR_PLLM_Pos)  // set divider
               | (PLLN << RCC_PLLCFGR_PLLN_Pos)  // set multiplier
               | (PLLR << RCC_PLLCFGR_PLLR_Pos)  // set divider
               | RCC_PLLCFGR_PLLREN;             // enable PLLR output
  
  // set AHB/APB prescalers to /1 (= 0b0000/0b000)
  RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
  
  // enable PLL
  RCC->CR |= RCC_CR_PLLON;
  
  // & wait PLL locked
  while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
    ;

  // select PLL as system clock
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // & wait switch ready
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ; 

  // setup MCO clock source & prescaler -- dev only
  RCC->CFGR &= ~(RCC_CFGR_MCOSEL | RCC_CFGR_MCOPRE);  // clear bits
  RCC->CFGR |= (0b0001 << RCC_CFGR_MCOSEL_Pos)        // select SYSCLK
            |  RCC_CFGR_MCOPRE_DIV4;                  // set divider (/4) 
}

// dump PLL constants
void pll_dump (void)
{
  printf ("clock settings:\n");
  printf ("PLLM                    : %ld\n", (uint32_t)PLLM);
  printf ("PLLN                    : %ld\n", (uint32_t)PLLN);
  printf ("HSI                     : %ld\n", (uint32_t)HSICLK);
  printf ("VCOCLK                  : %ld\n", (uint32_t)VCOCLK);
  printf ("PLLCLK                  : %ld\n", (uint32_t)PLLCLK);
  printf ("SYSCLK                  : %ld\n", (uint32_t)SYSCLK);
//printf ("HCLK                    : %ld\n", (uint32_t)HCLK);
  printf ("PCLK1 (APB1 Peripheral) : %ld\n", (uint32_t)(APB1CLK));
  printf ("PCLK1 (APB1 Timer)      : %ld\n", (uint32_t)(APB1CLK*2));
  printf ("PCLK2 (APB2 Peripheral) : %ld\n", (uint32_t)(APB2CLK));
  printf ("PCLK2 (APB2 Timer)      : %ld\n", (uint32_t)(APB2CLK));
}

