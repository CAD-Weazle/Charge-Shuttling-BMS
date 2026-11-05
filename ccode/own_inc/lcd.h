// file    : lcd.h
// author  : rb
// purpose : header file for lcd.c
// date    : 210904
// last    : 261017
//
// note    : list of RGB565 colors:  https://rgbcolorpicker.com/565/table
//note     : list of ironbow colors: https://stackoverflow.com/questions/28495390/thermal-imaging-palette#30066099

#ifndef _LCD_H_
#define _LCD_H_

// -- defines
#define LCD_WIDTH                           (240U) // Waveshare 2.0"/2.4" / DollaTek 2.8" LCD width
#define LCD_HEIGHT                          (320U) // Waveshare 2.0"/2.4" / DollaTek 2.8" LCD height
#define LCD_ROTWIDTH                  (LCD_HEIGHT) // screen 90 degrees rotated
#define LCD_ROTHEIGHT                  (LCD_WIDTH) // screen 90 degrees rotated

// -- 'ili9341.h' mapping
#define LCD_CMD_DISPLAYOFF  (ILI9341_CMD_DISPLAY_OFF)  // display off register    (0x28)
#define LCD_CMD_DISPLAYON    (ILI9341_CMD_DISPLAY_ON)  // display on register     (0x29)  
#define LCD_CMD_SETCOL      (ILI9341_CMD_COLUMN_ADDR)  // colomn address register (0x2a)   
#define LCD_CMD_SETROW        (ILI9341_CMD_PAGE_ADDR)  // page address register   (0x2b)  
#define LCD_CMD_WRITERAM           (ILI9341_CMD_GRAM)  // write to Frame Memory   (0x2c)  
#define LCD_CMD_READRAM           (ILI9341_CMD_RAMRD)  // read fromFrame Memory   (0x2e)  

// -- colors
#define LCD_COL_MIXTEST        (0b00000 << 11) | (0b000101 << 6) | (0b00001 << 0)  // RGB565

// note: invert bits for ST7789 driver
#define LCD_COL_INACTIVE                   0x62C2  // inactive bitton text (avg. DARKBG and DARKGOLDENROD)
                                           
#define LCD_COL_BLACK                      0x0000
#define LCD_COL_RED                        0xf800  // 5-bit
#define LCD_COL_GREEN                      0x07e0  // 6-bit
#define LCD_COL_BLUE                       0x001f  // 5-bit
#define LCD_COL_CYAN                       0x07ff
#define LCD_COL_MAGENTA                    0xf81f
#define LCD_COL_YELLOW                     0xffe0  // 0b11111 111111 00000
#define LCD_COL_WHITE                      0xffff
#define LCD_COL_OLDLACE2                   0xe6d7
#define LCD_COL_GRAY2                      0x4208
#define LCD_COL_GOLD2                      0xbcc4
#define LCD_COL_DARKBG                     0x0142  

#define LCD_COL_DARKSLATEGREY              0x328a
#define LCD_COL_ALICEBLUE                  0xf7df
#define LCD_COL_ANTIQUEWHITE               0xff5a
#define LCD_COL_AQUA                       0x07ff
#define LCD_COL_AQUAMARINE                 0x7ffa
#define LCD_COL_AZURE                      0xf7ff
#define LCD_COL_BEIGE                      0xf7bb
#define LCD_COL_BISQUE                     0xff38
#define LCD_COL_BLANCHEDALMOND             0xff59
#define LCD_COL_BLUE                       0x001f 
#define LCD_COL_BLUEVIOLET                 0x895c
#define LCD_COL_BROWN                      0xa145
#define LCD_COL_BURLYWOOD                  0xddd0
#define LCD_COL_CADETBLUE                  0x5cf4
#define LCD_COL_CHARTREUSE                 0x7fe0
#define LCD_COL_CHOCOLATE                  0xd343
#define LCD_COL_CORAL                      0xfbea
#define LCD_COL_CORNFLOWERBLUE             0x64bd
#define LCD_COL_CORNSILK                   0xffdb
#define LCD_COL_CRIMSON                    0xd8a7
#define LCD_COL_DARKBLUE                   0x0011
#define LCD_COL_DARKCYAN                   0x03ef 
#define LCD_COL_DARKCYAN2                  0x0451
#define LCD_COL_DARKGOLDENROD              0xbc21
#define LCD_COL_DARKGRAY                   0xad55
#define LCD_COL_DARKGREEN2                 0x0320
#define LCD_COL_DARKGREEN                  0x03e0 
#define LCD_COL_DARKGREY                   0x7bef 
#define LCD_COL_DARKKHAKI                  0xbdad
#define LCD_COL_DARKMAGENTA                0x8811
#define LCD_COL_DARKOLIVEGREEN             0x5345
#define LCD_COL_DARKORANGE                 0xfc60
#define LCD_COL_DARKORCHID                 0x9999
#define LCD_COL_DARKRED                    0x8800
#define LCD_COL_DARKSALMON                 0xecaf
#define LCD_COL_DARKSEAGREEN               0x8df1
#define LCD_COL_DARKSLATEBLUE              0x49f1
#define LCD_COL_DARKSLATEGRAY              0x2a69
#define LCD_COL_DARKTURQUOISE              0x067a
#define LCD_COL_DARKVIOLET                 0x901a
#define LCD_COL_DEEPPINK                   0xf8b2
#define LCD_COL_DEEPSKYBLUE                0x05ff
#define LCD_COL_DIMGRAY                    0x6b4d
#define LCD_COL_DODGERBLUE                 0x1c9f
#define LCD_COL_FIREBRICK                  0xb104
#define LCD_COL_FLORALWHITE                0xffde
#define LCD_COL_FORESTGREEN                0x2444
#define LCD_COL_FUCHSIA                    0xf81f
#define LCD_COL_GAINSBORO                  0xdefb
#define LCD_COL_GHOSTWHITE                 0xffdf
#define LCD_COL_GOLD                       0xfea0
#define LCD_COL_GOLDENROD                  0xdd24
#define LCD_COL_GRAY                       0x8410
#define LCD_COL_GREEN2                     0x0400
#define LCD_COL_GREEN                      0x07e0 
#define LCD_COL_GREENYELLOW                0xafe5 
#define LCD_COL_HONEYDEW                   0xf7fe
#define LCD_COL_HOTPINK                    0xfb56
#define LCD_COL_INDIANRED                  0xcaeb
#define LCD_COL_INDIGO                     0x4810
#define LCD_COL_IVORY                      0xfffe
#define LCD_COL_KHAKI                      0xf731
#define LCD_COL_LAVENDER                   0xe73f
#define LCD_COL_LAVENDERBLUSH              0xff9e
#define LCD_COL_LAWNGREEN                  0x7fe0
#define LCD_COL_LEMONCHIFFON               0xffd9
#define LCD_COL_LIGHTBLUE                  0xaedc
#define LCD_COL_LIGHTCORAL                 0xf410
#define LCD_COL_LIGHTCYAN                  0xe7ff
#define LCD_COL_LIGHTGOLDENRODYELLOW       0xffda
#define LCD_COL_LIGHTGREEN                 0x9772
#define LCD_COL_LIGHTGREY                  0xc618 
#define LCD_COL_LIGHTGREY2                 0xd69a
#define LCD_COL_LIGHTPINK                  0xfdb8
#define LCD_COL_LIGHTSALMON                0xfd0f
#define LCD_COL_LIGHTSEAGREEN              0x2595
#define LCD_COL_LIGHTSKYBLUE               0x867f
#define LCD_COL_LIGHTSLATEGRAY             0x7453
#define LCD_COL_LIGHTSTEELBLUE             0xb63b
#define LCD_COL_LIGHTYELLOW                0xfffc
#define LCD_COL_LIME                       0x07e0
#define LCD_COL_LIMEGREEN                  0x3666
#define LCD_COL_LINEN                      0xff9c
#define LCD_COL_MAGENTA                    0xf81f
#define LCD_COL_MAROON                     0x7800  
#define LCD_COL_MAROON2                    0x8000
#define LCD_COL_MEDIUMAQUAMARINE           0x6675
#define LCD_COL_MEDIUMBLUE                 0x0019
#define LCD_COL_MEDIUMORCHID               0xbaba
#define LCD_COL_MEDIUMPURPLE               0x939b
#define LCD_COL_MEDIUMSEAGREEN             0x3d8e
#define LCD_COL_MEDIUMSLATEBLUE            0x7b5d
#define LCD_COL_MEDIUMSPRINGGREEN          0x07d3
#define LCD_COL_MEDIUMTURQUOISE            0x4e99
#define LCD_COL_MEDIUMVIOLETRED            0xc0b0
#define LCD_COL_MIDNIGHTBLUE               0x18ce
#define LCD_COL_MINTCREAM                  0xf7ff
#define LCD_COL_MISTYROSE                  0xff3c
#define LCD_COL_MOCCASIN                   0xff36
#define LCD_COL_NAVAJOWHITE                0xfef5
#define LCD_COL_NAVY                       0x000f 
#define LCD_COL_NAVY2                      0x0010 
#define LCD_COL_OLDLACE                    0xffbc 
#define LCD_COL_OLIVE                      0x7be0 
#define LCD_COL_OLIVE2                     0x8400 
#define LCD_COL_OLIVEDRAB                  0x6c64 
#define LCD_COL_ORANGE                     0xfd20 
#define LCD_COL_ORANGERED                  0xfa20 
#define LCD_COL_ORCHID                     0xdb9a 
#define LCD_COL_PALEGOLDENROD              0xef55 
#define LCD_COL_PALEGREEN                  0x9fd3 
#define LCD_COL_PALETURQUOISE              0xaf7d 
#define LCD_COL_PALEVIOLETRED              0xdb92 
#define LCD_COL_PAPAYAWHIP                 0xff7a 
#define LCD_COL_PEACHPUFF                  0xfed7 
#define LCD_COL_PERU                       0xcc27 
#define LCD_COL_PINK                       0xf81f 
#define LCD_COL_PINK_2                     0xfe19 
#define LCD_COL_PLUM                       0xdd1b 
#define LCD_COL_POWDERBLUE                 0xb71c 
#define LCD_COL_PURPLE                     0x780f 
#define LCD_COL_PURPLE2                    0x8010 
#define LCD_COL_ROSYBROWN                  0xbc71 
#define LCD_COL_ROYALBLUE                  0x435c 
#define LCD_COL_SADDLEBROWN                0x8a22 
#define LCD_COL_SALMON                     0xfc0e 
#define LCD_COL_SANDYBROWN                 0xf52c 
#define LCD_COL_SEAGREEN                   0x2c4a 
#define LCD_COL_SEASHELL                   0xffbd 
#define LCD_COL_SIENNA                     0xa285 
#define LCD_COL_SILVER                     0xc618 
#define LCD_COL_SKYBLUE                    0x867d 
#define LCD_COL_SLATEBLUE                  0x6ad9 
#define LCD_COL_SLATEGRAY                  0x7412 
#define LCD_COL_SNOW                       0xffdf 
#define LCD_COL_SPRINGGREEN                0x07ef 
#define LCD_COL_STEELBLUE                  0x4416 
#define LCD_COL_TAN                        0xd5b1 
#define LCD_COL_TEAL                       0x0410 
#define LCD_COL_THISTLE                    0xddfb 
#define LCD_COL_TOMATO                     0xfb08 
#define LCD_COL_TURQUOISE                  0x471a 
#define LCD_COL_VIOLET                     0xec1d 
#define LCD_COL_WHEAT                      0xf6f6 
#define LCD_COL_WHITESMOKE                 0xf7be
#define LCD_COL_YELLOWGREEN                0x9e66

// -- prototypes
void init_lcd (void);

void lcd_clrscr (uint16_t col);
void lcd_prtscr (void);

void lcd_setX (uint16_t x0, uint16_t x1);
void lcd_setY (uint16_t y0, uint16_t y1);

void lcd_drawpixel (uint16_t x0, uint16_t y0, uint16_t color);
void lcd_drawline  (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void lcd_drawrect  (uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t col);  

void lcd_blocks (void);

void lcd_wr_cmd   (uint8_t cmd);
void lcd_wr_dat   (uint8_t dat);
void lcd_wr_dat16 (uint16_t dat);
void lcd_wr_col   (uint16_t col);

#endif


