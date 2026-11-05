// file    : adc.h
// author  : rb
// purpose : header file for adc.c
// note    : adapted for STM32F4xx, has no ADC calibration register - do not use for STM32L15x
// date    : 171130
// last    : 260911
//

#ifndef _ADC_H_
#define _ADC_H_

// -- defines 
#define VREFINT_CAL_ADR ((uint16_t *)0x1fff75AAUL) // absolute address VREFINT calibration data
#define VREFINT_CAL_DAT        *(VREFINT_CAL_ADR)  // VREFINT calibration data
#define VREFINT_CAL_VDDA                   (3.000) // VDDA used during VREFINT calibration [V]
#define ADC_FULLRANGE                       4095U  // 12-bit ADC
#define ADC_NUM_REFSAMPLES                    64U  // number of samples to average Vrefint

#define ADC_NUM_RCHANNELS                     12U  // number of ADC regular channels in use
#define ADC_NUM_JCHANNELS                      0U  // number of ADC injected channels in use

//#define ADC_BUF_SIZE                         2U  // buffer size for moving average filter - buffer size must be power of 2!
//#define ADC_BUF_SIZE                        64U  // buffer size for moving average filter - buffer size must be power of 2!
#define ADC_BUF_SIZE                         128U  // buffer size for moving average filter - buffer size must be power of 2!
//#define ADC_BUF_SIZE                       256U  // buffer size for moving average filter - buffer size must be power of 2!

#define ADC_MEDIAN_DEPTH                      15U  // number elements in median filter (odd numbers only please)

#define ADC_CHAN_VCELL4                        1U  // voltage cell4
#define ADC_CHAN_VCELL3                        2U  // voltage cell3
#define ADC_CHAN_VCELL2                        3U  // voltage cell2
#define ADC_CHAN_VCELL1                        4U  // voltage cell1
#define ADC_CHAN_I_HI                          7U  // current in boost path
#define ADC_CHAN_I_LO                          8U  // current in buck path
#define ADC_CHAN_VCELLS                        9U  // voltage all cells 
#define ADC_CHAN_VCHARGE                      11U  // charger input voltage
#define ADC_CHAN_VBUCK                        12U  // voltage DC/DC buck node
#define ADC_CHAN_IPACK                        14U  // current from/to battery pack
#define ADC_CHAN_VBOOST                       15U  // voltage DC/DC boost node
#define ADC_CHAN_VREF                         18U  // ADC channel Vrefint

#define ADC_POS_I_LO                           0U  // ??????ADC scan position I_LO     - register ADC1->SQR3
#define ADC_POS_I_HI                           1U  // ??????ADC scan position I_HI     - register ADC1->SQR3
#define ADC_POS_VCELL1                         2U  // ??????ADC scan position VCELL1   - register ADC1->SQR3
#define ADC_POS_VCELL2                         3U  // ??????ADC scan position VCELL2   - register ADC1->SQR3
#define ADC_POS_VCELL3                         4U  // ??????ADC scan position VCELL3   - register ADC1->SQR3
#define ADC_POS_VCELL4                         5U  // ??????ADC scan position VCELL4   - register ADC1->SQR3
#define ADC_POS_VCELLS                         0U  // ??????ADC scan position VCELLS   - register ADC1->SQR2
#define ADC_POS_VCHARGE                        1U  // ??????ADC scan position VCHARGE  - register ADC1->SQR2
#define ADC_POS_VBOOST                         2U  // ??????ADC scan position VBOOST   - register ADC1->SQR2
#define ADC_POS_VBUCK                          3U  // ??????ADC scan position VBUCK    - register ADC1->SQR2

// -- externs
extern volatile int adc_new;           // flag set in Timer 1 ISR, signals new ras ADC data
extern int ibck_raw;                   // DC/DC current in buck mode
extern float ibck_avg;                 // DC/DC current in buck mode

// -- structs 

// ADC data struct
typedef struct
{
  int32_t  ofs;                        // offset error compensation
  float    fac;                        // multiplication factor ADC data - measured with Keysight U1251B
  uint32_t raw;                        // raw ADC data
  float    avg;                        // average filtered ADC data
  uint32_t first;                      // flag used by averaging filter
} adc_t;

// -- moving average filter data structure
typedef struct 
{
  uint32_t ptr;
  int16_t  buf[ADC_BUF_SIZE];
} maf_t;

// -- median filter 
typedef struct 
{
  uint32_t ptr;
  int32_t  buf[ADC_MEDIAN_DEPTH];
} mef_t;


// -- protoypes
void init_adc (void);

void adc1_dma_init (void);

void adc1_init (void);

void adc_update (void);

uint16_t adc_read_channel (const uint8_t chan);

float adc_read_vref (void);
float adc_read_vpck (void);
float adc_read_vcrg (void);

float adc_read_ipck (void);

float adc_read_vstack1 (void);
float adc_read_vstack2 (void);
float adc_read_vstack3 (void);
float adc_read_vstack4 (void);

float adc_read_vstack (uint32_t num);

float adc_read_vcell1 (void);
float adc_read_vcell2 (void);
float adc_read_vcell3 (void);
float adc_read_vcell4 (void);

float adc_read_vbuck  (void);
float adc_read_vboost (void);

float adc_read_ibuck  (void);
float adc_read_iboost (void);

float adc_read_vrefint (void);
float adc_read_vdda    (void);

void adc_log_on  (void);
void adc_log_off (void);

float moving_average (maf_t *maf, adc_t *adc);
      
float median_filter  (mef_t *med, const int16_t dat, const int first);

void adc_dump_raw (void);
void adc_dump (void);
void adc_dump_registers (void);

#endif
