// file    : bms_test.c
// author  : rb
// purpose : pass-FET matrix functional test
// date    : 261010
// last    : 261022
//

#include "includes.h"

// globals
float pass_result [6][4];    // pass-FET test results

// BMS pass-FET matrix test
uint32_t bms_pass_test (void)
{
  static uint32_t state = BMS_PASS_START;
  static uint32_t cnt = 0U;
  static uint32_t num_lo = 0U;
  static uint32_t num_up = 1U;
  static uint32_t num_test = 0U;

  static float Vbck;
  static float Vbst;

  // pass-FET matrix test not active, bail with no error
  if (bms.pass_on == 0U)
    return (0U);

  switch (state)
  {
    case BMS_PASS_START:
    {
      // make sure ADC logging is off
      adc_log_off ();

      printf ("#start pass-FET matrix test\n");
      
      // block all pass-FETs first, just to be safe
      cell_block_lower (0);
      cell_block_lower (1);
      cell_block_lower (2);
      
      cell_block_upper (1);
      cell_block_upper (2);
      cell_block_upper (3);

      state = BMS_PASS_SELECT;
      break;
    }

    case BMS_PASS_SELECT:
    {
    //if (num_test < 3)
      if (num_test < BMS_PASS_NUM_LOWER)
        state = BMS_PASS_SELECT_LOWER;
    //else if ((num_test >= 3) && (num_test < 6))
      else if ((num_test >= BMS_PASS_NUM_LOWER) && (num_test < BMS_PASS_NUM_UPPER))
        state = BMS_PASS_SELECT_UPPER;
      else
        state = BMS_PASS_DONE;

      break;
    }

    // test lower Cell pass-FETs - connect to input boost converter
    case BMS_PASS_SELECT_LOWER:
    {
      printf ("#lower FETs %d in pass mode\n", num_lo);

      cell_pass_lower (num_lo);

      state = BMS_PASS_SAMPLE_LOWER;
      break;
    }

    case BMS_PASS_SAMPLE_LOWER:
    {
      // read DC/DC converter node voltages 
      Vbck = adc_read_vbuck  ();
      Vbst = adc_read_vboost ();
    
      // let voltage settle
      if (cnt++ > BMS_PASS_DELAY)
      {
        cnt = 0;
        cell_block_lower (num_lo);
        state = BMS_PASS_COLLECT_LOWER;
      }

      break;
    }

    case BMS_PASS_COLLECT_LOWER:
    {
      pass_result[num_test][0] = Vbck;
      pass_result[num_test][1] = Vbst;
      pass_result[num_test][2] = adc_read_vstack (num_lo);

      num_test++;
      num_lo++;

      state = BMS_PASS_SELECT;
      break;
    }

    // test upper Cell pass-FETs - connect to input buck converter
    case BMS_PASS_SELECT_UPPER:
    {
      printf ("#upper FETs %d in pass mode\n", num_up);

      cell_pass_upper (num_up);

      state = BMS_PASS_SAMPLE_UPPER;
      break;
    }

    case BMS_PASS_SAMPLE_UPPER:
    {
      // read DC/DC converter node voltages 
      Vbck = adc_read_vbuck  ();
      Vbst = adc_read_vboost ();
    
      // let voltage settle
      if (cnt++ > BMS_PASS_DELAY)
      {
        cnt = 0;
        cell_block_upper (num_up);
        state = BMS_PASS_COLLECT_UPPER;
      }

      break;
    }

    case BMS_PASS_COLLECT_UPPER:
    {
      pass_result[num_test][0] = Vbck;
      pass_result[num_test][1] = Vbst;
      pass_result[num_test][2] = adc_read_vstack (num_up);

      num_test++;
      num_up++;

      state = BMS_PASS_SELECT;
      break;
    }

    case BMS_PASS_DONE:
    {
      // reset state
      num_lo = 0;
      num_up = 1;
      num_test = 0;

      // signal end of pass-FET test
      bms.pass_on = 0;

      // dump results te serial port & exit
      return (bms_pass_eval ());

      state = BMS_PASS_START;
      break;
    }

    default:
      break;
  }

  return (0);
}


// dump pass-FET matrix test results
uint32_t bms_pass_eval (void)
{
  // reset error counter
  uint32_t err_cnt = 0;

  printf ("#----------------------------------------------------------------------------------------\n");
  printf ("#lower Cell pass-FETs:\n");

  for (int i = 0; i < BMS_PASS_NUM_LOWER; i++)
  {
    float diff = pass_result[i][0] - pass_result[i][2];

    // derive lower Cell number from loop counter
    int cell_lo = i + 1;

  //printf ("#Cell%d: Vbst: %6.3f Vbck: %6.3f Vstack: %6.3f diff: %6.2f - ", cell_lo, pass_result[i][0], pass_result[i][1], pass_result[i][2], diff); 
    printf ("#Cell%d: Vbck: %6.3f Vbst: %6.3f Vstack: %6.3f diff: %6.2f - ", \
              cell_lo, pass_result[i][0], pass_result[i][1], pass_result[i][2], diff); 

    if (fabs(diff) < (cell_lo * BMS_CELL_TOLERANCE))
    {
      printf ("pass-FETs lower Cell%d OK\n", cell_lo);
    }
    else
    {
      printf ("pass-FETs lower Cell%d \x1b[31merror\x1b[0m\n", cell_lo);
      err_cnt++;
    }
  }

  printf ("#upper Cell pass-FETs:\n");

  for (int i = BMS_PASS_NUM_LOWER; i < BMS_PASS_NUM_UPPER; i++)
  {
    float diff = pass_result[i][1] - pass_result[i][2];

    // derive upper Cell number from loop counter
    int cell_up = i - 1;

    printf ("#Cell%d: Vbck: %6.3f Vbst: %6.3f Vstack: %6.3f diff: %6.2f - ", \
             cell_up, pass_result[i][0], pass_result[i][1], pass_result[i][2], diff); 

    if (fabs(diff) < (cell_up * BMS_CELL_TOLERANCE))
    {
      printf ("pass-FETs upper Cell%d OK\n", cell_up);
    }
    else
    {
      printf ("pass-FETs upper Cell%d \x1b[31merror\x1b[0m\n", cell_up); 
      err_cnt++;
    }
  }

  printf ("#----------------------------------------------------------------------------------------\n");

  // report error
  if (err_cnt == 0U)
    printf ("#no errors found in pass-FET matrix, BMS can start (press BMS button on touch screen)\n");
  else
    printf ("#\x1b[31m#%d errors found in pass-FET matrix, BMS halted\x1b[0m\n", err_cnt);

  // resume logging to serial port again
  adc_log_on ();

  return (err_cnt);
}

uint32_t cycle_on = 0;

void bms_cycle_enable (void)
{
  printf ("#start BMS cycle test\n");

  // set flag
  cycle_on = 1;
}

void bms_cycle_disable (void)
{
  printf ("#stop BMS cycle test\n");

  // close charger input & load output 
  charger_disable ();
  load_disable    ();

  // adjust button text color
  gui_inactive_load   ();
  gui_inactive_charge ();

  // clear flag
  cycle_on = 0U;
}

void bms_cycle_test (void)
{
  static uint32_t busy = 0U;
  static uint32_t cnt  = 0U;

  // BSM cycle test not active, bail
  if (cycle_on == 0U)
  {
    busy = 0;
    return;
  }

  // get actual clamp voltage battery pack 
  float Vpack = adc_read_vpck ();

  // start with discharging
  if (busy == 0U)
  {
    charger_disable     (); // input FET off  - remove charger
    load_enable         (); // output FET on  - apply load
    gui_active_load     (); // adjust button text color
    gui_inactive_charge (); // adjust button text color
    busy = 1;
  }

  // cycle FSM
//if (Vpack > 13.75f) // slow
  if (Vpack > 13.55f)  // 
  {
    if (cnt++ > 600)
    {
      // battery pack full, start discharging 
      charger_disable     (); // input FET off  - remove charger
      load_enable         (); // output FET on  - apply load
      gui_active_load     (); // adjust button text color
      gui_inactive_charge (); // adjust button text color
      cnt = 0;
    }
  }
  else if (Vpack < 12.90f) 
  {
    if (cnt++ > 600)
    {
      // battery pack empty, start charging 
      charger_enable      (); // input FET on   - apply charger
      load_disable        (); // output FET off - remove load
      gui_inactive_load   (); // adjust button text color
      gui_active_charge   (); // adjust button text color
      cnt = 0;
    }
  }
  else
  {
    cnt = 0;
  }
}














