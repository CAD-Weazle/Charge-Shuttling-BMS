// file    : ticker.c
// author  : rb
// purpose : ticker & PWM Timers
// date    : 170906
// last    : 261017

#include "includes.h"

// globals
volatile uint8_t csec_elapsed;
volatile uint8_t dsec_elapsed;
volatile uint8_t sec_elapsed;

// ARM core SysTick ISR
void SysTick_Handler (void)
{
  static int csec_cnt, dsec_cnt, sec_cnt;
  
  if (++csec_cnt >= 10U)
  {
    csec_cnt = 0U;
    csec_elapsed = 1U;
  
    if (++dsec_cnt >= 10U)
    {
      dsec_cnt = 0U;
      dsec_elapsed = 1U;
     
      if (++sec_cnt >= 10U)
      {
        sec_cnt = 0U;
        sec_elapsed = 1U;
      }
    }
  } 
}

// Timer 1 Update Event ISR - synchronize ADC sampling with PWM signal
void TIM1_UP_TIM16_IRQHandler (void) 
{
  static int dly = 0U;

  // check for Update interrupt flag
  if (TIM1->SR & TIM_SR_UIF) 
  { 
    // clear flag
    TIM1->SR &= ~TIM_SR_UIF; 

    // PWM frequency is 100kHz, slow down to 1Ksps for ADC1
    if (++dly >= 100U)
    {
    //led_red_toggle (); // should be toggling with 500Hz - dev only

      // start Timer 2 to trigger ADC1 after some delay
      TIM2->CR1 |= TIM_CR1_CEN;
      dly = 0U;
    }
  }
}

// Timer 2 Update Event ISR for timing ADC1 sample timing - for checking ADC timing
// note: this ISR is only used to drive the green LED so the ADC/DMA timing can be checked with the scope
void TIM2_IRQHandler (void)
{
  if (TIM2->SR & TIM_SR_UIF)
  {
    // clear flag
    TIM2->SR &= ~TIM_SR_UIF;   
  
    // signal start of ADC1 conversion, turned oof in ADC DMA ISR - dev only, for measuring timing DMA 
  //led_grn_on (); 
  }
}

// init Timers
void init_ticker (void) 
{
  // enable Timer 1 clock          - Timer clock = APB2CLK = 100Mc 
  RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

  // enable Timer 2, Timer 5 clock - Timer clock = APB1CLK = 100Mc
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN
                |  RCC_APB1ENR1_TIM5EN;

  // init 16-bit Timer 1, generates complementary PWM signal for half bridge
  TIM1->PSC    = TIMER_100MHZ;                   // timer_clock
  TIM1->EGR    = TIM_EGR_UG;                     // force update
  TIM1->ARR    = PWM_100KHZ;                     // PWM frequency
  TIM1->CCR1   = (DCDC_PWMRESET*PWM_100KHZ)/1000;  // set duty cycle to safe reset value
  TIM1->DIER  |= TIM_DIER_UIE;                   // enable Update Event interrupt - for ADC1/PWM synchronization
  TIM1->CCMR1 |= TIM_CCMR1_OC1M_2                // select PWM mode 1 on OC1 (OC1M = 0b110)
              |  TIM_CCMR1_OC1M_1                
              |  TIM_CCMR1_OC1PE;                // enable preload register on OC1, channel is output
  TIM1->CCER  |= TIM_CCER_CC1E                   // select active high polarity on OC1 & enable output OC1
              |  TIM_CCER_CC1NE;                 // select active high polarity on OC1N & enable output OC1N

  TIM1->BDTR  = 0;                               // disable all enabled OCx/OCxN outputs

  TIM1->CR1    = TIM_CR1_CEN;                    // enable timer
            //|  TIM_CR1_CMS;                    // <> todo: select center aligned mode

//NVIC_EnableIRQ (TIM1_BRK_UP_TRG_COM_IRQn);     // enable Break, Update, Trigger & Commutation Interrupt Timer 1

  NVIC_EnableIRQ   (TIM1_UP_TIM16_IRQn);         // enable Timer 1 interrupt
  NVIC_SetPriority (TIM1_UP_TIM16_IRQn, 1);      // set priority Timer 1 interrupt

  // init 32-bit Timer 2 - for synchronization of ADC1 sampling during quite period of DC/DC PWM 
  TIM2->PSC  = TIMER_50MHZ;                      // set Timer 2 clock frequency
  TIM2->ARR  = 10;                               // set ADC1 tigger delay
  TIM2->CR1 |= TIM_CR1_OPM;                      // enable one-pulse mode
  TIM2->CR2 |= TIM_CR2_MMS_1;                    // select the Update Event as TRGO (MMS: 0b0010)
  TIM2->EGR |= TIM_EGR_UG;                       // update registers
//TIM2->CR1 |= TIM_CR1_CEN;                      // do not enable, Timer 2 is started in Timer 1 ISR

  TIM2->DIER |= TIM_DIER_UIE;                    // enable Update Event interrupt - <> for dev only
  NVIC_EnableIRQ (TIM2_IRQn);

  // init 32-bit Timer 5 to free running
  TIM5->PSC  = TIMER_1MHZ;                       // set Timer 5 clock frequency
  TIM5->EGR  = TIM_EGR_UG;                       // force update
  TIM5->CR1  = TIM_CR1_CEN;                      // enable Timer 5

  // init System Timer (from 'core_cm4.h') -- 1.000 ms tick after tweaking
  SysTick_Config (SYSCLK / 1000U);

  // set interrupt priority
  NVIC_SetPriority (SysTick_IRQn, 1); 
}

// kill time with 1us resolution
void usleep (int32_t usecs)
{
  uint32_t start = TIM5->CNT + usecs;
  
  while ((int32_t) (TIM5->CNT - start) < 0)
    continue;
}

// kill time with approx. ms resolution
void msleep (uint32_t msecs)
{
  while (msecs--)
    usleep (1000);
}

// dump Timer 1 register state
void timer1_dump_registers (void)
{
  printf ("-- Timer 1 registers --\n");
  printf ("RCC->APB1ENR1: %08lx\n", RCC->APB1ENR1);
  printf ("RCC->APB2ENR : %08lx\n", RCC->APB2ENR);
  printf ("TIM1->PSC    : %08lx\n", TIM1->PSC);
  printf ("TIM1->EGR    : %08lx\n", TIM1->EGR);
  printf ("TIM1->ARR    : %08lx\n", TIM1->ARR);
  printf ("TIM1->CCR1   : %08lx\n", TIM1->CCR1);
  printf ("TIM1->CCR2   : %08lx\n", TIM1->CCR2);
  printf ("TIM1->CCR3   : %08lx\n", TIM1->CCR3);
  printf ("TIM1->DIER   : %08lx\n", TIM1->DIER);
  printf ("TIM1->CR1    : %08lx\n", TIM1->CR1);
  printf ("TIM1->CR2    : %08lx\n", TIM1->CR2);  
  printf ("TIM1->SMCR   : %08lx\n", TIM1->SMCR);  
  printf ("TIM1->CCMR1  : %08lx\n", TIM1->CCMR1);  
  printf ("TIM1->CCMR2  : %08lx\n", TIM1->CCMR2);
  printf ("TIM1->CCER   : %08lx\n", TIM1->CCER);
  printf ("TIM1->BDTR   : %08lx\n", TIM1->BDTR);
}

// dump Timer 2 register state
void timer2_dump_registers (void)
{
  printf ("-- Timer 2 registers --\n");
  printf ("RCC->APB1ENR1: %08lx\n", RCC->APB1ENR1);
  printf ("RCC->APB2ENR : %08lx\n", RCC->APB2ENR);
  printf ("TIM2->PSC    : %08lx\n", TIM2->PSC);
  printf ("TIM2->EGR    : %08lx\n", TIM2->EGR);
  printf ("TIM2->ARR    : %08lx\n", TIM2->ARR);
  printf ("TIM2->CCR1   : %08lx\n", TIM2->CCR1);
  printf ("TIM2->CCR2   : %08lx\n", TIM2->CCR2);
  printf ("TIM2->CCR3   : %08lx\n", TIM2->CCR3);
  printf ("TIM2->DIER   : %08lx\n", TIM2->DIER);
  printf ("TIM2->CR1    : %08lx\n", TIM2->CR1);
  printf ("TIM2->CR2    : %08lx\n", TIM2->CR2);  
  printf ("TIM2->SMCR   : %08lx\n", TIM2->SMCR);  
  printf ("TIM2->CCMR1  : %08lx\n", TIM2->CCMR1);  
  printf ("TIM2->CCMR2  : %08lx\n", TIM2->CCMR2);
  printf ("TIM2->CCER   : %08lx\n", TIM2->CCER);
  printf ("TIM2->BDTR   : %08lx\n", TIM2->BDTR);
}


// Timer 1 CCR former value 
int ccr_old = DCDC_PWMMIN;

// disable DC/DC complementary PWM signal
void pwm_disable (void)
{
  // disable all enabled OCx/OCxN outputs, force to safe state (all low)
  TIM1->BDTR = 0;                    
}

// set duty cycle compl. h-bridge PWM signals - range 'duty_cycle' 0..100%: 0..1000
void pwm_set (uint32_t duty_cycle)
{
  int ccr_new = duty_cycle;
  int ccr_step;

  // check for lower bound allowed PWM duty cycle
  if (ccr_new < DCDC_PWMMIN)
    ccr_new = DCDC_PWMMIN;

  // check for upper bound allowed PWM duty cycle
  if (ccr_new > DCDC_PWMMAX)
    ccr_new = DCDC_PWMMAX;

  // nothing to do, bail
  if (abs(ccr_new - ccr_old) == 0)
    return;

  // duty cycle increase or decrease
  if (ccr_new > ccr_old)
    ccr_step = 1;
  else
    ccr_step = -1;

  // adjust Timer register
  while (abs(ccr_old - ccr_new) > 0)
  {
    // adjust duty cycle
    ccr_old += ccr_step;

    // update Timer register
    TIM1->CCR1 = ((ccr_old*PWM_100KHZ) / (100*10));
    
    // delay added for smooth transitions & keep FETs from being damaged - critical!!!
    usleep (1000);
  }

  // enable PWM 
  if (TIM1->BDTR == 0)  // check if PWM not enabled
  {
    TIM1->BDTR  |= TIM_BDTR_MOE                    // enable all enabled OCx/OCxN outputs
                |  20;                             // set deadtime (~200ns, no shoot-thru)
  }
}
