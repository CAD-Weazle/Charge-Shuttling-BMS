// file    : printl.c
// author  : rb
// purpose : print formatted text to LCD
//           (only 'lcd_puts' implemented, no 'printl' yet)
// date    : 240330
// last    : 260629
//
// note    : ubuntumono_10x16 16pt font generated with 'fontedit.exe'
//           ubuntumono_24x42 36pt font generated with 'fontedit.exe'
//
// note    : define character data as 'const' to store it in FLASH

#define TEXT_ORIENTATION_NORMAL             // comment out for inverted orientation

#include "includes.h"
//#include "./fonts/fixedsys_8x14.h"
//#include "./fonts/ubuntumono_8x14.h"
//#include "./fonts/ubuntumono_16x28.h"
//#include "./fonts/fixedsys_16pt.h"
#include "./fonts/ubuntumono_10x16.h"       // ubuntu 16pt
#include "./fonts/ubuntumono_18x31.h"       // ubuntu 26pt
//#include "./fonts/ubuntumono_24x42.h"     // ubuntu 36pt

// define LCD fonts
font_t normal_text;                         // normal small text
font_t large_text;                          // normal large text
font_t invert_text;                         // fill percentage text, normal mode
font_t alarm_text;                          // fill percentage text, alarm mode

// setup LCD fonts
void init_font (void)
{
  // setup 14pt font - normal small text
  font_setup (&normal_text, UBUNTUMONO10x16_WIDTH, UBUNTUMONO10x16_HEIGHT, ubuntu_mono_14pt, GUI_PANEL_COLFG, GUI_PANEL_COLBG);

  // setup 14pt font - fill percentage text normal
  font_setup (&invert_text, UBUNTUMONO10x16_WIDTH, UBUNTUMONO10x16_HEIGHT, ubuntu_mono_14pt, GUI_CELL_BG, GUI_CELL_BORDER);

  // setup 14pt font - fill percentage text alarm
  font_setup (&alarm_text, UBUNTUMONO10x16_WIDTH, UBUNTUMONO10x16_HEIGHT, ubuntu_mono_14pt, GUI_CELL_BG, LCD_COL_RED);

  // setup 26pt font - normal large text
  font_setup (&large_text, UBUNTUMONO18x31_WIDTH, UBUNTUMONO18x31_HEIGHT, ubuntu_mono_26pt, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);

  // dump font settings - dev only
 //font_dump (&normal_text);
 //font_dump (&invert_text);
 //font_dump (&alarm_text);
 //font_dump (&large_text);
}

// initialize font struct
void font_setup (font_t *font, uint16_t f_width, uint16_t f_height, const unsigned char *f_data, uint16_t fg, uint16_t bg)
{
  uint16_t num_bytes = f_height * (f_width/8 + ((f_width%8) ? 1 : 0));

  // font definition
  font->w = f_width;    // width  [px]
  font->h = f_height;   // height [px]
  font->b = num_bytes;  // bytes per character
  font->d = f_data;     // pointer to font definition

  // color definition
  font->fg = fg;        // character foreground color
  font->bg = bg;        // character background color
}

// change font foreground & background color
void font_setcolor (font_t *font, uint16_t fg, uint16_t bg)
{
  font->fg = fg;        // character foreground color
  font->bg = bg;        // character background color
}

// draw '\0' terminated string
//void lcd_puts (uint16_t xpos, uint16_t ypos, const char *str, const font_t *font)
//{
//  char ch;
//  uint8_t cnt = 0;
//
//  while ((ch = str[cnt++]) != '\0')
//    lcd_putc (xpos + (cnt * font->w), ypos, ch, font);
//}  

//void lcd_puts (uint16_t xpos, uint16_t ypos, const char *str, const font_t *font)
//{
//  uint16_t i = 0;
//
//  while (str[i] != '\0')
//  {
//    lcd_putc (xpos + (i * font->w), ypos, str[i], font);
//    i++;
//  }
//}

void lcd_puts(uint16_t xpos, uint16_t ypos, const char *str, const font_t *font)
{
  uint16_t i = 0;
  
  while (str[i] != '\0')
  {
    lcd_putc(xpos + (i * font->w), ypos, str[i], font);
    i++;
  }
}

// draw ASCII character
void lcd_putc (uint16_t xpos, uint16_t ypos, char ch, const font_t *font)  
{
  uint32_t dat;
  
  // set start column & character width
  lcd_wr_cmd   (LCD_CMD_SETCOL);
  lcd_wr_dat16 (xpos);
  lcd_wr_dat16 (xpos + (font->w - 1));

  // set start row & character height
  lcd_wr_cmd   (LCD_CMD_SETROW);
  lcd_wr_dat16 (ypos);
  lcd_wr_dat16 (ypos + (font->h - 1));

  // write to GRAM
  lcd_wr_cmd (LCD_CMD_WRITERAM);

  // number of bytes of a single character row
  int row_bytes = font->w/8 + ((font->w%8) ? 1 : 0);

  // number of bytes of a single character
  int num_bytes = font->h * row_bytes;

  // calculate character location in font array
  int idx = (ch - ' ') * num_bytes;  // note: first character in map is ' '

  // draw all character pixels
#ifdef TEXT_ORIENTATION_NORMAL
  for (int i = 0; i < num_bytes; i += row_bytes)     // normal LCD text orientation
#else
  for (int i = num_bytes; i >= 0 ; i -= row_bytes)   // inverse LCD text orientation
#endif
  {
    // get row of character pixels
    if (row_bytes == 3)
    { 
      // 3 bytes per character row
      dat = (((uint32_t)font->d[(idx+2) + i] << 16) |
             ((uint32_t)font->d[(idx+1) + i] << 8)  |
                        font->d[idx + i]);
    }
    else
    {
      // 2 bytes per character row
      dat = (((uint32_t)font->d[(idx+1) + i] << 8)  | 
                        font->d[idx + i]);
    }

    // draw row of character pixels
    for (int bit = 0; bit < font->w; bit++)
    {
      if (dat & (1 << bit))
        lcd_wr_col (font->fg);  // character pixel in foreground color
      else                   
        lcd_wr_col (font->bg);  // & rest in background color
    }
  }
}

// dump Font struct data - dev only
void font_dump (font_t *font)
{
  printf ("font width    : %d\n",     font->w);
  printf ("font height   : %d\n",     font->h);
  printf ("bytes per char: %d\n",     font->b);
  printf ("font data addr: 0x%p\n",   font->d);
  printf ("bf color      : 0x%04x\n", font->bg);
  printf ("ff color      : 0x%04x\n", font->fg);
  printf ("--\n");
}

// dump character set - dev only
void lcd_dump_font (void)
{
  int line_len    = 16;      // numbers of characters per line
  int font_width  = 10;      // 10x16px font
  int font_height = 16;      // 10x16px font
  int num_bytes   = font_height * (font_width / 8 + ((font_width % 8) ? 1 : 0));
  int num_char    = sizeof(ubuntu_mono_14pt)/num_bytes;

  printf ("num_bytes: %d num_char: %d\n", num_bytes, num_char);

  for (int ch = 0; ch < num_char; ch++)
  {
    // set start column & character width
    lcd_wr_cmd (LCD_CMD_SETCOL);
    lcd_wr_dat16 (5 + font_width*(ch%line_len));
    lcd_wr_dat16 (5 + font_width*(ch%line_len + 1));
    
    // set start row & character height
    lcd_wr_cmd (LCD_CMD_SETROW);
    lcd_wr_dat16 (5 + font_height*(ch/line_len));
    lcd_wr_dat16 (5 + font_height*(ch/line_len + 1));
    
    // write to SRAM
    lcd_wr_cmd (LCD_CMD_WRITERAM);

    // dump all characters
    for (int i = 0; i < num_bytes; i += 2)
    {
      // one scan line is 2 bytes
      uint8_t lo = ubuntu_mono_14pt[ch*num_bytes + i];
      uint8_t hi = ubuntu_mono_14pt[ch*num_bytes + i + 1];
      uint16_t dat = ((uint16_t)hi << 8) | lo;
    
      printf ("0x%04x ", dat);

      // only display used pixels
      for (int bit = 0; bit <= font_width; bit++)
      {
        if (dat & (1 << bit))
        {
          printf ("*");
          lcd_wr_col (LCD_COL_DARKGOLDENROD);     // pixel in foreground color
        }
        else
        {
          printf (".");
          lcd_wr_col (LCD_COL_DARKBG);            // pixel in background color
        }
      }

      printf ("\n");
    }

    printf ("\n");
  }
}
