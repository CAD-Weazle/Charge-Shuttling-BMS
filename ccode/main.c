// file    : main.c
// author  : rb
// purpose : Buck-Boost Converter Charge Shuttling Active Battery Management System
// board   : BMSTEST hv1.0 - STM32G474MCT6 (Cortex-M4, 170MHz, 128KB RAM, 256KB FLASH, LQFP80)
// date    : 260514
// last    : 261023
//

#include "includes.h"

int main (void)  
{
  // init all the things!1!!
  init_ports    ();          // setup GPIOs
  init_pll      ();          // setup clocks & PLL
  init_adc      ();          // setup ADC & DMA
  init_ticker   ();          // start Timers
  init_uart1    (115200);    // setup UART1 - debug port
  init_i2c      ();          // setup I2C
  init_spi      ();          // setup SPI
  init_ili9341  ();          // setup ILI9341 WaveShare LCD/Adafruit display
  init_lcd      ();          // setup LCD display
  init_cst026   ();          // setup CST026 Capacitive Touch Controller
  init_font     ();          // setup fonts - call before 'init_gui ()'
  init_gui      ();          // setup GUI
  init_bms      ();          // setup BMS

  // dump version
  version ();

  while (1)
  {
    // poll commandline
    poll_cli ();                                 
  
    // read & filter ADC data 
    adc_update ();
  
    // handle touch screen
    cst026_update ();

    if (dsec_elapsed)
    {
      // run the BMS
      bms_update ();

      // run the BMS test
      bms_cycle_test ();

      // scan LCD GUI buttons
      gui_update_buttons ();

      dsec_elapsed = 0;
    }

    if (sec_elapsed)
    {
      // life beat
      led_red_flash (50);

      // dump ADC data to serial port
      adc_dump ();	  

      // render LCD GUI with current BMS state
      gui_update (&bms);

      sec_elapsed = 0;
    }
  }
}
