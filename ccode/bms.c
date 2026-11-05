// file    : bms.c
// author  : rb
// purpose : Buck-Boost Converter Charge Shuttling Active Battery Management System
// date    : 260331
// last    : 261023
//
// note    : battery Cells are numbered bottom to top
//
//           GND -> -Cell1+ -> -Cell2+ -> -Cell3+ -> -Ceel4+ -> LiFePO4 pack_output/charger_input

#include "includes.h"

#define BMS_DEBUG             (0U)  // 0: normal BMS operation / 1: print BMS debug output
#define BMS_DEBUG_BUCK        (0U)  // 0: normal BMS operation / 1: print BMS debug output
#define BMS_DEBUG_BOOST       (0U)  // 0: normal BMS operation / 1: print BMS debug output

// struct holding BMS state
bms_t bms;

// setup BMS
void init_bms (void)
{
  // reset BMS state
  bms_reset ();

  // always start with pass-FET matrix test
  bms_pass_run ();

  // reset DC/DC error counter
  bms.dcdc_err = 0;
}

// reset state machines & balancer off
void bms_reset (void)
{
  // shut down DC/DC charger
  pwm_disable ();

  // block all pass-FETs
  bms_pass_block ();

  // put all BMS FSMs in reset state
  bms.ctl_state = BMS_CONTROL_START; 
  bms.bst_state = BMS_BALANCE_INIT; 
  bms.bck_state = BMS_BALANCE_INIT; 

  // reset all BMS flags
  bms.bms_on  = 0U;
  bms.bms_run = 0U;
  bms.pass_on = 0U;

  // all LEDs off
  led_boost_off ();
  led_buck_off  ();
  led_grn_off   ();
}

// run the BMS
void bms_update (void)
{
  static uint32_t cnt = 0U;
  static uint32_t err = 0U;

  // LCD backlight control
  if (action_button ())
    lcd_blight_toggle ();

  // first check pass-FET matrix functionality
  if ((err != 0U) && (bms_pass_test () != 0U))
  {
    printf ("#pass-FET matrix error, BMS stopped\n");
    err = 1U;
    return;
  }

  // get Cell clamp voltages
  bms.Vcell[0] = adc_read_vcell1 ();  
  bms.Vcell[1] = adc_read_vcell2 ();  
  bms.Vcell[2] = adc_read_vcell3 ();  
  bms.Vcell[3] = adc_read_vcell4 ();  

  // get DC/DC buck & boost node voltage
  bms.Vbck_actual = adc_read_vbuck  ();
  bms.Vbst_actual = adc_read_vboost ();

  // get DC/DC buck & boost node current
  bms.Ibck_actual = adc_read_ibuck  ();                    
  bms.Ibst_actual = adc_read_iboost ();                    

  // get battery pack & charger voltage/current
  bms.Vpck_actual = adc_read_vpck ();
  bms.Ipck_actual = adc_read_ipck ();
  bms.Vcrg_actual = adc_read_vcrg ();

  // calculate dynamic maximum inter-cell voltage difference - for debug & LCD display only
  uint32_t cell_lo = bms_cell_lowest  (bms.Vcell);
  uint32_t cell_hi = bms_cell_highest (bms.Vcell);
  bms.cell_dv = bms.Vcell[cell_hi] - bms.Vcell[cell_lo];

  // check charge transfer hardware, bail on error
  if (bms_check_dcdc () != 0)
  {
    printf ("#charge transfer hardware error, BMS stopped: %d\n", bms.dcdc_err);
    return;
  }

  // check battery pack health, bail on error
  if (bms_check_pack () != BMS_PACKERR_NONE)
  {
    printf ("#battery pack error, BMS stopped\n");
    return;
  }

  // BMS not active, bail
  if (bms.bms_on == 0)
    return;

  // charge transfer main control loop
  switch (bms.ctl_state)
  {
    case BMS_CONTROL_START:
    {
      // find Cells with highest / lowest clamp voltage 
      bms.cell_lo = bms_cell_lowest  (bms.Vcell);
      bms.cell_hi = bms_cell_highest (bms.Vcell);
      
      // & calculate max. inter-cell voltage difference
      bms.cell_dV = bms.Vcell[bms.cell_hi] - bms.Vcell[bms.cell_lo];
    
      debug (BMS_DEBUG, "#lowest Vcell: %d highest Vcell: %d diff: %.3f\n", bms.cell_lo+1, bms.cell_hi+1, bms.cell_dV);

      // do not start belancing when voltage difference is too small
      if (bms.cell_dV < LIFEPO4_CDIV_PERFECT)
      {
        // balancing idle
        bms.bms_run = 0U;
      }

      // start balancing when difference with added hysterisis is too big
      if (bms.cell_dV > LIFEPO4_CDIV_PERFECT + LIFEPO4_CDIV_HYST)
      {
        // balancing busy
        bms.bms_run = 1U;
      }

      // select balancing method: buck or boost
      if (bms.bms_run)
      {
        // start BMS in buck or boost mode
        if (bms.cell_hi < bms.cell_lo)        // a lower Cell has a higher clamp voltage than an upper Cell  - start boost mode
          bms.ctl_state = BMS_CONTROL_BOOST;
        else if (bms.cell_hi > bms.cell_lo)   // an upper Cell has a higher clamp voltage than a lower Cell - start buck mode
          bms.ctl_state = BMS_CONTROL_BUCK;
        else
          bms.ctl_state = BMS_CONTROL_ERROR;
      }

      break;
    }

    case BMS_CONTROL_BOOST:
    {
      if (bms_boost_mode (bms.cell_hi, bms.cell_lo) == 0)
      {
        debug (BMS_DEBUG, "#boost mode charge transfer ready, enter Cell voltage relaxation period\n");
        bms.ctl_state = BMS_CONTROL_IDLE;
      }

      break;
    }

    case BMS_CONTROL_BUCK:
    {
      if (bms_buck_mode (bms.cell_hi, bms.cell_lo) == 0)
      {
        debug (BMS_DEBUG, "#buck mode charge transfer ready, enter Cell voltage relaxation period\n");
        bms.ctl_state = BMS_CONTROL_IDLE;
      }

      break;
    }

    case BMS_CONTROL_IDLE:
    {
      if (cnt++ > BMS_RELAXATION_TIME)
      {
        cnt = 0;
        bms.ctl_state = BMS_CONTROL_START;
      }

      break;
    }

    case BMS_CONTROL_ERROR:
    {
      // reset all BMS FSM's
      bms_reset ();
      printf ("#some BMS error\n");

      break;
    }

    default:
      break;
  }
}

// charge transfer in buck mode - from upper to lower cell
uint32_t bms_buck_mode (int from_cell, int to_cell)
{
  static int cnt = 0;

  switch (bms.bck_state)
  {
    case BMS_BALANCE_INIT:
    {
      // sanity check, bail on error
      if (from_cell <= to_cell)
      {
        printf ("#can't transport charge in buck mode from Cell%d to Cell%d\n", from_cell+1, to_cell+1);
        return (-1);
      }

      // signal buck converter active
      led_buck_on ();

      // make sure DC/DC PWM is disabled 
      pwm_disable (); // <> needed?

      // reset counter
      cnt = 0;

      bms.bck_state = BMS_BALANCE_START;
      break;
    }

    case BMS_BALANCE_START:
    {
      debug (BMS_DEBUG, "#set to safe PWM duty cycle & enable DC/DC in buck mode\n");

      // set duty cycle to safe value for buck mode
      bms.pwm_ccr = DCDC_PWM_BUCKSAFE;

      // & enable DC/DC converter in buck mode
      pwm_set (bms.pwm_ccr);  // stalling, beware of timing

      bms.bck_state = BMS_BALANCE_SOURCE;
      break;
    }

    case BMS_BALANCE_SOURCE:
    {
      debug (BMS_DEBUG, "#connect upper Cell%d as input buck converter\n", from_cell+1);

      // connect upper cell - source
      cell_pass_upper (from_cell);

      bms.bck_state = BMS_BALANCE_ADJUST_VOLTAGE;
      break;
    }

    case BMS_BALANCE_ADJUST_VOLTAGE:
    {
      // get target voltage for buck conversion
      bms.Vbck_target = adc_read_vstack (to_cell);

      // get voltage difference source and target
      float diff = bms.Vbck_actual - bms.Vbck_target;

      debug (BMS_DEBUG_BUCK, "#BCK_ADJUSTV: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BUCK, "Vstack: %.4f V Vbck_act: %.4f DCDC_error: %d diff: %.3f - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbck_actual, bms.dcdc_err, diff, from_cell+1, to_cell+1);

      // adjust DC/DC PWM duty cycle & check difference voltage between source and target 
      // note: difference must be small enough to prevent huge inrush currents - this is critical!!!!
      if ((diff > 0.0) && (diff < BMS_CELL_SAFEDIFF))
      { 
        // difference between source and target small enough: connect both together
        debug (BMS_DEBUG_BUCK, "#BCK_ADJUSTV: inter-cell voltage difference small enough: connect Cell%d with Cell%d\n", from_cell+1, to_cell+1);
        cell_pass_lower (to_cell);

        bms.bck_state = BMS_BALANCE_ADJUST_CURRENT; 
      }
      else
      {
        if (bms.pwm_ccr < DCDC_PWMMAX)
        {
          uint32_t step = (fabs(diff) > BMS_PWM_ADJUST_LIMIT) ? BMS_PWM_ADJUST_FAST : BMS_PWM_ADJUST_SLOW;
        
          // adjust DC/DC PWM, clip at max
          if (step > (DCDC_PWMMAX - bms.pwm_ccr))
            bms.pwm_ccr = DCDC_PWMMAX;
          else
            bms.pwm_ccr += step;

          // update PWM dutycycle
          pwm_set (bms.pwm_ccr);  // stalling, beware of timing
        }
      }

      break;
    }

    case BMS_BALANCE_ADJUST_CURRENT:
    {
      debug (BMS_DEBUG_BUCK, "#BCK_ADJUSTI: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BUCK, "Vstack: %.4f V Vbck_act: %.4f DCDC_error: %d - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbck_actual, bms.dcdc_err, from_cell+1, to_cell+1);

      // slowly approach final charging current
      if (bms.Ibck_actual < BMS_ICHARGE)
      {
        // update state
        bms.pwm_ccr++;

        // update PWM dutycycle
        pwm_set (bms.pwm_ccr);  // stalling, beware of timing
      } 
      else
      {
        bms.bck_state = BMS_BALANCE_TRANSFER;
        break;
      }

      break;
    }

    case BMS_BALANCE_TRANSFER:
    {
      debug (BMS_DEBUG_BUCK, "#BCK_TRANSFR: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BUCK, "Vstack: %.4f V Vbck_act: %.4f DCDC_error: %d - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbck_actual, bms.dcdc_err, from_cell+1, to_cell+1);

      // stop charge transfer after fixed time
      if (cnt++ > BMS_TRANSFER_TIME)
      {
        cnt = 0;
        bms.bck_state = BMS_BALANCE_DISCONNECT;
      }

      break;
    }

    case BMS_BALANCE_DISCONNECT:
    {
      debug (BMS_DEBUG, "#after charge transport, disconect Cell %d\n", cnt);

      // disconnect both cells
      cell_block_upper (from_cell);
      cell_block_lower (to_cell);

      // charge transfer done - delay added to prevent spurs on current sense output
      if (cnt++ > BMS_DISCONNECT_TIME)
      {
        cnt = 0;
        bms.bck_state = BMS_BALANCE_DONE;
      }

      break;
    }

    case BMS_BALANCE_DONE:
    {
      // shutdown DC/DC converter 
      pwm_disable ();

      // signal buck converter inactive
      led_buck_off ();

      // charge transfer done 
      bms.bck_state = BMS_BALANCE_INIT;
      return (0);
    }

    case BMS_BALANCE_ERROR:  // <> not used now
    {
      // block all pass-FETs
      bms_pass_block ();

      // shutdown DC/DC converter ASAP
      bms_reset ();

      printf ("#charge transfer in buck mode error condition caught\n");

      // some error, handle in caller - latch error state
      bms.bck_state = BMS_BALANCE_ERROR;
      return (-1);
    } 

    default:
      break;
  }

  // signal busy
  return (1);
}

// charge transfer in boost mode - from lower to upper Cell
uint32_t bms_boost_mode (int from_cell, int to_cell)
{
  static uint32_t cnt = 0;
  static int do_it = 0;

  switch (bms.bst_state)
  {
    case BMS_BALANCE_INIT:
    {
      // sanity check, bail on error
      if (from_cell >= to_cell)
      {
        printf ("#can't transport charge in boost mode from Cell%d to Cell%d\n", from_cell+1, to_cell+1);
        return (-1);
      }

      // signal boost converter active
      led_boost_on ();

      // make sure DC/DC PWM is disabled 
      pwm_disable (); // <> needed?

      // reset counter
      cnt = 0;

      bms.bst_state = BMS_BALANCE_START;
      break;
    }

    case BMS_BALANCE_START:
    {
      debug (BMS_DEBUG_BOOST, "#set to safe PWM duty cycle & enable DC/DC in boost mode\n");

      // set duty cycle to safe value for boost mode
      bms.pwm_ccr = DCDC_PWM_BOOSTSAFE;

      // & enable DC/DC converter in boost mode
      pwm_set (bms.pwm_ccr);  // stalling, beware of timing

      bms.bst_state = BMS_BALANCE_SOURCE;
      break;
    }

    case BMS_BALANCE_SOURCE:
    {
      debug (BMS_DEBUG_BOOST, "#connect lower Cell%d as input boost converter\n", from_cell+1);

      // connect lower cell - source
      cell_pass_lower (from_cell);

      bms.bst_state = BMS_BALANCE_ADJUST_VOLTAGE;
      break;
    }

    case BMS_BALANCE_ADJUST_VOLTAGE:
    {
      // get target voltage for boost conversion
      bms.Vbst_target = adc_read_vstack (to_cell);  // Vstack not constant??? TEST

      // get voltage difference source and target
      float diff = bms.Vbst_target - bms.Vbst_actual;

      debug (BMS_DEBUG_BOOST, "#BST_ADJUSTV: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BOOST, "Vstack: %.4f V Vbst_trg: %.4f Vbst_act: %.4f dcdc_error: %d diff: %.3f - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbst_target, bms.Vbst_actual, bms.dcdc_err, diff, from_cell+1, to_cell+1);

      // when supplying the top Cell3, FETs are starting to conduct due to high Vbst
      // in this situation 'diff' can fall very rapidly and even becom negative
      // extra test catches this event and advances the FSM
      if ((to_cell == 3) && (diff < 0))  
      {
        do_it = 0;
        bms.bst_state = BMS_BALANCE_ADJUST_CURRENT;
        break;
      }

      // adjust DC/DC PWM duty cycle & check difference voltage small enough to prevent huge inrush currents (this is critical!!!!)
      if ((diff > 0) && (diff < BMS_CELL_SAFEDIFF))  
      {
        // voltage difference between source and target small enough: connect both together
        debug (BMS_DEBUG_BOOST, "#BST_ADJUSTV: inter-cell voltage difference small enough: connect Cell%d with Cell%d\n", from_cell+1, to_cell+1);
        cell_pass_upper (to_cell);

        do_it = 0;

        bms.bst_state = BMS_BALANCE_ADJUST_CURRENT;
      }
      else
      {
        if (bms.pwm_ccr > DCDC_PWMMIN)
        {
          uint32_t step = (fabs(diff) > BMS_PWM_ADJUST_LIMIT) ? BMS_PWM_ADJUST_FAST : BMS_PWM_ADJUST_SLOW;
        
          // adjust DC/DC PWM, clip at max
          if (step > (bms.pwm_ccr - DCDC_PWMMIN))
            bms.pwm_ccr = DCDC_PWMMIN;
          else
          {
            // increase voltage slower when Cell4 is charged
            if ((to_cell == 3) && (step == BMS_PWM_ADJUST_SLOW))
            {
              do_it = ~do_it;

              // skip alternating rounds
              if (!do_it)
                break;  
            }

            bms.pwm_ccr -= step;
          }

          // update PWM dutycycle
          pwm_set (bms.pwm_ccr);  // stalling, beware of timing
        }
      }

      break;
    }

    case BMS_BALANCE_ADJUST_CURRENT:
    {
      debug (BMS_DEBUG_BOOST, "#BST_ADJUSTI: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BOOST, "Vstack: %.4f V Vbst_act: %.4f DCDC_error: %d - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbst_actual, bms.dcdc_err, from_cell+1, to_cell+1);

      // slowly approach final charging current
      if (-bms.Ibck_actual < BMS_ICHARGE)  // current is negative
      {
        // update state
        bms.pwm_ccr--;

        // update PWM dutycycle
        pwm_set (bms.pwm_ccr);  // stalling, beware of timing
      } 
      else
      {
        bms.bst_state = BMS_BALANCE_TRANSFER;
        break;
      }

      break;
    }

    case BMS_BALANCE_TRANSFER:
    {
      debug (BMS_DEBUG_BOOST, "#BST_TRANSFR: #PWM_CCR: %d Ibck: %.3f Ibst: %.3f ", bms.pwm_ccr, bms.Ibck_actual, bms.Ibst_actual);     
      debug (BMS_DEBUG_BOOST, "Vstack: %.4f V Vbst_act: %.4f DCDC_error: %d - from Cell%d to Cell%d\n", adc_read_vstack (to_cell), bms.Vbst_actual, bms.dcdc_err, from_cell+1, to_cell+1);

      // stop charge transfer after fixed time
      if (cnt++ > BMS_TRANSFER_TIME)
      {
        cnt = 0;
        bms.bst_state = BMS_BALANCE_DISCONNECT;
      }

      break;
    }

    case BMS_BALANCE_DISCONNECT:
    {
      debug (BMS_DEBUG, "#after charge transport, disconect Cells %d\n", cnt);

      // disconnect both cells
      cell_block_upper (to_cell);
      cell_block_lower (from_cell);

      // charge transfer done - delay added to prevent spurs on current sense output
      if (cnt++ > BMS_DISCONNECT_TIME)
      {
        cnt = 0;
        bms.bst_state = BMS_BALANCE_DONE;
      }

      break;
    }

    case BMS_BALANCE_DONE:
    {
      // shutdown DC/DC converter 
      pwm_disable ();

      // signal boost converter inactive
      led_boost_off ();

      // charge transfer done 
      bms.bst_state = BMS_BALANCE_INIT;
      return (0);
    }

    case BMS_BALANCE_ERROR: // <> not used now
    {
      // block all pass-FETs
      bms_pass_block ();

      // shutdown DC/DC converter
      bms_reset ();

      printf ("#charge transfer in boost mode error condition caught\n");

      // some error, handle in caller - latch error state
      bms.bst_state = BMS_BALANCE_ERROR;
      return (-1);
    } 

    default:
      break;
  }

  // signal busy
  return (1);
}


// start BMS
void bms_enable (void)
{
  // set flag
  bms.bms_on  = 1U;

  // signal BMS on
  led_grn_on ();
}

// stop BMS
void bms_disable (void)
{
  // shutdown & set BMS to safe state
  bms_reset ();
}

// start pass-FET network test
void bms_pass_run (void)
{
  // set flag
  bms.pass_on = 1U;
}


// find Cell with lowest clamp voltage
uint32_t bms_cell_lowest (float *vc)
{
  int   tmp_c = 0;      // suppress compiler warning
  float tmp_v = 999.9;  // start with out of bounds value

  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    if (vc[i] < tmp_v)
    {
      tmp_v = vc[i];
      tmp_c = i;
    }
  }

  return (tmp_c);
}

// find Cell with highest clamp voltage
uint32_t bms_cell_highest (float *vc)
{
  int   tmp_c = 0;      // suppress compiler warning
  float tmp_v = 0.0;    // start with out of bounds value

  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    if (vc[i] > tmp_v)
    {
      tmp_v = vc[i];
      tmp_c = i;
    }
  }

  return (tmp_c);
}

// check for error condition in DC/DC charge transfer hardware
uint32_t bms_check_dcdc (void)
{
  // check for overcurrent in DC/DC current paths - reset BMS but keep going
  // note: overcurrent conditions are mainly transients in the pass-FET matrix
  if ((fabs(bms.Ibck_actual) > BMS_ICHARGE_SAFE) ||
      (fabs(bms.Ibst_actual) > BMS_ICHARGE_SAFE))
  {
    // first, shut down DC/DC converter & reset FSM's state
    bms_reset ();

    // than start BSM back on again
    bms_enable ();

    // change 'BMS button' text color to signal error event
    gui_bms_alarm (); 

    // increment DC/DC error counter
    bms.dcdc_err++;

    printf ("BMS strict error - overcurrent condition: Ibck: %.3f Ibst: %.3f - keep BMS going\n", bms.Ibck_actual, bms.Ibst_actual);
    return (1);                                                       
  }

  // check for overvoltage on DC/DC nodes - shutdown BMS indefinitly 
  // note: overvoltage can damage the battery pack, stop the BMS to prevent that
  if ((bms.Vbst_actual > BMS_VCHARGE_SAFE) ||
      (bms.Vbck_actual > BMS_VCHARGE_SAFE))
  {
    // shut down DC/DC converter & reset FSM's state
    bms_reset ();

    // change 'BMS button' text color to signal error event
    gui_bms_alarm (); 

    // increment DC/DC error counter
    bms.dcdc_err++;

    printf ("#BMS fatal error - overvoltage condition: Vbck: %.3f Vbst: %.3f - shutdown BMS\n", bms.Vbck_actual, bms.Vbst_actual);
    return (1);                                                       
  }

  return (0);
}

// check battery pack health
uint32_t bms_check_pack (void)
{
  uint32_t err = BMS_PACKERR_NONE;

  // check Cell status
  for (int i = 0; i < LIFEPO4_CELL_NUM; i++)
  {
    if ((bms.Vcell[i] > LIFEPO4_CELL_VCOVLO) ||  // Cell overvoltage
        (bms.Vcell[i] < LIFEPO4_CELL_VCUVLO))    // Cell undervoltage
    {
      // shut down BMS
      bms_disable ();

      // change 'bms button' text color to signal error event
      gui_bms_alarm (); 

      // disable input FET - remove charger
      charger_disable ();

      // disable output FET - remove load
      load_disable    ();

      // change 'charger button' text color to signal error event
      gui_charger_alarm (); 

      // change 'load button' text color to signal error event
      gui_load_alarm (); 

      printf ("#faulty Cell%d: %.3fV - shutdown BMS\n", i, bms.Vcell[i]);
      err = BMS_PACKERR_CELL;
    }
  }

  // check inter-cell voltage difference
  if (bms.cell_dV > LIFEPO4_CDIV_DANGER)
  {
    // shut down BMS
    bms_disable ();

    // change 'bms button' text color to signal error event
    gui_bms_alarm (); 

    printf ("#inter-cell voltage difference too big: %.3fV - shutdown BMS\n", bms.cell_dV);
    err = BMS_PACKERR_CELL;
  }

  // check battery pack for overvoltage condition
  if (bms.Vpck_actual >= LIFEPO4_PACK_OV)  // pack overvoltage
  {
    // disable input FET - remove charger
    charger_disable ();

    // change 'charger button' text color to signal error event
    gui_charger_alarm (); 

    printf ("#overvoltage battery pack: %.3fV - disable charger input, keep BMS going\n", bms.Vpck_actual);
    err = BMS_PACKERR_OV;
  }

  // check battery pack for undervoltage condition
  if (bms.Vpck_actual < LIFEPO4_PACK_UV)  // pack undervoltage lockout
  {
    // disable output FET - remove load
    load_disable ();

    // change 'load button' text color to signal error event
    gui_load_alarm (); 

    printf ("#undervoltage battery pack: %.3fV - disable load output, keep BMS going\n", bms.Vpck_actual);
    err = BMS_PACKERR_UV;
  }

  return (err);
}

// block all pass-FETs
void bms_pass_block (void)
{
 cell_block_lower (0);  // block lower Cell1
 cell_block_lower (1);  // block lower Cell2
 cell_block_lower (2);  // block lower Cell3
                        
 cell_block_upper (1);  // block upper Cell2
 cell_block_upper (2);  // block upper Cell3
 cell_block_upper (3);  // block upper Cell4
}


// get BMS state, used for assembling log data in 'adc.c'
uint32_t bms_cell_lo (void)
{
  return (bms.cell_lo);
}

// get BMS state, used for assembling log data in 'adc.c'
uint32_t bms_cell_hi (void)
{
  return (bms.cell_hi);
}

// get BMS state, used for assembling log data in 'adc.c'
uint32_t bms_get_ctl (void)
{
  return (bms.ctl_state);
}

// get max inter-cell voltage difference, used for assembling log data in 'adc.c'
float bms_get_dv (void)
{
  return (bms.cell_dv);
}

// get number of DCDC error, used for assembling log data in 'adc.c'
uint32_t bms_get_dcdcerr (void)
{
  return (bms.dcdc_err);
}


// dump BMS state - dev only
void bms_dump (void)
{
  // voltages & currents
  printf ("VC: %.3f %.3f %.3f %.3f  ", bms.Vcell[0], bms.Vcell[1], bms.Vcell[2], bms.Vcell[3]);
  printf ("Vbst: %.3f Vbck: %.3f  "  , bms.Vbst_actual, bms.Vbck_actual);
  printf ("Ibst: %.3f Ibck: %.3f  "  , bms.Ibst_actual, bms.Ibck_actual);
  printf ("Vpck: %.2f "              , bms.Vpck_actual);
  printf ("Vcrg: %.2f "              , bms.Vcrg_actual);

  // state BMS
  printf ("- dV: %.3f S: %d %d %d ", bms.cell_dv, bms.ctl_state, bms.bck_state, bms.bst_state);

  if ((bms.ctl_state == BMS_CONTROL_START) || (bms.ctl_state == BMS_CONTROL_IDLE))
  {
    // BMS inactive: print nothing 
    printf ("");
  }
  else
  {
    // BMS active: print Cells being balanced 
    printf ("%d -> %d ", bms.cell_hi+1, bms.cell_lo+1);
  }

  printf ("\n");
}
