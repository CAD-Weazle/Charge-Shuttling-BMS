// file    : gui.h
// author  : rb
// purpose : header file for gui.c
// date    : 240327
// last    : 261019
//

#ifndef _GUI_H_
#define _GUI_H_

#include "bms.h"  // defines 'bms_t bms', for use in this header file

// -- defines
#define GUI_X_OFS                               0  // offset in touch screen x-axis
#define GUI_Y_OFS                               5  // offset in touch screen y-axis

// battery cells defines
#define GUI_CELL_BORDER     LCD_COL_DARKGOLDENROD  // cell body outline color
#define GUI_CELL_FILL             LCD_COL_MAROON2  // cell charge fill color
#define GUI_CELL_BG                LCD_COL_DARKBG  // cell background color
#define GUI_CELL_ORGX                          26  // cell origin upperleft corner
#define GUI_CELL_ORGY                          22  // cell origin upperleft corner
#define GUI_CELL_LENGTH                       100  // charge bar length
#define GUI_CELL_HEIGTH                        24  // charge bar height
#define GUI_CELL_SPACE      (GUI_CELL_HEIGTH + 10) // intercell spacing

// solar/battery display panel
#define GUI_PANEL_COLFG     LCD_COL_DARKGOLDENROD  // cell body outline color
#define GUI_PANEL_COLBG            LCD_COL_DARKBG  // cell background color
#define GUI_PANEL_ORGX                          0  // cell origin upperleft corner
#define GUI_PANEL_ORGY  (LCD_ROTHEIGHT-GUI_PANEL_HEIGTH) // cell origin upperleft corner
#define GUI_PANEL_LENGTH            (LCD_ROTWIDTH) // U/I display panel lenght
#define GUI_PANEL_HEIGTH                       80  // U/I display panel height

// -- structs

// battery Cell data 
typedef struct
{ 
  int   cnum;                          // Cell number
  float Vcell;                         // Cell voltage [V]
  int   Vperc;                         // Cell voltage as percent of cell voltage range [%]
  int   Vpold;                         // state [%]
} batcell_t; 

// button data

// forward declaration so 'button_t' it can be used inside the struct
typedef struct button_t button_t;

struct button_t
{
 uint16_t    but_x;                    // but_x
 uint16_t    but_y;                    // but_y
 uint16_t    but_w;                    // but_w
 uint16_t    but_h;                    // but_h
                                       
 const char *lbl_text;                 // button label text 
 uint16_t    lbl_x;                    // button label text x position
 uint16_t    lbl_y;                    // button label text y position
                                       
 uint16_t    col_bg;                   // button background color
 uint16_t    col_pressed;              // button text color when active/on
 uint16_t    col_inactive;             // button text color when inactive/off
                                       
 uint16_t    touch_busy;               // flag used to latch touch screen button press
                                       
 uint16_t    but_pressed;              // button has been pressed
 uint16_t    but_active;               // button state: 0: inactive (off) / 1: active (on)
                                       
 const       font_t *font;             // text font

 void (*callback)(button_t *button);   // butten callback function
};


// -- prototypes
void init_gui (void);

void gui_update (bms_t *bms);

void gui_bcell_init   (void);
void gui_bcell_update (float *v_cell);
void gui_bcell_render (void);
void gui_bcell_body   (uint16_t xpos, uint16_t ypos, uint16_t color, batcell_t *bcell);

void gui_panel_init   (void);
void gui_panel_update (float Vsol, float Vbat, float Ibat, int dcdc_state, float dV);

void gui_draw_vbat (float Vbat);
void gui_draw_vcrg (float Vcrg);
void gui_draw_ibat (float Ibat);
void gui_draw_dV   (float dV);

void     gui_button_init    (void);
void     gui_button_draw    (button_t *button);
void     gui_button_scan    (touch_t *touch);
uint32_t gui_button_pressed (button_t *button, touch_t *touch);
void     gui_button_update  (button_t *button);

void gui_update_buttons (void);

void gui_bms_alarm (void);

void callback_button_bms    (button_t *button);
void callback_button_cycle  (button_t *button);
void callback_button_charge (button_t *button);
void callback_button_load   (button_t *button);

void gui_bms_alarm     (void);
void gui_charger_alarm (void);
void gui_load_alarm    (void);

void gui_active_load   (void);
void gui_inactive_load (void);

void gui_active_charge   (void);
void gui_inactive_charge (void);

#endif





