// file    : bms_test.h
// author  : rb
// purpose : header file for bms_test.c
// date    : 261010
// last    : 261011
//

#ifndef _BMS_TEST_H_
#define _BMS_TEST_H_

// -- defines 
#define BMS_PASS_NUM_LOWER                    (3U) // index lower pass-FETs
#define BMS_PASS_NUM_UPPER                    (6U) // index upper pass-FETs
#define BMS_PASS_DELAY                        (5U) // Cell stake voltage sample delay                      
#define BMS_CELL_TOLERANCE                  (0.3f) // maximum allowed difference stack and DC/DC nodes [V]


// -- for pass-FET test FSM
enum pass_states
{
  BMS_PASS_START = 0    ,
  BMS_PASS_SELECT       ,
  BMS_PASS_SELECT_LOWER ,
  BMS_PASS_SAMPLE_LOWER ,
  BMS_PASS_COLLECT_LOWER,
  BMS_PASS_SELECT_UPPER ,
  BMS_PASS_SAMPLE_UPPER ,
  BMS_PASS_COLLECT_UPPER,
  BMS_PASS_RESULT       ,
  BMS_PASS_DONE         ,   
};


// -- protoypes
uint32_t bms_pass_test (void);
uint32_t bms_pass_eval (void);

void bms_cycle_enable  (void);
void bms_cycle_disable (void);
void bms_cycle_test    (void);


#endif




