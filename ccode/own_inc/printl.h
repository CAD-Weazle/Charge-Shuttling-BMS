// file    : printl.h
// author  : rb
// purpose : header file for printl.c
// date    : 240330
// last    : 240330
//

#ifndef _PRINTL_H_
#define _PRINTL_H_

// -- font definitions 
typedef struct
{
  uint16_t w;    // font width [pixels]
  uint16_t h;    // font height [pixels]
  uint16_t b;    // bytes per character
  const unsigned char *d;    // font data
  uint16_t fg;   // foreground color
  uint16_t bg;   // background color
} font_t;

// -- globals
extern font_t normal_text; 
extern font_t invert_text; 
extern font_t alarm_text; 
extern font_t large_text; 

// -- defines

// -- prototypes
void init_font (void);

void font_setup (font_t *font, uint16_t f_width, uint16_t f_height, const unsigned char *f_data, uint16_t fg, uint16_t bg);

void font_setcolor (font_t *font, uint16_t fg, uint16_t bg);

void font_dump (font_t *font);

void lcd_puts (uint16_t xpos, uint16_t ypos, const char *str, const font_t *font);
void lcd_putc (uint16_t xpos, uint16_t ypos, const char ch, const font_t *font);

void lcd_dump_font (void);


#endif

