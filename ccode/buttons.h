// file    : buttons.h
// author  : rb
// purpose : GUI button definitions for BMSTEST
// date    : 240327
// last    : 260930
//

#include "includes.h"

#define GUI_BUTTONX 226
#define GUI_BUTTONx (226 + 42)
#define GUI_BUTTONY 10
#define GUI_BUTTONW 78
#define GUI_BUTTONw 36
#define GUI_BUTTONH 34
#define GUI_BUTTONs 34
#define GUI_BUTTONS (GUI_BUTTONH + 5)

// define buttons

// -- INFO field - not a button, label is used for displaying status info
button_t button_info = 
{
  .but_x        = GUI_BUTTONX,
  .but_y        = GUI_BUTTONY + 0*GUI_BUTTONS,
  .but_w        = GUI_BUTTONW,
  .but_h        = GUI_BUTTONH-1,                  
  .lbl_text     = "",
  .lbl_x        = GUI_BUTTONX + 13,
  .lbl_y        = GUI_BUTTONY + 1*GUI_BUTTONS + 10,
  .col_bg       = LCD_COL_DARKOLIVEGREEN,
  .col_pressed  = LCD_COL_DARKGOLDENROD,
  .col_inactive = LCD_COL_INACTIVE,
  .but_active   = 0,
  .but_pressed  = 0,
  .touch_busy   = 0,
  .font         = &normal_text,
};

// -- BMS button
button_t button_bms = 
{
  .but_x        = GUI_BUTTONX,
  .but_y        = GUI_BUTTONY + 1*GUI_BUTTONS,
  .but_w        = GUI_BUTTONW,
  .but_h        = GUI_BUTTONH-1,                  
  .lbl_text     = "BMS",
  .lbl_x        = GUI_BUTTONX + 21,
  .lbl_y        = GUI_BUTTONY + 1*GUI_BUTTONS + 10,
  .col_bg       = LCD_COL_DARKBG,
  .col_pressed  = LCD_COL_DARKGOLDENROD,
  .col_inactive = LCD_COL_INACTIVE,
  .but_active   = 0,
  .but_pressed  = 0,
  .touch_busy   = 0,
  .font         = &normal_text,
  .callback     = callback_button_bms,
};

// -- CYCLE button
button_t button_cycle = 
{
  .but_x        = GUI_BUTTONX,
  .but_y        = GUI_BUTTONY + 2*GUI_BUTTONS,
  .but_w        = GUI_BUTTONW,
  .but_h        = GUI_BUTTONH-1,                  
  .lbl_text     = "CYCLE",
  .lbl_x        = GUI_BUTTONX + 13,
  .lbl_y        = GUI_BUTTONY + 2*GUI_BUTTONS + 10,
  .col_bg       = LCD_COL_DARKBG,
  .col_pressed  = LCD_COL_DARKGOLDENROD,
  .col_inactive = LCD_COL_INACTIVE,
  .but_active   = 0,
  .but_pressed  = 0,
  .touch_busy   = 0,
  .font         = &normal_text,
  .callback     = callback_button_cycle,
};

// -- LOAD button
button_t button_load = 
{
  .but_x        = GUI_BUTTONx,
  .but_y        = GUI_BUTTONY + 3*GUI_BUTTONS,
  .but_w        = GUI_BUTTONw,
  .but_h        = GUI_BUTTONH-1,                  
  .lbl_text     = "L",
  .lbl_x        = GUI_BUTTONx + 10,
  .lbl_y        = GUI_BUTTONY + 3*GUI_BUTTONS + 2,
  .col_bg       = LCD_COL_DARKBG,
  .col_pressed  = LCD_COL_DARKGOLDENROD,
  .col_inactive = LCD_COL_INACTIVE,
  .but_active   = 0,
  .but_pressed  = 0,
  .touch_busy   = 0,
  .font         = &large_text,
  .callback     = callback_button_load,
};

// -- CHARGER button
button_t button_charge = 
{
  .but_x        = GUI_BUTTONX,
  .but_y        = GUI_BUTTONY + 3*GUI_BUTTONS,
  .but_w        = GUI_BUTTONw,
  .but_h        = GUI_BUTTONH-1,
  .lbl_text     = "C",
  .lbl_x        = GUI_BUTTONX + 8,
  .lbl_y        = GUI_BUTTONY + 3*GUI_BUTTONS + 2,
  .col_bg       = LCD_COL_DARKBG,
  .col_pressed  = LCD_COL_DARKGOLDENROD,
  .col_inactive = LCD_COL_INACTIVE,
  .but_active   = 0, 
  .but_pressed  = 0, 
  .touch_busy   = 0, 
  .font         = &large_text,
  .callback     = callback_button_charge,
};
