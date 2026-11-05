// file    : cst026.c
// author  : rb
// purpose : I2C routines for CST026 Capacitive Touch Panel Controller
// date    : 260717
// last    : 260930
//
// note    : written for STM32G474

#include "includes.h"

// stuct holding touch screen state
touch_t ts;

// handle CTS026 touch event
void EXTI9_5_IRQHandler(void)
{
  // check EXTI8 interrupt (PC8 = TOUCH_IRQ_L)
  if (EXTI->PR1 & EXTI_PR1_PIF8)
  {
    // clear interrupt flag
    EXTI->PR1 = EXTI_PR1_PIF8;
    
    // set touch event flag
    ts.new_touch = 1;
    
    // signal touch event - dev only
  //led_grn_toggle();
  }
}


// setup CST026 Touch Controller
void init_cst026 (void)
{
  // enable SYSCFG clock
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  
  // select PC8 as source for EXTI8
  SYSCFG->EXTICR[2] &= ~(0xfU << 0);            // clear EXTI8 field
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI8_PC; // select PC8 (= TOUCH_IRQ_L)
  
  // configure EXTI8 interrupt
  EXTI->IMR1 |=  EXTI_IMR1_IM8;                 // enable interrupt on EXTI8
  EXTI->EMR1 &= ~EXTI_EMR1_EM8;                 // disable event on EXTI8
  
  // falling-edge trigger for TOUCH_IRQ_L
  EXTI->RTSR1 &= ~EXTI_RTSR1_RT8;               // disable rising edge
  EXTI->FTSR1 |=  EXTI_FTSR1_FT8;               // enable falling edge
  
  // clear any pending EXTI8 interrupt
  EXTI->PR1 = EXTI_PR1_PIF8;
  
  // enable EXTI lines 5..9 interrupt in NVIC
  NVIC_ClearPendingIRQ (EXTI9_5_IRQn);
  NVIC_SetPriority     (EXTI9_5_IRQn, 2);
  NVIC_EnableIRQ       (EXTI9_5_IRQn);
}

// get current touch panel state - polling mode 
void cst026_update (void)
{
  // no touch detected, bail
  if (!ts.new_touch)
    return;

  // clear flag - acknowledge touch event 
  ts.new_touch = 0;

  // get coordinate data touch point
  uint8_t x_hi = i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_TOUCH1_XH);
  uint8_t x_lo = i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_TOUCH1_XL);
  uint8_t y_hi = i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_TOUCH1_YH);
  uint8_t y_lo = i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_TOUCH1_YL);

  // calculate coordinates
  uint16_t x = ((uint16_t)(x_hi & 0x0f) << 8) | x_lo;
  uint16_t y = ((uint16_t)(y_hi & 0x0f) << 8) | y_lo;

  // remap to actual display layout: horizontal orientation, origin top left & x and y swapped
  ts.x_pos = (LCD_ROTWIDTH-1) - y;
  ts.y_pos = x;

  // get touch screen pressed/released status flags from hi-nibble x-coordinate
  ts.pressed = (x_hi & CST026_MASK_PRESSED) != 0;
  ts.release = (x_hi & CST026_MASK_RELEASE) != 0;

  // scan all buttons for touch event
  gui_button_scan (&ts);

  // dump current touch data - dev only
//cst026_dump ();
}

// dump touch state - dev only
void cst026_dump (void)
{
  // print touch data
  printf ("P: (%d, %d) ", ts.x_pos, ts.y_pos);
  printf ("- pressed: %d release: %d", ts.pressed, ts.release);
  printf ("\n");
}

// dump CST026 ID & firmware version - dev only
void cst026_dump_registers (void)
{
  // most registers appear unimplemented and always read 0x00
  printf ("lib version    : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_LIB_VERSION));
  printf ("chip ID/cipher : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_CIPHER));
  printf ("interrupt mode : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_GMODE));
  printf ("firmware ID    : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_FIRMID));
  printf ("vendor ID      : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_VENDORID));
  printf ("FT5335 state   : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_STATE));
  printf ("threshold      : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_TH_GROUP));
  printf ("rate active    : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_PERIODACTIVE)); 
  printf ("rate monitor   : 0x%02x\n", i2c4_read_reg8 (CST026_I2C_ADR, CST026_REG_PERIODMONITOR));
}








