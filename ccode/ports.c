// file    : ports.c
// author  : ao/rb
// purpose : IO port routines - STM32F4xx
// date    : 170810
// last    : 261017
//
// note    : make sure GPIO is in mode GPIO_SPEED_FREQ_HIGH for fast signals
//           (IO in slow mode will block fast signals!1!)

#include "includes.h"

struct port_info
{
  uint16_t mode : 2;
  uint16_t speed: 2;
  uint16_t od   : 1;
  uint16_t pupd : 2;
  uint16_t afr  : 4;
};

static const struct port_info porta[16] = 
{
  [ 0] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VCELLS4        - ADC12[1] - clamp voltage Cell4
  [ 1] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VCELLS3        - ADC12[2] - clamp voltage Cell3
  [ 2] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VCELLS2        - ADC12[3] - clamp voltage Cell4      
  [ 3] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VCELLS1        - ADC12[4] - clamp voltage Cell5 
  [ 4] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 5] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 6] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 7] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 8] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF0_MCO},    // MCO            - dev only
  [ 9] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF7_USART1}, // SER_TXD        - USART1
  [10] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF7_USART1}, // SER_RXD        - USART1  
  [11] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [12] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [13] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF0_SWJ},    // SWIO
  [14] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF0_SWJ},    // SWCLK 
  [15] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
};

static const struct port_info portb[16] = 
{
   [0] = {GPIO_Mode_AN,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VBOOST         - ADC1[15]
   [1] = {GPIO_Mode_AN,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VBUCK          - ADC1[12]
   [2] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
   [3] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF5_SPI1},   // OLED_SCK  
   [4] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CD3  
   [5] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CD2  
   [6] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CD1  
   [7] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CU4  
   [8] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CU3  
   [9] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CU2  
  [10] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // DISCHARGE_ENA  - output FET control
  [11] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // I_PACK         - ADC12[14]
  [12] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // V_CHARGE       - ADC1[11]
  [13] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [14] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // LED_RED
  [15] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // LED_GRN
};

static const struct port_info portc[16] = 
{
  [ 0] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF2_TIM1},   // HI_PWM         - Timer 1 compl. half-bridge PWM
  [ 1] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // I_HI           - ADC12[7]
  [ 2] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // I_LO           - ADC12[8]
  [ 3] = {GPIO_Mode_AN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // VCELLS         - ADC12[9]
  [ 4] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 5] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [ 6] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF8_I2C4},       // TOUCH_SCL      - I2C4
  [ 7] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_AF8_I2C4},       // TOUCH_SDA      - I2C4
  [ 8] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_UP},                      // TOUCH_IRQ_L    - tousch screen IRQ line
  [ 9] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [10] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_UP},                      // ACTION         - ACTION push button switch
  [11] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [12] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [13] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF4_TIM1},   // LO_PWM         - Timer 1 compl. half-bridge PWM
  [14] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // NU
  [15] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // HSIGN          - sign current sense DC/DC
};

static const struct port_info porte[16] = 
{
  [ 7] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // OLED_BL        - LCD backlight
  [ 8] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_UP},                      // OLED_RESET_L   - LCD reset
  [ 9] = {GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // OLED_CD        - LCD command/data
  [10] = {GPIO_Mode_OUT, GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // OLED_CS_L      - LCD select
  [11] = {GPIO_Mode_IN,  GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_NOPULL},                  // PSIGN          - sign current sense battery pack
  [12] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF5_SPI4},   // OLED_SCK       - LCD SPI SCK
  [13] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF5_SPI4},   // OLED_MISO      - LCD SPI MISO
  [14] = {GPIO_Mode_AF,  GPIO_Speed_100MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_AF5_SPI4},   // OLED_MOSI      - LCD SPI MOSI
  [15] = {GPIO_Mode_OUT, GPIO_Speed_2MHz,   GPIO_OType_PP, GPIO_PuPd_DOWN},                    // CHARGE_ENA     - input FET control
};


static const struct port_info portf[16] = 
{
  [ 0] = {GPIO_Mode_OUT, GPIO_Speed_2MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                    // LED_BUCK
  [ 1] = {GPIO_Mode_OUT, GPIO_Speed_2MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL},                    // LED_BOOST
};

static void init_port (GPIO_TypeDef *GPIOx, const struct port_info *port)
{
    int i;
    uint32_t mode = 0, speed = 0, type = 0, pupd = 0;
    uint64_t afr = 0;

    for (i = 0; i < 16; i++)
    {
      mode  |= (uint32_t) port[i].mode  << i*2;
      speed |= (uint32_t) port[i].speed << i*2;
      pupd  |= (uint32_t) port[i].pupd  << i*2;
      type  |= (uint32_t) port[i].od    << i;
      afr   |= (uint64_t) port[i].afr   << i*4;
    }

  GPIOx->AFR[0]  = afr;           // set AFR registers first
  GPIOx->AFR[1]  = afr >> 32;     // set AFR registers first
  GPIOx->MODER   = mode;
  GPIOx->OSPEEDR = speed;
  GPIOx->PUPDR   = pupd;
  GPIOx->OTYPER  = type;
}

void init_ports (void) 
{
  // enable clocks for GPIOA/B/C <> todo D/E/F
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN
               |  RCC_AHB2ENR_GPIOBEN
               |  RCC_AHB2ENR_GPIOCEN
               |  RCC_AHB2ENR_GPIOEEN
               |  RCC_AHB2ENR_GPIOFEN;

  // setup GPIOs
  init_port (GPIOA, porta);
  init_port (GPIOB, portb);
  init_port (GPIOC, portc);
  init_port (GPIOE, porte);
  init_port (GPIOF, portf);

  // block all pass FETs first, just to be safe!
  cell1_block_down ();
  cell2_block_down ();
  cell3_block_down ();
  
  cell2_block_up ();
  cell3_block_up ();
  cell4_block_up ();

  // all LEDs off
  led_red_off   ();
  led_grn_off   ();
  led_buck_off  ();
  led_boost_off ();

  // output disabled
//output_disable ();
}

// green LED routines
void led_grn_on (void)
{
  gpio_set (GPIOB, LED_GRN);
}

void led_grn_off (void)
{
  gpio_clr (GPIOB, LED_GRN);
}

void led_grn_toggle (void)
{
  GPIOB->ODR ^= (1U << LED_GRN);
}

void led_grn_flash (uint16_t delay)
{
  gpio_set (GPIOB, LED_GRN);
  msleep (delay);
  gpio_clr (GPIOB, LED_GRN);
}

// red LED routines
void led_red_on (void)
{
  gpio_set (GPIOB, LED_RED);
}

void led_red_off (void)
{
  gpio_clr (GPIOB, LED_RED);
}

void led_red_toggle (void)
{
  GPIOB->ODR ^= (1U << LED_RED);
}

void led_red_flash (uint16_t delay)
{
  gpio_set (GPIOB, LED_RED);
  msleep (delay);
  gpio_clr (GPIOB, LED_RED);
}

void led_buck_off (void)
{
  gpio_clr (GPIOF, LED_BUCK);
}

void led_buck_on (void)
{
  gpio_set (GPIOF, LED_BUCK);
}

void led_boost_off (void)
{
  gpio_clr (GPIOF, LED_BOOST);
}

void led_boost_on (void)
{
  gpio_set (GPIOF, LED_BOOST);
}

// battery cell pass FET control - upper cells
void cell_pass_upper (uint32_t cell)
{
  // sanity check, bail on error
  if ((cell <= 0) || (cell >= LIFEPO4_CELL_NUM))
  {
    printf ("#no such upper Cell as cell number %d\n", cell);
    return;
  }

  switch (cell)
  {
    case 1: cell2_pass_up (); break;
    case 2: cell3_pass_up (); break;
    case 3: cell4_pass_up (); break;
    default: break;
  }
}

void cell_block_upper (uint32_t cell)
{
  // sanity check, bail on error
  if ((cell <= 0) || (cell >= LIFEPO4_CELL_NUM))
  {
    printf ("#no such upper Cell as cell number %d\n", cell);
    return;
  }

  switch (cell)
  {
    case 1: cell2_block_up (); break;
    case 2: cell3_block_up (); break;
    case 3: cell4_block_up (); break;
    default: break;
  }
}

// battery cell pass FET control - lower cells
void cell_pass_lower (uint32_t cell)
{
  // sanity check, bail on error
  if ((cell < 0) || (cell >= LIFEPO4_CELL_NUM-1))
  {
    printf ("#no such lower Cell as cell number %d\n", cell);
    return;
  }

  switch (cell)
  {
    case 0: cell1_pass_down (); break;
    case 1: cell2_pass_down (); break;
    case 2: cell3_pass_down (); break;
    default: break;
  }
}

void cell_block_lower (uint32_t cell)
{
  // sanity check, bail on error
  if ((cell < 0) || (cell >= LIFEPO4_CELL_NUM-1))
  {
    printf ("#no such lower Cell as cell number %d\n", cell);
    return;
  }

  switch (cell)
  {
    case 0: cell1_block_down (); break;
    case 1: cell2_block_down (); break;
    case 2: cell3_block_down (); break;
    default: break;
  }
}

// battery cell pass FET control - lower cells
void cell1_pass_down (void)
{
  gpio_set (GPIOB, CD1);
}

void cell1_block_down (void)
{
  gpio_clr (GPIOB, CD1);
}

void cell2_pass_down (void)
{
  gpio_set (GPIOB, CD2);
}

void cell2_block_down (void)
{
  gpio_clr (GPIOB, CD2);
}

void cell3_pass_down (void)
{
  gpio_set (GPIOB, CD3);
}

void cell3_block_down (void)
{
  gpio_clr (GPIOB, CD3);
}

void cell2_pass_up (void)
{
  gpio_set (GPIOB, CU2);
}

void cell2_block_up (void)
{
  gpio_clr (GPIOB, CU2);
}

void cell3_pass_up (void)
{
  gpio_set (GPIOB, CU3);
}

void cell3_block_up (void)
{
  gpio_clr (GPIOB, CU3);
}

void cell4_pass_up (void)
{
  gpio_set (GPIOB, CU4);
}

void cell4_block_up (void)
{
  gpio_clr (GPIOB, CU4);
}

// get DC/DC buck path current sense sign bit
uint32_t ibck_sign (void)
{
  if (GPIOC->IDR & (1 << LSIGN))
    return (0);
  else
	return (1);  
}

// get DC/DC boost path current sense sign bit
uint32_t ibst_sign (void)
{
  // note: polarity reversed for better PCB routing
  if (GPIOB->IDR & (1 << HSIGN))
    return (1);
  else
	return (0);  
}

// get battery pack current sense sign bit
uint32_t ipck_sign (void)
{
  // postive: from charger to battery pack / neagtive: from battery pack to load
  if (GPIOE->IDR & (1 << PSIGN))
    return (1);
  else
	return (0);  
}

// get ACTION push button switch state
uint32_t action_switch (void)
{
  return (GPIOC->IDR & (1U << ACTION)) ? 0 : 1;
}

// debounce ACTION push button
uint32_t action_button (void)
{
  static uint32_t but_cnt = 0;
  static uint32_t but_old = 0;
         uint32_t but_cur;

  // get current ACTION push button state
  but_cur = action_switch();
  
  // debounce
  if (but_cur != but_old) 
    but_cnt = 0;
  else if (++but_cnt >= 3 && but_cur) 
  {
    but_old = 0;
    return 1;
  }
  
  // update state
  if (!but_cur) 
    but_old = 0;
  else 
    but_old = but_cur;
  
  return 0;
}


// enable load output FET
void load_enable (void)
{
  gpio_set (GPIOB, DISCHARGE_ENA);
}

// disable load output FET
void load_disable (void)
{
  gpio_clr (GPIOB, DISCHARGE_ENA);
}

// toggle load output FET
void load_toggle (void)
{
 if (GPIOB->IDR & (1U << DISCHARGE_ENA))
   load_disable ();
 else
   load_enable ();
}

// enable charger input FET
void charger_enable (void)
{
  gpio_set (GPIOE, CHARGE_ENA);
}

// disable charger input FET
void charger_disable (void)
{
  gpio_clr (GPIOE, CHARGE_ENA);
}

// toggle charger input FET
void charger_toggle (void)
{
 if (GPIOE->IDR & (1U << CHARGE_ENA))
   charger_disable ();
 else
   charger_enable ();
}


// LCD backlight on
void lcd_blight_on  (void)
{
  gpio_set (GPIOE, OLED_BL);
}

// LCD backlight off
void lcd_blight_off (void)
{
  gpio_clr (GPIOE, OLED_BL);
}

// toggle charger input FET
void lcd_blight_toggle (void)
{
 if (GPIOE->IDR & (1U << OLED_BL))
   lcd_blight_off ();
 else
   lcd_blight_on ();
}

// apply reset pulse to LCD display
void lcd_reset (void)
{
  gpio_clr (GPIOE, OLED_RESET_L);
  usleep (100);
  gpio_set (GPIOE, OLED_RESET_L);
}
