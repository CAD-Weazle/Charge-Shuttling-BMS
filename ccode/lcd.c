// file    : lcd.c
// author  : rb
// purpose : LCD graphics routines
// date    : 240325
// last    : 261015
//
// note    : ubuntumono_10x16 16pt font generated with 'fontedit.exe'
//           ubuntumono_24x42 36pt font generated with 'fontedit.exe'
//

#include "includes.h"

// setup LCD display
void init_lcd (void)
{
  // wipe screen
  lcd_clrscr (LCD_COL_DARKBG);
}

// clear screen, fill with color
void lcd_clrscr (uint16_t col)  
{
  // set start column & width
  lcd_wr_cmd (LCD_CMD_SETCOL);
  lcd_wr_dat16 (0);
  lcd_wr_dat16 (LCD_HEIGHT-1);

  // set start row & height
  lcd_wr_cmd (LCD_CMD_SETROW);
  lcd_wr_dat16 (0);
  lcd_wr_dat16 (LCD_WIDTH-1);

  // write to SRAM
  lcd_wr_cmd (LCD_CMD_WRITERAM);

  // fill SRAM
  for (int i = 0; i < LCD_WIDTH; i++)  
    for (int j = 0; j < LCD_HEIGHT; j++)  
      lcd_wr_col (col);
}

/*
// read LCD image memory
void lcd_prtscr (void)
{
  // stop logging data to serial port
  adc_log_off ();

  // send warning to user & wait 
  printf ("#dumping LCD image data to serial port in 10 seconds, switch Teraterm to binary mode\n");

  // give user time to switch Teraterm to binary log mode
  for (int i = 0; i < 10; i++)
    msleep (1000);

  // dump TGA image header (first set Teraterm to binary mode)
  msleep (500);
  tga_dump (NULL, LCD_ROTWIDTH, LCD_ROTHEIGHT); // image too big to fit in memory, so NULL pointer
  msleep (500);

  // select part of LCD image to read
  lcd_setX (0, LCD_ROTWIDTH);  // 320
  lcd_setY (0, LCD_ROTHEIGHT); // 240
  
  // deassert DATA/CMD_L line
  ili9341_cmd ();

  // chip select 
  ili9341_select ();

  // send read RAM command
  spi4_write_byte (ILI9341_CMD_RAMRD);

  // assert DATA/CMD_L line
  ili9341_dat ();

  usleep (10);

  // dummy read
  spi4_read_byte ();

  // read LCD image RAM & write to Teraterm
  // note: image too big to store in STM32 SRAM, so data is read directly from LCD GRAM
  for (int i = 0; i < LCD_ROTWIDTH * LCD_ROTHEIGHT; i++) 
  {
    uint8_t B = spi4_read_byte ();
    uint8_t G = spi4_read_byte ();
    uint8_t R = spi4_read_byte ();

    // swap colors from BGR (LCD image) to RGB (TGA image)
    uart1_putc_raw (R);
    uart1_putc_raw (G);
    uart1_putc_raw (B);
  }

  // chip deselect
  ili9341_deselect ();

  // watit some more
  for (int i = 0; i < 10; i++)
    msleep (1000);

  // continue logging data to serial port
  adc_log_on ();
}
*/

void lcd_setX (uint16_t x0, uint16_t x1)
{
  lcd_wr_cmd (LCD_CMD_SETCOL);
  lcd_wr_dat16 (x0);
  lcd_wr_dat16 (x1);
}

void lcd_setY (uint16_t y0, uint16_t y1)
{
  lcd_wr_cmd (LCD_CMD_SETROW);
  lcd_wr_dat16 (y0);
  lcd_wr_dat16 (y1);
}
  
//void lcd_setXY (uint16_t x0, uint16_t y0)
//{
//  lcd_setX (x0, x0);
//  lcd_setY (y0, y0);
//  lcd_wr_cmd (LCD_CMD_WRITERAM);
//}

// draw single pixel
void lcd_drawpixel (uint16_t x0, uint16_t y0, uint16_t color)
{
  lcd_setX (x0, x0);
  lcd_setY (y0, y0);
  lcd_wr_cmd (LCD_CMD_WRITERAM);
  
  lcd_wr_dat16 (color);
}

// draw line - Bresenham via AI
void lcd_drawline (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  int dx = x1 - x0;
  int dy = y1 - y0;
  int dx2 = dx * 2;
  int dy2 = dy * 2;
  int x = x0;
  int y = y0;

  if (abs(dy) < abs(dx)) 
  {
    int d = dy2 - dx;

    while (x < x1) 
    {
      lcd_drawpixel (x++, y, color);

      if (d < 0) 
      {
        d += dy2;
      } 
      else 
      {
        d += dy2 - dx2;
        y++;
      }
    }
  } 
  else 
  {
    int d = dx2 - dy;

    while (y < y1) 
    {
      lcd_drawpixel (x, y++, color);

      if (d < 0) 
      {
        d += dx2;
      } 
      else 
      {
        d += dx2 - dy2;
        x++;
      }
    }
  }
}

// draw color filled rectangle 
void lcd_drawrect (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col)  
{
  // set start column & width
  lcd_wr_cmd (LCD_CMD_SETCOL);
  lcd_wr_dat16 (x);
  lcd_wr_dat16 (x+w-1);

  // set start row & height
  lcd_wr_cmd (LCD_CMD_SETROW);
  lcd_wr_dat16 (y);
  lcd_wr_dat16 (y+h-1);

  // write to SRAM
  lcd_wr_cmd (LCD_CMD_WRITERAM);

  // fill SRAM
  for (int i = 0; i < w*h; i++)  
  {
    lcd_wr_col (col);
  }
}

// color test - dev only
// RRRR|RGGG|GGGB|BBBB
#define DELTA 2
#define R_MIN 0b00010
#define G_MIN 0b00001

void lcd_blocks (void)
{
  static int R = R_MIN;
  static int G = G_MIN;
//static int B = 0b00010;
//static int B = 0b00100;
  static int B = 0b01000;
  static int x_ofs = 0;
  static int y_ofs = 0;

  int col = (R << 11) | (G << 5) | (B << 0);

//if ((x_ofs > 320) && (y_ofs > 240))
  if (y_ofs > 240)
    return;

  lcd_drawrect (0+x_ofs, 0+y_ofs, DELTA, DELTA, col);
//lcd_drawpixel(0+x_ofs, 0+y_ofs, col);

  printf ("%d %d: R: 0x%02x G: 0x%02x B: 0x%02x col: 0x%04x\n", x_ofs, y_ofs, R, G, B, col);

  R += 1;

  if (R > 0b11111)
  {
    R = R_MIN;

    G += 1;

    if (G > 0b111111)
    {
      G = G_MIN;

      B += 0;
     
      if (B > 0b11111)
      {
        B = 0;
      }
    }
  }

  x_ofs += DELTA;

  if (x_ofs >= 320)
  {
    y_ofs += DELTA;
    x_ofs = 0;
  }

 
}

// *** aliases for 'ili9341.c' routines ***
// write command to LCD display - 
void lcd_wr_cmd (uint8_t cmd)
{
  ili9341_wr_cmd (cmd);
}

// write 8-bit data to LCD display
void lcd_wr_dat (uint8_t dat)
{
  ili9341_wr_dat (dat);
}

// write 16-bit data to LCD display
void lcd_wr_dat16 (uint16_t dat)
{
  ili9341_wr_dat16 (dat);
}

// write 16-bit color pixel to GRAM
void lcd_wr_col (uint16_t col)
{
  ili9341_wr_dat16 (col);
}
// *** aliases for 'ili9341.c' routines ***




