// file    : gui.h
// author  : rb
// purpose : BMSTEST GUI
// date    : 240327
// last    : 261018
//

#include "includes.h"
#include "buttons.h"

// struct holding state battery pack Cells
batcell_t bcell[LIFEPO4_CELL_NUM]; 


// init battery monitor GUI
void init_gui (void)
{
  // setup battery Cell graphics & structs
  gui_bcell_init ();

  // draw Voltage/Current panels
  gui_panel_init ();

  // draw touch buttons
  gui_button_init ();
}

// update display with BMS data
void gui_update (bms_t *bms)
{
  gui_panel_update (bms->Vpck_actual, bms->Vcrg_actual, bms->Ipck_actual, bms->ctl_state, bms->cell_dv);
  gui_bcell_update (bms->Vcell);
}


// ** battery Cell routines

// setup battery Cell graphics & init structs
void gui_bcell_init (void)
{
  // init battery Cell state
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    bcell[i].cnum  = i;
    bcell[i].Vcell = LIFEPO4_CELL_VCUVLO; // minimum allowed Cell voltage by BMS
    bcell[i].Vpold = 0;
  }

  // draw battery Cell outlines
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
    gui_bcell_body (GUI_CELL_ORGX, GUI_CELL_ORGY, GUI_CELL_BORDER, &bcell[i]);
}

// update battery Cell graphics with new BMS data
void gui_bcell_update (float *v_cell)
{
  // copy battery Cell voltages from BMS
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
    bcell[i].Vcell = v_cell[i];

  // calculate Cell voltage as percentage of allowed range 
  // note: Cell voltage percentage = [(Vcell - Vcell_min) / (Vcell_max - Vcell_min)] * TOTAL_BAR_LENGHT
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  { 
    // clip Cell voltage at lowest allowed value
    if (v_cell[i] < LIFEPO4_CELL_VCUV) 
      v_cell[i] = LIFEPO4_CELL_VCUV;   

    // clip Cell voltage at highest allowed value
    if (v_cell[i] > LIFEPO4_CELL_VCOV)
      v_cell[i] = LIFEPO4_CELL_VCOV;
   
    float ratio = (v_cell[i] - LIFEPO4_CELL_VCUV) / (LIFEPO4_CELL_VCOV - LIFEPO4_CELL_VCUV);

    // make sure 'ratio' is never negative (not really needed, just in case...)
    if (ratio < 0.0f)
      ratio = 0.0f;

    bcell[i].Vperc = (int)(ratio * GUI_CELL_LENGTH);
  }

  // render new data on LCD
  gui_bcell_render ();
}

// draw Cell voltages
void gui_bcell_render (void)
{
  char str[6];
  int dV[LIFEPO4_CELL_NUM];

  int xpos = GUI_CELL_ORGX;
  int ypos = GUI_CELL_ORGY;
 
  // draw Cell voltage as text
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    sprintf (str, "%1.3fV", bcell[i].Vcell);
    lcd_puts (GUI_CELL_ORGX + 120, GUI_CELL_ORGY+5 + i*GUI_CELL_SPACE, str, &normal_text);
  }

  // draw voltage as percentage bar
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    // calculate voltage deviation in time
    dV[i] = (bcell[i].Vperc - bcell[i].Vpold);

    // increase Voltage bar
    if (dV[i] > 0)
    {
      // draw bar
      for (int j = 0; j < dV[i]; j++)
      {
        lcd_drawline (xpos+bcell[i].Vpold, i*GUI_CELL_SPACE + ypos + 3, 
                      xpos+bcell[i].Vpold, i*GUI_CELL_SPACE + ypos + GUI_CELL_HEIGTH-3, 
                      GUI_CELL_FILL);
      }

      // update state
      if (bcell[i].Vpold < GUI_CELL_LENGTH)  // clip at 100%
        bcell[i].Vpold = bcell[i].Vpold + 1;
    }

    // decrease Voltage bar
    if (dV[i] < 0)
    {
      // remove bar
      for (int j = 0; j < abs(dV[i]); j++)
      {
        lcd_drawline (xpos+bcell[i].Vpold, i*GUI_CELL_SPACE + ypos + 3, 
                      xpos+bcell[i].Vpold, i*GUI_CELL_SPACE + ypos + GUI_CELL_HEIGTH-3, 
                      GUI_CELL_BG);
      }

      // update state
      if (bcell[i].Vpold > 0)  // clip at 0%
        bcell[i].Vpold = bcell[i].Vpold - 1;
    }
  }
}

// draw battery Cell body
void gui_bcell_body (uint16_t xpos, uint16_t ypos, uint16_t color, batcell_t *bcell)
{
  const uint16_t y = ypos + (bcell->cnum * GUI_CELL_SPACE);
  const uint16_t x_right = xpos + GUI_CELL_LENGTH;
  
  // draw Cell outline
  
  // -- top edge
  for (uint16_t i = 0; i < 3; i++)
    lcd_drawline(xpos, y + i, x_right + 1, y + i, color);
  
  // -- bottom edge 
  for (uint16_t i = 0; i < 3; i++)
    lcd_drawline(xpos, y + GUI_CELL_HEIGTH - 1 - i, x_right + 1, y + i, color);
  
  // -- left edge - rounded corners
  for (uint16_t i = 0; i < 3; i++)
    lcd_drawline(xpos - 1 - i, y + i, xpos - 1 - i, y + GUI_CELL_HEIGTH - i, color);
  
  // -- right edge - rounded corners
  for (uint16_t i = 0; i < 3; i++)
    lcd_drawline(x_right + i, y + i, x_right + i, y + GUI_CELL_HEIGTH - i, color);
  
  // -- Cell nipple
  for (uint16_t i = 0; i < 6; i++)
  {
    const uint16_t x = x_right + 3 + i;
  
    lcd_drawline(x, y + 6, x, y + GUI_CELL_HEIGTH - 6, color);
  }
}


// ** Charger & Battery Pack Voltage/Current Panel 

// draw charger & battery pack Voltage/Current panel 
void gui_panel_init (void)
{
  char str[16];

  // draw border
  lcd_drawrect (0, 0, 15, GUI_PANEL_ORGY, GUI_PANEL_COLFG); 
  lcd_drawrect (0, 0, GUI_PANEL_LENGTH, 10, GUI_PANEL_COLFG); 

  // draw rightside status field 
  lcd_drawrect (GUI_PANEL_LENGTH-100, 0, 150, GUI_PANEL_ORGY, GUI_PANEL_COLFG); 

  // set colors
  font_setcolor (&invert_text, 0x0000, GUI_CELL_BORDER);

  // draw panel background
  lcd_drawrect (GUI_PANEL_ORGX, GUI_PANEL_ORGY, GUI_PANEL_LENGTH, GUI_PANEL_HEIGTH, GUI_PANEL_COLFG); 

  // draw 'panels'
  lcd_drawrect ( 14, 170, 90, 50, GUI_PANEL_COLBG); 
  lcd_drawrect (114, 170, 90, 50, GUI_PANEL_COLBG); 
  lcd_drawrect (214, 170, 90, 50, GUI_PANEL_COLBG); 

  // draw caption text
  sprintf (str, "%s", "BATTERY");
  lcd_puts (24, 224, str, &invert_text);    

  sprintf (str, "%s", "CHARGER");
  lcd_puts (126, 224, str, &invert_text);    

  sprintf (str, "%s", "CURRENT");
  lcd_puts (224, 224, str, &invert_text);    
}

// update charger & battery pack Voltage/Current data
void gui_panel_update (float Vsol, float Vbat, float Ibat, int dcdc_state, float dV)
{
  // draw battery pack Voltage
  gui_draw_vbat (Vbat);

  // draw charger Voltage
  gui_draw_vcrg (Vsol);

  // draw battery pack Current
  gui_draw_ibat (Ibat);

  // draw maximum inter-cell voltage difference
  gui_draw_dV (dV);
}

// draw battery pack Voltage
void gui_draw_vbat (float Vbat)
{
         char str[16];        // current string to draw
  static char old[16] = {0};  // former string, needed to wipe screen

  // sanity check
  if (Vbat < 0.0)
    Vbat = 0.0;

  // convert to string
  sprintf (str, "%2.1f", Vbat);

  // wipe former text - only when needed
  if (strlen(old) != strlen(str))
  {
    font_setcolor (&large_text, GUI_CELL_BG, GUI_CELL_BG);             // set font to background color
    lcd_puts (23, 180, old, &large_text);                               // right align small values
    font_setcolor (&large_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);   // switch back to normal color
  }

  // draw input voltage value
  if (strlen (str) == 3)
  {
    // right align 1-digit values
    lcd_puts (23+18, 180, str, &large_text);
  }
  else
  {
    // signal battery undervoltage condition with text in alarm color
    if (Vbat < LIFEPO4_CELL_VCUVLO) 
      font_setcolor (&large_text, LCD_COL_RED, GUI_CELL_BG); // set font to alarm color

    // right align 2-digit values
    lcd_puts (23, 180, str, &large_text);    
  }

  // switch back to normal text color
  font_setcolor (&large_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);

  // update state
  strcpy (old, str);
}

// draw battery pack Voltage
void gui_draw_vcrg (float Vcrg)
{
         char str[16];        // current string to draw
  static char old[16] = {0};  // former string, needed to wipe screen

  // sanity check
  if (Vcrg < 0.0)
    Vcrg = 0.0;

  // convert to string
  sprintf (str, "%2.1f", Vcrg);

  // wipe former text - only when needed
  if (strlen(old) != strlen(str))
  {
    font_setcolor (&large_text, GUI_CELL_BG, GUI_CELL_BG);             // set font to background color
  //lcd_puts (105, 180, old, &large_text);                             // right align small values
    lcd_puts (124, 180, old, &large_text);                             // right align small values
    font_setcolor (&large_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);   // switch back to normal color
  }

  // draw battery voltage, signal undervoltage condition
  if (strlen (str) == 3)
  {
    // set font to alarm color
    font_setcolor (&large_text, LCD_COL_RED, GUI_CELL_BG);           

    // right align 1-digit values
    lcd_puts (124+18, 180, str, &large_text);                        
  }
  else
  {
    // right align 2-digit values
    lcd_puts (124, 180, str, &large_text);   
  }

  // update state
  strcpy (old, str);
}

// draw battery current
void gui_draw_ibat (float Ibat)
{
         char str[16];        // current string to draw
  static char old[16] = {0};  // former string, needed to wipe screen

  // convert to string
  sprintf (str, "%2.1f", Ibat);

  // wipe former text - only when needed
  if (strlen(old) != strlen(str))
  {
    font_setcolor (&large_text, GUI_CELL_BG, GUI_CELL_BG);             // set font to background color
    lcd_puts (223, 180, old, &large_text);                             // right align small values
    font_setcolor (&large_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);   // switch back to normal color
  }

  // draw battery current
  if (strlen (str) == 3)
  {
    // right align 1-digit values
    lcd_puts (223+18, 180, str, &large_text);    
  }
  else
  {
    // right align 2-digit values
    lcd_puts (223, 180, str, &large_text);   
  }

  // update state
  strcpy (old, str);
}

// draw maximum inter-cell voltage difference
void gui_draw_dV (float dV)
{
  char str[6];       

  // sanity check
  if (dV < 0.0)
    dV = 0.0;

  // clip at 2 digits
  if (dV >= 0.099)
    dV = 0.099;

  // convert to string
  sprintf (str, " %2dmV", (int)(dV*1000));

  // set text color
  if (dV < 0.099)
    font_setcolor (&normal_text, GUI_CELL_BG, LCD_COL_DARKOLIVEGREEN);  // all ok: normal text color
  else
    font_setcolor (&normal_text, LCD_COL_RED, LCD_COL_DARKOLIVEGREEN);  // diff too big: exeption text color

  // & draw text
  lcd_puts (GUI_PANEL_LENGTH - 86, 20, str, &normal_text);    

  // switch back to normal text color agian
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
}


//** touch screen buttons routines 

// setup all buttons
void gui_button_init (void)
{
  gui_button_draw (&button_info);
  gui_button_draw (&button_bms);
  gui_button_draw (&button_cycle);
  gui_button_draw (&button_charge);
  gui_button_draw (&button_load);
}

// draw button body & text
void gui_button_draw (button_t *button)
{
  // draw button shape
  lcd_drawrect (button->but_x, button->but_y, button->but_w, button->but_h, button->col_bg);

  // set font color to inactive state
  font_setcolor (&large_text,  button->col_inactive, button->col_bg);
  font_setcolor (&normal_text, button->col_inactive, button->col_bg);

  // draw button text
  lcd_puts (button->lbl_x, button->lbl_y, button->lbl_text, button->font); 

  // switch back to normal font color again
  font_setcolor (&large_text,  LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);  
}

// scan all buttons for touch event - called shortly after touch ISR 
void gui_button_scan (touch_t *touch)
{
  // set 'pressed' flag when touch is within button boundaries
  if (gui_button_pressed (&button_bms, touch))
    button_bms.but_pressed = 1;

  if (gui_button_pressed (&button_cycle, touch))
    button_cycle.but_pressed = 1;;

  if (gui_button_pressed (&button_load, touch))
    button_load.but_pressed = 1;

  if (gui_button_pressed (&button_charge, touch))
    button_charge.but_pressed = 1;
}

// check for touched buttons 
// note: with 2-finger touch, both 'pressed' and 'release' flag can be '0' 
uint32_t gui_button_pressed (button_t *button, touch_t *touch)
{
  // touch screen coordinates adjusted for LCD coordinates
  uint16_t x_adjust = touch->x_pos - GUI_X_OFS;
  uint16_t y_adjust = touch->y_pos - GUI_Y_OFS;

  // bail when not pressed inside button boundary
  if ((x_adjust < button->but_x) || (x_adjust >= (button->but_x + button->but_w)) 
      ||
      (y_adjust < button->but_y) || (y_adjust >= (button->but_y + button->but_h)))
  {
    button->touch_busy = 0;  // reset flag
    return (0);
  }

  // trigger once when button is initially pressed
  if (touch->pressed && !button->touch_busy)
  {
    button->touch_busy = 1;
    return (1);
  }

  // reset busy flag after touch is released
  if (touch->release)
    button->touch_busy = 0;

  return (0);
}

// update button text color & state
void gui_update_buttons (void)
{
  gui_button_update (&button_bms);
  gui_button_update (&button_cycle);
  gui_button_update (&button_load);
  gui_button_update (&button_charge);
}

// update button text color depending on active/inactive state & callback
void gui_button_update (button_t *button)
{
  // button not pressed, bail
  if (button->but_pressed == 0)
    return;

  // clear flag
  button->but_pressed = 0;

  // set font color
  if (button->but_active)
  {
    font_setcolor (&large_text,  button->col_inactive, button->col_bg);
    font_setcolor (&normal_text, button->col_inactive, button->col_bg);
    button->but_active = 0;
  }
  else
  {
    font_setcolor (&large_text,  button->col_pressed, button->col_bg);
    font_setcolor (&normal_text, button->col_pressed, button->col_bg);
    button->but_active = 1;
  }

  // draw button text
  lcd_puts (button->lbl_x, button->lbl_y, button->lbl_text, button->font);   

  // switch back to normal font color again
  font_setcolor (&large_text,  LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);

  // invoke the callback
  if (button->callback != NULL)
    button->callback (button);
}

// only used to signal BMS error by changing button text color
void gui_bms_alarm (void)
{
  // switch tp error color
  font_setcolor (&normal_text, LCD_COL_RED, GUI_CELL_BG);

  // draw button text
  lcd_puts (button_bms.lbl_x, button_bms.lbl_y, button_bms.lbl_text, button_bms.font); 

  // switch back to normal font color again
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
}

// only used to signal charger error by changing button text color
void gui_charger_alarm (void)
{
  // switch tp error color
  font_setcolor (&normal_text, LCD_COL_RED, GUI_CELL_BG);

  // draw button text
  lcd_puts (button_charge.lbl_x, button_charge.lbl_y, button_charge.lbl_text, button_charge.font); 

  // switch back to normal font color again
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
}

// only used to signal load error by changing button text color
void gui_load_alarm (void)
{
  // switch tp error color
  font_setcolor (&normal_text, LCD_COL_RED, GUI_CELL_BG);

  // draw button text
  lcd_puts (button_load.lbl_x, button_load.lbl_y, button_load.lbl_text, button_load.font); 

  // switch back to normal font color again
  font_setcolor (&normal_text, LCD_COL_DARKGOLDENROD, GUI_CELL_BG);
}


//** touch screen buttons callback routines 

// callback function for BMS button
void callback_button_bms (button_t *button)
{
  if (button->but_active)
    bms_enable ();
  else
    bms_disable ();
}

// callback function for CYCLE button
void callback_button_cycle (button_t *button)
{
  if (button->but_active)
    bms_cycle_enable  ();
  else
    bms_cycle_disable ();
}

// callback function for CHARGE button
void callback_button_charge (button_t *button)
{
  if (button->but_active)
    charger_enable  ();
  else
    charger_disable ();
}

// callback function for LOAD button
void callback_button_load (button_t *button)
{
  if (button->but_active)
    load_enable  ();
  else
    load_disable ();
}

// change color of buttons 'C' and 'L' - quick fix, for cycle test
void gui_active_load (void)
{
  button_load.but_pressed = 1;
  button_load.but_active  = 0;
}

void gui_inactive_load (void)
{
  button_load.but_pressed = 1;
  button_load.but_active  = 1;
}

void gui_active_charge (void)
{
  button_charge.but_pressed = 1;
  button_charge.but_active  = 0;
}

void gui_inactive_charge (void)
{
  button_charge.but_pressed = 1;
  button_charge.but_active  = 1;
}
