// file    : ili9341.h
// author  : rb
// purpose : ILI9341 WaveShare LCD driver / ADAFRUIT 2.8" TFT
// date    : 240323
// last    : 260926
//

// TODO:
//#define ILI9341_CASET  0x2A
//#define ILI9341_PASET  0x2B
//#define ILI9341_RAMRD  0x2E
//
//#define LCD_WIDTH   240
//#define LCD_HEIGHT  320
//
//void ili9341_read_memory(uint8_t *buf)
//{
// uint32_t pixels = LCD_WIDTH * LCD_HEIGHT;
//
// /* Select entire display */
// ili9341_write_command(ILI9341_CASET);
// ili9341_write_data(0x00);
// ili9341_write_data(0x00);
// ili9341_write_data((LCD_WIDTH - 1) >> 8);
// ili9341_write_data((LCD_WIDTH - 1) & 0xFF);
//
// ili9341_write_command(ILI9341_PASET);
// ili9341_write_data(0x00);
// ili9341_write_data(0x00);
// ili9341_write_data((LCD_HEIGHT - 1) >> 8);
// ili9341_write_data((LCD_HEIGHT - 1) & 0xFF);
//
// /* Start memory read */
// ili9341_write_command(ILI9341_RAMRD);
//
// /*
//  * First byte returned by RAMRD is dummy.
//  */
// /* First byte is dummy */
//  (void)spi4_rw_byte(0x00);
//
//
//  /* Read RGB data continuously */
//  for (uint32_t i = 0; i < count; i++)
//      buf[i] = spi4_rw_byte(0x00);// bon't toggle CS_L here, read in one burst
// 
//uint8_t r = spi4_rw_byte(0x00);
//uint8_t g = spi4_rw_byte(0x00);
//uint8_t b = spi4_rw_byte(0x00);
//
//uint16_t rgb565 =
//      ((r & 0xF8) << 8)
//    | ((g & 0xFC) << 3)
//    | ((b & 0xF8) >> 3);

#include "includes.h"

// set up LCD display
// note: from DollaTek documentation, works with 2.0", 2.4" en 2.8" LCD 
// note: 251122: also works with Adafruit 2.8" TFT 
void init_ili9341 (void)
{  
  // apply hardware reset
  ili9341_hwreset ();    
  
  // ADA ONLY ??? NOT FOUND IN DATASHEET ???
  //ili9341_wr_cmd (0xef);
  //ili9341_wr_dat (0x03);
  //ili9341_wr_dat (0x80);
  //ili9341_wr_dat (0x02);

  // power control B (0xcf) page 196
  ili9341_wr_cmd (ILI9341_CMD_POWERB);
  ili9341_wr_dat (0x00);  
  ili9341_wr_dat (0xc9);  // 0xc1 
  ili9341_wr_dat (0X30); 

  // power on sequence control (0xed) page 200  
  ili9341_wr_cmd (ILI9341_CMD_POWER_SEQ);   
  ili9341_wr_dat (0x64); 
  ili9341_wr_dat (0x03); 
  ili9341_wr_dat (0X12); 
  ili9341_wr_dat (0X81); 

  // driver timing control A (0xe8) page 197  
  ili9341_wr_cmd (ILI9341_CMD_DTCA);        
  ili9341_wr_dat (0x85); 
  ili9341_wr_dat (0x10); 
  ili9341_wr_dat (0x7a); // 0x78: ADAFRUTICAKE DOC

  // power control A (CBh) page 195
  ili9341_wr_cmd (ILI9341_CMD_POWERA);  
  ili9341_wr_dat (0x39); 
  ili9341_wr_dat (0x2c); 
  ili9341_wr_dat (0x00); 
  ili9341_wr_dat (0x34); 
  ili9341_wr_dat (0x02); 

  // pump ratio control (F7h) page 202
  ili9341_wr_cmd (ILI9341_CMD_PRC);  
  ili9341_wr_dat (0x20); 
  
  // driver timing control B (EAh) page 199
  ili9341_wr_cmd (ILI9341_CMD_DTCB);  
  ili9341_wr_dat (0x00); 
  ili9341_wr_dat (0x00); 
 
  // Power control 1 (0xc0)
  ili9341_wr_cmd (ILI9341_CMD_POWER1);       
  ili9341_wr_dat (0x1b);     // VRH[5:0] // ADA: 0x23
  
  // Power Control 2 (0xc1) page 179
  ili9341_wr_cmd (ILI9341_CMD_POWER2);       
  ili9341_wr_dat (0x00);     // SAP[2:0];BT[3:0] 01 // ADA: 0x10
  
  // VCOM Control 1 (0xc5) page 180
  ili9341_wr_cmd (ILI9341_CMD_VCOM1);        
  ili9341_wr_dat (0x30); 	 // 0x3f ADA: 0x3e
  ili9341_wr_dat (0x30); 	 // 0x3c ADA: 0x28
  
  // VCOM Control 2 (0xc7) page 182
  ili9341_wr_cmd (ILI9341_CMD_VCOM2);        
  ili9341_wr_dat (0xb7);     // ADA: 0x86

  // ADA only // read Display MADCTL (0x0b) page 95
  //writecommand(ILI9341_CMD_RDDMADCTL);    	
  //writedata(0x48);
  
  // Memory Access Control 
  ili9341_wr_cmd (ILI9341_CMD_MAC);          
  ili9341_wr_dat (0x08); 

  // COLMOD: Pixel Format Set (0x3a) page 134  
  ili9341_wr_cmd (ILI9341_CMD_PIXEL_FORMAT);   
  ili9341_wr_dat (0x55); 

  // Frame Rate Control (In Normal Mode/Full Colors) (0xb1) page 155
  ili9341_wr_cmd (ILI9341_CMD_FRMCTR1);   
  ili9341_wr_dat (0x00);   
  ili9341_wr_dat (0x1a); // ADA: 0x18

  // Display Function Control (0xb6) page 164
  ili9341_wr_cmd (ILI9341_CMD_DFC);          
  ili9341_wr_dat (0x0a); 
  ili9341_wr_dat (0xa2); 
  // ADA
  //ili9341_wr_dat (0x08); // do not use
  //ili9341_wr_dat (0x82); // do not use
  //ili9341_wr_dat (0x27); // do not use 
  //ADA 
  
  // disable 3Gamma (0xf2h) page 201
  ili9341_wr_cmd (ILI9341_CMD_3GAMMA_EN);    
  ili9341_wr_dat (0x00); 
  
  // Gamma Set (0x26) page 107
  ili9341_wr_cmd (ILI9341_CMD_GAMMA);        
  ili9341_wr_dat (0x01); 
  
  // Positive Gamma Correction (0xe0) page 188
  ili9341_wr_cmd (ILI9341_CMD_PGAMMA);       
  ili9341_wr_dat (0x0f); // ADA: 0x0F 
  ili9341_wr_dat (0x2a); // ADA: 0x31 
  ili9341_wr_dat (0x28); // ADA: 0x2B 
  ili9341_wr_dat (0x08); // ADA: 0x0C 
  ili9341_wr_dat (0x0e); // ADA: 0x0E 
  ili9341_wr_dat (0x08); // ADA: 0x08 
  ili9341_wr_dat (0x54); // ADA: 0x4E 
  ili9341_wr_dat (0xa9); // ADA: 0xF1 
  ili9341_wr_dat (0x43); // ADA: 0x37 
  ili9341_wr_dat (0x0a); // ADA: 0x07 
  ili9341_wr_dat (0x0f); // ADA: 0x10 
  ili9341_wr_dat (0x00); // ADA: 0x03 
  ili9341_wr_dat (0x00); // ADA: 0x0E 
  ili9341_wr_dat (0x00); // ADA: 0x09 
  ili9341_wr_dat (0x00); // ADA: 0x00 		 
  
  // Negative Gamma Correction (0xe1) page 189
  ili9341_wr_cmd (ILI9341_CMD_NGAMMA);       
  ili9341_wr_dat (0x00); // ADA: 0x00
  ili9341_wr_dat (0x15); // ADA: 0x0E
  ili9341_wr_dat (0x17); // ADA: 0x14
  ili9341_wr_dat (0x07); // ADA: 0x03
  ili9341_wr_dat (0x11); // ADA: 0x11
  ili9341_wr_dat (0x06); // ADA: 0x07
  ili9341_wr_dat (0x2b); // ADA: 0x31
  ili9341_wr_dat (0x56); // ADA: 0xC1
  ili9341_wr_dat (0x3c); // ADA: 0x48
  ili9341_wr_dat (0x05); // ADA: 0x08
  ili9341_wr_dat (0x10); // ADA: 0x0F
  ili9341_wr_dat (0x0f); // ADA: 0x0C
  ili9341_wr_dat (0x3f); // ADA: 0x31
  ili9341_wr_dat (0x3f); // ADA: 0x36
  ili9341_wr_dat (0x0f); // ADA: 0x0F

  ili9341_wr_cmd (ILI9341_CMD_PAGE_ADDR); 
  ili9341_wr_dat (0x00);
  ili9341_wr_dat (0x00);
  ili9341_wr_dat (0x01);
  ili9341_wr_dat (0x3f);
  
  ili9341_wr_cmd (ILI9341_CMD_COLUMN_ADDR); 
  ili9341_wr_dat (0x00);
  ili9341_wr_dat (0x00);
  ili9341_wr_dat (0x00);
  ili9341_wr_dat (0xef);	 
  
  // exit Sleep Out (0x11) page 101
  ili9341_wr_cmd (ILI9341_CMD_SLEEP_OUT); 
  msleep (120);
  
  // display on		
  ili9341_wr_cmd (ILI9341_CMD_DISPLAY_ON); 
  
  // set display direction
  ili9341_wr_cmd (ILI9341_CMD_MAC);
  ili9341_wr_dat (0b01101000); // origin: landscape top left
  
  // turn on backlight
  ili9341_backlight_on ();
}

// write command to LCD display
void ili9341_wr_cmd (uint8_t dat)
{
  // deassert DATA/CMD_L line
  ili9341_cmd ();

  // write command byte
  ili9341_select ();
  spi4_write_byte (dat);
  ili9341_deselect ();

  // relax a bit
  usleep (1);
}

// write byte to LCD display
void ili9341_wr_dat (uint8_t dat)
{
  // assert DATA/CMD_L line
  ili9341_dat ();

  // write data
  ili9341_select ();
  spi4_write_byte (dat);
  ili9341_deselect ();
  
  // relax a bit
  usleep (1);
}

// write word to LCD display
void ili9341_wr_dat16 (uint16_t dat)
{
  uint8_t hi = (uint8_t)(dat>>8);
  uint8_t lo = (uint8_t)(dat&0x00ff);

  // assert DATA/CMD_L line
  ili9341_dat ();

  // write data
  ili9341_select ();
  spi4_write_byte (hi);
  spi4_write_byte (lo);
  ili9341_deselect ();

  // relax a bit
  usleep (1);
}

// generate hardware reset
void ili9341_hwreset (void)
{
  gpio_clr (GPIOE, OLED_RESET_L);
  msleep (100);
  gpio_set (GPIOE, OLED_RESET_L);
  msleep (100);
}

// generate software reset
void ili9341_swreset (void)
{
  ili9341_wr_cmd (ILI9341_CMD_SWRESET);
  msleep (5);      // relax a bit as per DS
}

// LCD control lines for communication
void ili9341_select (void)
{
  gpio_clr (GPIOE, OLED_CS_L);
}

void ili9341_deselect (void)
{
  gpio_set (GPIOE, OLED_CS_L);
}

void ili9341_cmd (void)
{
  gpio_clr (GPIOE, OLED_CD);
}

void ili9341_dat (void)
{
  gpio_set (GPIOE, OLED_CD);
}

// backlight on
void ili9341_backlight_on (void)
{
  gpio_set (GPIOE, OLED_BL);
}

// backlight off
void ili9341_backlight_off (void)
{
  gpio_clr (GPIOE, OLED_BL);
}

// backlight toggle
void ili9341_backlight_toggle (void)
{
 if (GPIOE->IDR & (1U << OLED_BL))
   ili9341_backlight_off ();
 else
   ili9341_backlight_on ();
}

// set display brightness - no effect
void ili9341_brightness (uint8_t dat) 
{
  ili9341_wr_cmd (ILI9341_CMD_WDB);
  ili9341_wr_dat (dat);
}
