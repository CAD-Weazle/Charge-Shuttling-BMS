// file    : bms.h
// author  : rb
// purpose : header file for bms.c
// date    : 251126
// last    : 261021
//

#ifndef _BMS_H_
#define _BMS_H_

// -- BMS defines 
#define BMS_PWM_ADJUST_SLOW                      (1U) // adjust DC/DC PWM slowly when reaching target voltage
#define BMS_PWM_ADJUST_FAST                     (10U) // adjust DC/DC PWM faster when removed from target
#define BMS_PWM_ADJUST_LIMIT                  (1.25f) // slow/fast V increase limit [V]

#define BMS_TRANSFER_TIME                      (200U) // inter-cell charge stransfer duration               [10*s]  - <> adjust this to own needs
#define BMS_RELAXATION_TIME                    (300U) // Cell voltage relaxation time after charge transfer [10*s]  - <> adjust this to own needs
#define BMS_DISCONNECT_TIME                      (1U) // discconnect delay                                  [10*s]

#define BMS_ICHARGE                            (0.3f) // charge transfer current     [A] - <> adjust this to own needs
#define BMS_ICHARGE_SAFE         (BMS_ICHARGE + 0.2f) // safe value charger current  [A]
#define BMS_VCHARGE_SAFE                      (15.0f) // safe value charger voltage  [V}

#define BMS_CELL_SAFEDIFF                      (0.1f) // prevent in-rush current, safe voltage difference between from_cell & to_cell [V]

// -- LiFePO4 pack safety values
#define LIFEPO4_CELL_NUM                                      (4U) // number of LiFePO4 cells in a battery pack
#define LIFEPO4_CELL_VEOC                                  (3.65f) // End Of Charge voltage LiFePO4 cell        [V]
#define LIFEPO4_PACK_VCUV (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VCUV  ) // undervoltage voltage LiFePO4 pack         [V]
#define LIFEPO4_PACK_VEOC (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VEOC  ) // End Of Charge voltage LiFePO4 pack        [V]
#define LIFEPO4_PACK_OV   (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VCOV  ) // battery pack overvoltage                  [V]
#define LIFEPO4_PACK_OVLO (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VCOVLO) // battery pack overvoltage lockout          [V]
#define LIFEPO4_PACK_UV   (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VCUV  ) // battery pack undervoltage                 [V]
#define LIFEPO4_PACK_UVLO (LIFEPO4_CELL_NUM * LIFEPO4_CELL_VCUVLO) // battery pack undervoltage lockout         [V]

// -- LiFePO4 Cell characteristics
#define LIFEPO4_CELL_VCOVLO                   (3.70f) // LiFePO4 Cell Overvoltage Lockout   [V]
#define LIFEPO4_CELL_VCOV   (LIFEPO4_CELL_VEOC+0.05f) // LiFePO4 Cell Overvoltage           [V]
#define LIFEPO4_CELL_VCOVR  (LIFEPO4_CELL_VEOC-0.05f) // LiFePO4 Cell Overvoltage Recovery  [V]
#define LIFEPO4_CELL_VCUVR                    (3.10f) // LiFePO4 Cell Undervoltage Recovery [V]
#define LIFEPO4_CELL_VCNOM                    (3.30f) // LiFePO4 Cell Nominal Voltage       [V]
#define LIFEPO4_CELL_VCUV                     (3.00f) // LiFePO4 Cell Undervoltage          [V]
#define LIFEPO4_CELL_VCUVLO                   (2.70f) // LiFePO4 Cell Undervoltage Lock Out [V]
                                                     
#define LIFEPO4_CELL_VBMIN                    (2.70f) // LiFePO4 Cell Balancing Minimum Voltage [V]
#define LIFEPO4_CELL_VBMAX                    (3.70f) // LiFePO4 Cell Balancing Maximum Voltage [V]
                                                   
#define LIFEPO4_CDIV_PERFECT                 (0.010f) // inter-cell voltage difference is perfect      [V]
//#define LIFEPO4_CDIV_PERFECT               (0.015f) // inter-cell voltage difference is perfect      [V]
#define LIFEPO4_CDIV_OK                      (0.030f) // inter-cell voltage difference needs balancing [V]
#define LIFEPO4_CDIV_ALARM                   (0.050f) // inter-cell voltage difference something wrong [V]
#define LIFEPO4_CDIV_DANGER                  (0.100f) // inter-cell voltage difference stop using pack [V]
#define LIFEPO4_CDIV_HYST                    (0.005f) // added hysteresis for improved stability       [V]

//  0–10 mV - excellent balance
// 10–20 mV - very good
// 20–30 mV - generally acceptable
// 30–50 mV - check balancing, especially near full charge
// >  50 mV - investigate the cause
// > 100 mV - significant imbalance; check before continuing normal operation

// -- states for battery pack test
enum
{
  BMS_PACKERR_NONE = 0U,               // battery pack OK
  BMS_PACKERR_CELL     ,               // battery cell over- or undervoltage condition found
  BMS_PACKERR_OV       ,               // battery pack overvoltage condition found
  BMS_PACKERR_UV       ,               // battery pack undervoltage condition found
};

// -- main control FSM states
enum ctl_states
{
  BMS_CONTROL_START = 0U,
  BMS_CONTROL_BOOST     ,
  BMS_CONTROL_BUCK      ,
  BMS_CONTROL_IDLE      ,
  BMS_CONTROL_ERROR     ,
};

// -- charge transfer buck/boost FSM states
enum bal_states
{
  BMS_BALANCE_INIT = 0U     ,
  BMS_BALANCE_START         ,
  BMS_BALANCE_SOURCE        ,
  BMS_BALANCE_ADJUST_VOLTAGE,
  BMS_BALANCE_ADJUST_CURRENT,
  BMS_BALANCE_TRANSFER      ,
  BMS_BALANCE_DISCONNECT    ,
  BMS_BALANCE_DONE          ,
  BMS_BALANCE_ERROR         ,
};

// -- struct holds BMS state
typedef struct
{
  uint32_t bms_on;                     // 0: BMS inactive           / 1: BMS active
  uint32_t bms_run;                    // 0: BMS balancing          / 1: BMS waiting
  uint32_t pass_on;                    // 0: pass-FET test inactive / 1: pass-FET test active

  enum ctl_states ctl_state;           // state main BMS control loop FSM
  enum bal_states bst_state;           // state boost converter FSM
  enum bal_states bck_state;           // state buck converter FSM

  float Vpck_actual;                   // battery pack voltage [V]
  float Ipck_actual;                   // battery charger current [A]
  float Vcrg_actual;                   // battery charger voltage [V]

  float Vcell[LIFEPO4_CELL_NUM];       // Cell voltages [V]
  float cell_dV;                       // largest voltage difference between Cells [V] - for control loop
  float cell_dv;                       // largest voltage difference between Cells [V] - for LCD & debug purposes

  float Vbck_actual;                   // actual voltage at DC/DC buck node [V]
  float Vbck_target;                   // target voltage at DC/DC buck node [V]
  float Ibck_actual;                   // actual current at DC/DC buck node [A]

  float Vbst_target;                   // actual voltage at DC/DC boost node [V]
  float Vbst_actual;                   // target voltage at DC/DC boost node [V]
  float Ibst_actual;                   // actual current at DC/DC boost node [A]

  uint32_t cell_hi;                    // cell number of Cell with highest clamp voltage 
  uint32_t cell_lo;                    // cell number of Cell with lowest clamp voltage  

  uint32_t pwm_ccr;                    // current DC/DC PWM Timer CCR register value 

  uint32_t dcdc_err;                   // DC/DC converter overcurrent/overvoltage error counter
} bms_t;

// -- extern used in 'gui_update ()' & 'pass_test ()'
extern bms_t bms;

// -- protoypes
void init_bms (void);

void bms_reset (void);

void bms_update (void);

uint32_t bms_buck_mode  (int from_cell, int to_cell);
uint32_t bms_boost_mode (int from_cell, int to_cell);

void bms_enable  (void);
void bms_disable (void);

void bms_pass_run (void);

uint32_t bms_cell_lowest  (float *vc);
uint32_t bms_cell_highest (float *vc);

uint32_t bms_check_dcdc (void);
uint32_t bms_check_pack (void);

void bms_pass_block (void);

uint32_t bms_cell_lo (void);
uint32_t bms_cell_hi (void);

uint32_t bms_get_ctl     (void);
float    bms_get_dv      (void);
uint32_t bms_get_dcdcerr (void);

void bms_dump (void);

#endif

