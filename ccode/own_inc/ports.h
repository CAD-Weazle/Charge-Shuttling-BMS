// file    : ports.h
// author  : rb
// purpose : header file for ports.c
// date    : 170810
// last    : 260917
//

#ifndef _PORTS_H_
#define _PORTS_H_

// -- GPIO defines
// GPIO modes
enum
{ 
  GPIO_Mode_IN   = 0x00,   // GPIO Input Mode 
  GPIO_Mode_OUT  = 0x01,   // GPIO Output Mode 
  GPIO_Mode_AF   = 0x02,   // GPIO Alternate function Mode 
  GPIO_Mode_AN   = 0x03,   // GPIO Analog Mode 
};

// GPIO output driver type
enum
{ 
  GPIO_OType_PP = 0x00,
  GPIO_OType_OD = 0x01,
};

// GPIO speed setting
enum
{ 
  GPIO_Speed_2MHz   = 0x00,  // low speed 
  GPIO_Speed_25MHz  = 0x01,  // medium speed 
  GPIO_Speed_50MHz  = 0x02,  // fast speed 
  GPIO_Speed_100MHz = 0x03,  // high speed on 30 pF (80 Mc Output max speed on 15 pF) 
};

// GPIO pull resistor settings
enum
{ 
  GPIO_PuPd_NOPULL = 0x00,   // no internal pull resistors
  GPIO_PuPd_UP     = 0x01,   // internal pull-up
  GPIO_PuPd_DOWN   = 0x02,   // internal pull-down
};

// GPIO output set/clear macros
#define gpio_set(port, pin)       do {port->BSRR = (0x00000001 << pin);} while(0) // set outputs: lo-word BSRR
#define gpio_clr(port, pin)       do {port->BSRR = (0x00010000 << pin);} while(0) // clr outputs: hi-word BSRR

// -- defines 
// port A

// port B
#define HSIGN                                   3  // sign current sense DC/DC - upper
#define CD3                                     4  // pass logic FET cell 3 - down 
#define CD2                                     5  // pass logic FET cell 2 - down 
#define CD1                                     6  // pass logic FET cell 1 - down 
#define CU4                                     7  // pass logic FET cell 4 - up
#define CU3                                     8  // pass logic FET cell 3 - up
#define CU2                                     9  // pass logic FET cell 2 - up
#define DISCHARGE_ENA                          10  // output FET enable/disable
#define LED_RED                                14  // red LED
#define LED_GRN                                15  // green LED

// port C
#define ACTION                                 10  // push button ACTION 
#define LSIGN                                  15  // sign current sense DC/DC - lower

// port E
#define OLED_BL                                 7  // LCD backlight on/off
#define OLED_RESET_L                            8  // LCD reset
#define OLED_CD                                 9  // LCD command/data
#define OLED_CS_L                              10  // LCD select
#define PSIGN                                  11  // sign current sense battery pack
#define CHARGE_ENA                             15  // input FET enable/disable

// port F
#define LED_BUCK                                0  // buck mode LED
#define LED_BOOST                               1  // boost mode LED


// -- prototypes
void init_ports (void) ;

void led_red_on     (void);
void led_red_off    (void);
void led_red_toggle (void);
void led_red_flash  (uint16_t delay);

void led_grn_on     (void);
void led_grn_off    (void);
void led_grn_toggle (void);
void led_grn_flash  (uint16_t delay);

void led_buck_off  (void);
void led_buck_on   (void);
void led_boost_off (void);
void led_boost_on  (void);

void cell_pass_upper  (uint32_t cell);
void cell_block_upper (uint32_t cell);

void cell_pass_lower  (uint32_t cell);
void cell_block_lower (uint32_t cell);

void cell1_pass_down (void);
void cell2_pass_down (void);
void cell3_pass_down (void);

void cell1_block_down (void);
void cell2_block_down (void);
void cell3_block_down (void);

void cell2_pass_up (void);
void cell3_pass_up (void);
void cell4_pass_up (void);

void cell2_block_up (void);
void cell3_block_up (void);
void cell4_block_up (void);

uint32_t ibck_sign (void);
uint32_t ibst_sign (void);
uint32_t ipck_sign (void);

uint32_t action_switch (void);
uint32_t action_button (void);

void load_enable  (void);
void load_disable (void);
void load_toggle  (void);

void charger_enable  (void);
void charger_disable (void);
void charger_toggle  (void);

void fan_on  (void);
void fan_off (void);

void power12v_on  (void);
void power12v_off (void);

void lcd_blight_on     (void);
void lcd_blight_off    (void);
void lcd_blight_toggle (void);

void lcd_reset (void);

#endif



