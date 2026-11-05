// file    : adc.c
// author  : rb
// purpose : STM32G4xx ARM ADC routines
// date    : 171130
// last    : 261008
//    
// note    : the KP184 e-load introduces excessive signal noise 
// 
// note    : ADC1 samples shortly after the PWM rising edge that drives the low-side
//           DC/DC FET, preventing hot-node ringing from coupling into the ADC.
//      
//           chain: - Timer 1 generates the PWM signal
//                  - Timer 1 generates an interrupt on each Update Event
//                  - in Timer 1 ISR, Timer 2 is started in one-shot mode once
//                    every N PWM cycles, yielding a 1 kSPS ADC1 sampling rate
//                  - Timer 2 generates a TRGO signal on its Update Event
//                  - ADC1 is triggered by the Timer 2 TRGO signal, and DMA1 is started
// -----------------------------------------------------------------------------
// ADC1 sampling chain
//
// TIM1: 100 kHz DC/DC PWM
//   └─ Update Event → ISR
//       └─ every 100 PWM cycles → start TIM2
//
// TIM2: one-shot sampling delay
//   └─ Update Event → TRGO
//       └─ ADC1 external trigger
//
// ADC1: N-channel regular scan
//   └─ DMA1 Channel 1 → adc_dma[]
//       └─ Transfer Complete → adc_new = 1
// -----------------------------------------------------------------------------
//
// note : <> check this on STM32G4: ADC reading of current sensor for 'I_HI' 
//            non-linear between 0V2..0V4 (WTF-jes??)

#include "includes.h"

// globals
         float VDDA;                             // actual board voltage [mV]
         int adc_log = 1;                        // log flag
volatile int adc_new = 0;                        // flag set in ISR, signals new raw ADC data

volatile uint16_t adc_dma[ADC_NUM_RCHANNELS];    // ADC raw data, filled via DMA

// ADC data structs                              
adc_t vcl1 = { 0, 5.4174, 0, 0, 1};               // battery cell 1 voltage
adc_t vcl2 = { 0, 5.4140, 0, 0, 1};               // battery cell 1..2 voltage
adc_t vcl3 = { 0, 5.4009, 0, 0, 1};               // battery cell 1..3 voltage
adc_t vcl4 = { 0, 5.4231, 0, 0, 1};               // battery cell 1..4 voltage

adc_t vpck = { 0, 5.4057, 0, 0, 1};               // battery pack voltage
adc_t ipck = {15, 0.9523, 0, 0, 1};               // <> todo battery pack current
adc_t vcrg = { 0, 5.4249, 0, 0, 1};               // charger input voltage

adc_t vbst = { 0, 5.2935, 0, 0, 1};               // DC/DC boost node voltage
adc_t ibck = {13, 0.5000, 0, 0, 1};               // DC/DC current in buck mode

adc_t vbck = { 0, 5.3870, 0, 0, 1};               // DC/DC buck node voltage
adc_t ibst = {15, 0.5000, 0, 0, 1};               // DC/DC current in boost mode

adc_t vref = { 0, 1.0000, 0, 0, 1};               // refint - internal referemce voltage

// moving average filter structs                 
maf_t vp;                                        // moving average filter Vpck
maf_t vc;                                        // moving average filter Vcharge
maf_t ik;                                        // moving average filter Ibuck
maf_t it;                                        // moving average filter Iboost
maf_t ip;                                        // moving average filter Ipack
maf_t c1;                                        // moving average filter Vcell1
maf_t c2;                                        // moving average filter Vcell2
maf_t c3;                                        // moving average filter Vcell3
maf_t c4;                                        // moving average filter Vcell4
maf_t vt;                                        // moving average filter Vboost
maf_t vk;                                        // moving average filter Vbuck
maf_t vr;                                        // moving average filter Vrefint

// DMA1 channel ISR for ADC1
void DMA1_Channel1_IRQHandler (void)
{
  // check for transfer complete interrupt on DMA1 Channel 1
  if ((DMA1->ISR & DMA_ISR_TCIF1) != 0U)
  {
    // clear transfer complete flag for DMA1 Channel 1
    DMA1->IFCR = DMA_IFCR_CTCIF1;

    // signal end of ADC1 data transfer,, LED turned on in Timer 2 ISR - dev only, for measuring timing DMA 
  //led_grn_off (); 

    // signal new ADC data
    adc_new = 1;
  }

  // clear all other DMA1 Channel 1 interrupt flags
  DMA1->IFCR = DMA_IFCR_CGIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1;
}

// init ADC
void init_adc (void)
{
  // setup DMA1 Channel 1
  adc1_dma_init ();

  // setup ADC1
  adc1_init (); 
  
  // log ADC data to terminal
  adc_log_on ();
}

// setup DMA1 channel
void adc1_dma_init (void)
{
  // enable DMA1 & DMAMUX1 clock
  RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN 
               |  RCC_AHB1ENR_DMAMUX1EN;

  // disable DMA1 Channel 1 before programming channel registers
  DMA1_Channel1->CCR &= ~DMA_CCR_EN;

  // DMAMUX1 Channel 0 feeds DMA1 Channel 1 -- note: DMA1 must be disabled to write this register!!
  DMAMUX1_Channel0->CCR = DMA_REQUEST_ADC1;  // request ID 5: ADC1

  // peripheral source address is ADC1 regular data register
  DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;

  // memory destination is ADC DMA buffer base address
  DMA1_Channel1->CMAR = (uint32_t)&adc_dma[0];

  // number of transferred words equals number of regular channels
  DMA1_Channel1->CNDTR = (uint32_t)ADC_NUM_RCHANNELS;

  // clear pending DMA1 Channel 1 interrupt flags before enabling
  DMA1->IFCR = DMA_IFCR_CGIF1 
             | DMA_IFCR_CTCIF1 
             | DMA_IFCR_CHTIF1 
             | DMA_IFCR_CTEIF1;

  // configure DMA1 Channel 1
  DMA1_Channel1->CCR = DMA_CCR_PL_1      // high priority 
                     | DMA_CCR_MSIZE_0   // memory = 16 bit 
                     | DMA_CCR_PSIZE_0   // peripheral = 16 bit 
                     | DMA_CCR_MINC      // increment memory 
                     | DMA_CCR_CIRC      // circular 
                     | DMA_CCR_TCIE;     // enable transfer complete interrupt

  // enable NVIC line for DMA1 Channel 1 transfer complete interrupt
  NVIC_SetPriority (DMA1_Channel1_IRQn, 2U);
  NVIC_EnableIRQ   (DMA1_Channel1_IRQn);

  // enable DMA1 Channel 1 after ADC and sequence are fully configured
  DMA1_Channel1->CCR |= DMA_CCR_EN;
}

// setup ADC1
void adc1_init (void)
{
  // enable ADC12 clock
  RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;

  // select ADC12 kernel clock 
  RCC->CCIPR &= ~RCC_CCIPR_ADC12SEL;              // clear ADC12 clock source selection bits
  RCC->CCIPR |= (0b10 << RCC_CCIPR_ADC12SEL_Pos); // 0b10 = SYSCLK

  // set ADC clock mode
  ADC12_COMMON->CCR &= ~ADC_CCR_CKMODE; // 0b00 = asynchronous clock mode
  
  // if enabled, disable ADC before calibration sequence
  if ((ADC1->CR & ADC_CR_ADEN) != 0U)
  {
    // stop ongoing regular conversion before disabling ADC
    if ((ADC1->CR & ADC_CR_ADSTART) != 0U)
    {
      ADC1->CR |= ADC_CR_ADSTP;

      while ((ADC1->CR & ADC_CR_ADSTP) != 0U)
        ;
    }
  
    // disable ADC
    ADC1->CR |= ADC_CR_ADDIS;
  
    // & wait ADC disabled
    while ((ADC1->CR & ADC_CR_ADEN) != 0U)
      ;
  }

  // exit deep-power-down mode before any ADC configuration
  ADC1->CR &= ~ADC_CR_DEEPPWD;

  // enable ADC voltage regulator before calibration/enable
  ADC1->CR |= ADC_CR_ADVREGEN;

  // wait for regulator startup (~50us at 100Mc)
  for (volatile uint32_t i = 0U; i < 500U; i++)
    ;

  // start single-ended ADC calibration
  ADC1->CR &= ~ADC_CR_ADCALDIF;
  ADC1->CR |= ADC_CR_ADCAL;

  // & wait calibration ready
  while ((ADC1->CR & ADC_CR_ADCAL) != 0U)
    ;

  // set asynchronous ADC clock prescaler
  ADC12_COMMON->CCR &= ~ADC_CCR_PRESC;                // clear prescaler bits
  ADC12_COMMON->CCR |= (0b0001 << ADC_CCR_PRESC_Pos); // ADC_CLK = SYSCLK/2 = 50Mc

  // enable internal Vrefint channel path
  ADC12_COMMON->CCR |= ADC_CCR_VREFEN;

  // set ADC1 sample time
  // * channel sample times:
  // 0b000:   2.5 ADC clock cycles
  // 0b001:   6.5 ADC clock cycles
  // 0b010:  12.5 ADC clock cycles
  // 0b011:  24.5 ADC clock cycles
  // 0b100:  47.5 ADC clock cycles
  // 0b101:  92.5 ADC clock cycles
  // 0b110: 247.5 ADC clock cycles
  // 0b111: 640.5 ADC clock cycles
  ADC1->SMPR1 = (0b000 <<  0) |  // Channel 10  -- NU
                (0b010 <<  3) |  // Channel 11  -- ADC_CHAN_VCHARGE
                (0b010 <<  6) |  // Channel 12  -- ADC_CHAN_VBUCK  
                (0b000 <<  9) |  // Channel 13  -- NU
                (0b010 << 12) |  // Channel 14  -- ADC_CHAN_IPACK 
                (0b010 << 15) |  // Channel 15  -- ADC_CHAN_VBOOST
                (0b000 << 18) |  // Channel 16  -- NU
                (0b000 << 21) |  // Channel 17  -- NU
                (0b111 << 24) |  // Channel 18  -- Vref            - long sampling
                (0b000 << 27);   // Channel 19  -- NU

  ADC1->SMPR2 = (0b000 <<  0) |  // channel  0  -- NU
                (0b111 <<  3) |  // channel  1  -- ADC_CHAN_VCELL4 - long sampling
                (0b111 <<  6) |  // channel  2  -- ADC_CHAN_VCELL3 - long sampling
                (0b111 <<  9) |  // channel  3  -- ADC_CHAN_VCELL2 - long sampling
                (0b111 << 12) |  // channel  4  -- ADC_CHAN_VCELL1 - long sampling
                (0b000 << 15) |  // channel  5  -- NU
                (0b000 << 18) |  // channel  6  -- NU
                (0b010 << 21) |  // channel  7  -- ADC_CHAN_I_HI  
                (0b010 << 24) |  // channel  8  -- ADC_CHAN_I_LO  
                (0b010 << 27);   // channel  9  -- ADC_CHAN_VCELLS

  // setup scan sequence length & scan order regular channels 
  ADC1->SQR1 = ((uint32_t)(ADC_NUM_RCHANNELS - 1U) << ADC_SQR1_L_Pos) 
             | ((uint32_t)ADC_CHAN_VCELL4          << ADC_SQR1_SQ1_Pos)     
             | ((uint32_t)ADC_CHAN_VCELL3          << ADC_SQR1_SQ2_Pos)     
             | ((uint32_t)ADC_CHAN_VCELL2          << ADC_SQR1_SQ3_Pos)       
             | ((uint32_t)ADC_CHAN_VCELL1          << ADC_SQR1_SQ4_Pos);      

  ADC1->SQR2 = ((uint32_t)ADC_CHAN_I_HI            << ADC_SQR2_SQ5_Pos)   
             | ((uint32_t)ADC_CHAN_I_LO            << ADC_SQR2_SQ6_Pos)   
             | ((uint32_t)ADC_CHAN_VCELLS          << ADC_SQR2_SQ7_Pos) 
             | ((uint32_t)ADC_CHAN_VCHARGE         << ADC_SQR2_SQ8_Pos) 
             | ((uint32_t)ADC_CHAN_VBUCK           << ADC_SQR2_SQ9_Pos); 

  ADC1->SQR3 = ((uint32_t)ADC_CHAN_IPACK           << ADC_SQR3_SQ10_Pos)
             | ((uint32_t)ADC_CHAN_VBOOST          << ADC_SQR3_SQ11_Pos)
             | ((uint32_t)ADC_CHAN_VREF            << ADC_SQR3_SQ12_Pos);

  // setup external trigger, circular DMA requests, overwrite on overrun
  ADC1->CFGR = (0b01011 << ADC_CFGR_EXTSEL_Pos) // External Trigger: event 11
             | (0b01    << ADC_CFGR_EXTEN_Pos)  // External Trigger: rising edge
             | ADC_CFGR_DMAEN                   // enable DMA
             | ADC_CFGR_DMACFG                  // DMA circular mode
             | ADC_CFGR_OVRMOD;                 // overwrite data on overrun error
  
  // clear ADC interrupt flags 
  ADC1->ISR = 0x3ff;
  
  // enable ADC
  ADC1->CR |= ADC_CR_ADEN;
  
  // & wait for enable
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0U)
    ;
 
  // enable conversion, ADC1 waits for external trigger (= Timer 2 TRGO)
  ADC1->CR |= ADC_CR_ADSTART;
 }

					
// read new ADC data & filter - <> todo: time calcualtion (about 200us?)
void adc_update (void)
{
  // check ADC1 data ready - set in DMA1 ISR
  if (!adc_new)
    return;

  // copy new ADC data
  vcl4.raw = (uint32_t)adc_dma[0];  // clamp voltage battery Cell4
  vcl3.raw = (uint32_t)adc_dma[1];  // clamp voltage battery Cell3
  vcl2.raw = (uint32_t)adc_dma[2];  // clamp voltage battery Cell2
  vcl1.raw = (uint32_t)adc_dma[3];  // clamp voltage battery Cell1
  ibst.raw = (uint32_t)adc_dma[4];  // DC/DC current in buck mode
  ibck.raw = (uint32_t)adc_dma[5];  // DC/DC current in boost mode
  vpck.raw = (uint32_t)adc_dma[6];  // battery pack voltage
  vcrg.raw = (uint32_t)adc_dma[7];  // charger input voltage
  vbck.raw = (uint32_t)adc_dma[8];  // DC/DC voltage in buck mode
  ipck.raw = (uint32_t)adc_dma[9];  // current battery pack
  vbst.raw = (uint32_t)adc_dma[10]; // DC/DC voltage in boost mode
  vref.raw = (uint32_t)adc_dma[11]; // Vrefint

  // average battery cell voltages
  vcl1.avg = moving_average (&c1, &vcl1);
  vcl2.avg = moving_average (&c2, &vcl2);
  vcl3.avg = moving_average (&c3, &vcl3);
  vcl4.avg = moving_average (&c4, &vcl4);

  // average total battery pack voltage
  vpck.avg = moving_average (&vp, &vpck);

  // average charger voltage
  vcrg.avg = moving_average (&vc, &vcrg);

  // average output voltage buckt converter
  vbck.avg = moving_average (&vk, &vbck);

  // average battery pack current
  ipck.avg = moving_average (&ip, &ipck);
  
  // average output voltage boost converter
  vbst.avg = moving_average (&vt, &vbst);
  
  // average output current buck converter
  ibck.avg = moving_average (&ik, &ibck); 

  // average output current boost converter
  ibst.avg = moving_average (&it, &ibst);

  // average Vrefint
  vref.avg = moving_average (&vr, &vref);

  // update actual board VDDA voltage - used for ADC-to-voltage calculations
  VDDA = (VREFINT_CAL_VDDA * VREFINT_CAL_DAT) / vref.avg;
 
  // reset flag
  adc_new = 0;
}

// read voltage battery pack [V]
float adc_read_vpck (void)
{
  // hand tweaked, offset error subtracted
  return ((vpck.fac * (vpck.avg - vpck.ofs) * VDDA)/ ADC_FULLRANGE);
}

// read cuurent battery pack [V]
float adc_read_ipck (void)
{
  // hand tweaked, offset error subtracted<> redo measurement
  float tmp = ((ipck.fac * (ipck.avg - ipck.ofs) * VDDA)/ ADC_FULLRANGE);

  return ((ipck_sign() ? 1 : -1) * tmp);
}

// read voltage charger [V] 
float adc_read_vcrg (void)
{
  // hand tweaked, offset error subtracted<> redo measurement
  return ((vcrg.fac * (vcrg.avg - vcrg.ofs) * VDDA)/ ADC_FULLRANGE);
}


// read buck mode current [A]
float adc_read_ibuck (void)
{
  float tmp = ((ibck.fac * ibck.avg * VDDA) / ADC_FULLRANGE);

  return ((ibck_sign() ? 1 : -1) * tmp);
}

// read boost mode current [A]
float adc_read_iboost (void)
{
  float tmp = ((ibst.fac * ibst.avg * VDDA) / ADC_FULLRANGE);

  return ((ibst_sign() ? 1 : -1) * tmp);
}



// read battery Cell 1 voltage [V]
float adc_read_vstack1 (void)
{
  float tmp = ((vcl1.fac * (vcl1.avg - vcl1.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read battery Cell + Cell 2 voltage [V]
float adc_read_vstack2 (void)
{
  float tmp = ((vcl2.fac * (vcl2.avg - vcl2.ofs) * VDDA) / ADC_FULLRANGE); 

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read battery Cell + Cell 2 + C3ll 3 voltage [V]
float adc_read_vstack3 (void)
{
  float tmp = ((vcl3.fac * (vcl3.avg - vcl3.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read battery Cell + Cell 2 + C3ll 3 + Cell 4 voltage [V]
float adc_read_vstack4 (void)
{
  float tmp = ((vcl4.fac * (vcl4.avg - vcl4.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read Cell stack voltage
float adc_read_vstack (uint32_t num)
{
  switch (num)
  {
    case 0:
      return (adc_read_vstack1 ());
    
    case 1:
      return (adc_read_vstack2 ());
    
    case 2:
      return (adc_read_vstack3 ());
    
    case 3:
      return (adc_read_vstack4 ());
    
    default:
      return (0.0f);
  }
}

// read battery Cell 1 clamp voltage [V]
float adc_read_vcell1 (void)
{
  float tmp = adc_read_vstack1 ();

  return (tmp);
}

// read battery Cell 2 clamp voltage [V]
float adc_read_vcell2 (void)
{
  float tmp1 = adc_read_vstack1 ();
  float tmp2 = adc_read_vstack2 ();

  return (tmp2 - tmp1);
}

// read battery Cell 3 clamp voltage [V]
float adc_read_vcell3 (void)
{
  float tmp1 = adc_read_vstack2 ();
  float tmp2 = adc_read_vstack3 ();

  return (tmp2 - tmp1);
}

// read battery Cell 3 clamp voltage [V]
float adc_read_vcell4 (void)
{
  float tmp1 = adc_read_vstack3 ();
  float tmp2 = adc_read_vstack4 ();

  return (tmp2 - tmp1);
}

// read DC/DC boost node voltage [V]
float adc_read_vboost (void)
{
  // hand tweaked, offset error subtracted<> redo measurement
  float tmp = ((vbst.fac * (vbst.avg - vbst.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read DC/DC buck node voltage [V]
float adc_read_vbuck (void)
{
  // hand tweaked, offset error subtracted<> redo measurement
  float tmp = ((vbck.fac * (vbck.avg - vbck.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read Vrefint, internal reference voltage [V]
float adc_read_vrefint (void)
{
  // hand tweaked, offset error subtracted<> redo measurement
  float tmp = ((vref.fac * (vref.avg - vref.ofs) * VDDA) / ADC_FULLRANGE);

  return ((tmp < 0.0) ? 0.0 : tmp);
}

// read VDDA, actual board voltage [V]
float adc_read_vdda (void)
{
  return (VDDA);
}

// start logging ADC data to serial port
void adc_log_on (void)
{
  adc_log = 1;
}

// stop logging ADC data to serial port
void adc_log_off (void)
{
  adc_log = 0;
}

// dump board voltages/currents
void adc_dump (void)
{
  if (!adc_log)
    return;

  printf ("VC: ");           
  printf ("%2.4f ", adc_read_vcell1 ());                    
  printf ("%2.4f ", adc_read_vcell2 ());                    
  printf ("%2.4f ", adc_read_vcell3 ());                    
  printf ("%2.4f ", adc_read_vcell4 ());                    

  //// voltage at DC/DC buck node
  printf ("Vbck: ");
  printf ("%.2f ", adc_read_vbuck ());

  // current in DC/DC buck node
  printf ("Ibck: ");
  printf ("%.2f ", adc_read_ibuck ());

  // voltage at DC/DC boost node
  printf ("Vbst: ");
  printf ("%.2f ", adc_read_vboost ());

  // current in DC/DC boost node
  printf ("Ibst: ");
  printf ("%.2f ", adc_read_iboost ());

  printf ("Vpck: %2.2f ",  adc_read_vpck ());                    
  printf ("Ipck: %2.2f ",  adc_read_ipck ());                    
  printf ("Vcrg: %2.2f ",  adc_read_vcrg ());                    
  
  printf (" - ");

  printf ("DCDC_err: %d ", bms_get_dcdcerr ()); 

  printf (" - ");

  printf ("dV: %d mV ", (uint32_t)(bms_get_dv () * 1000));

  printf ("BMS: %d ", bms_get_ctl ());

  if (bms_get_ctl () == BMS_CONTROL_BOOST)
  {
    printf ("hi: %d ", bms_cell_hi () + 1);
    printf ("lo: %d ", bms_cell_lo () + 1);
    printf ("boost mode");
  }
  
  if (bms_get_ctl () == BMS_CONTROL_BUCK)
  {
    printf ("hi: %d ", bms_cell_hi () + 1);
    printf ("lo: %d ", bms_cell_lo () + 1);
    printf ("buck mode");
  }
  
  if (bms_get_ctl () == BMS_CONTROL_IDLE)
  {
    printf ("cell relax ");
  }
  
  printf ("\n");
}




// dump ADC raw data - dev only
void adc_dump_raw (void)
{
//// battery Cell voltages
//printf ("VCell: ");
//printf ("%d ", vcl1.raw);
//printf ("%d ", vcl2.raw);
//printf ("%d ", vcl3.raw);
//printf ("%d ", vcl4.raw);
//
//// battery pack voltage (== VCell4)
//printf ("Vpck: ");
//printf ("%d ", vpck.raw);
//
//// battery pack charge/discharge current
  printf ("Ipck: ");
  printf ("%d ", ipck.raw - ipck.ofs);
  printf ("%.3f ", ipck.avg);

//
//// charger voltage
//printf ("Vcrg: ");
//printf ("%d ", vcrg.raw);
//
//// voltage at DC/DC buck node
//printf ("Vbck: ");
//printf ("%d ", vbck.raw);

  // current in DC/DC buck node
  printf ("Ibck: ");
  printf ("%d ", ibck.raw - ibck.ofs);

//// voltage at DC/DC boost node
//printf ("Vbst: ");
//printf ("%d ", vbst.raw);

  // current in DC/DC boost node
  printf ("Ibst: ");
  printf ("%d ", ibst.raw - ibst.ofs);

  // current in DC/DC buck node
  printf ("Vrefint: ");
  printf ("%d ", vref.raw);

  printf ("VDDA: ");
  printf ("%.3f ", VDDA);

  printf ("\n");
}




// dump ADC registers -- dev only
void adc_dump_registers (void)
{
  // ADC1 registers
  printf ("-- ADC registers --\n");
  printf ("ADC1_ISR             : %08lx\n", ADC1->ISR);
  printf ("ADC1_IER             : %08lx\n", ADC1->IER);
  printf ("ADC1_CR              : %08lx\n", ADC1->CR);
  printf ("ADC1_CFGR            : %08lx\n", ADC1->CFGR);
  printf ("ADC1_CFGR2           : %08lx\n", ADC1->CFGR2);
  printf ("ADC1_SMPR1           : %08lx\n", ADC1->SMPR1);
  printf ("ADC1_SMPR2           : %08lx\n", ADC1->SMPR2);
  printf ("ADC1_SQR1            : %08lx\n", ADC1->SQR1);
  printf ("ADC1_SQR2            : %08lx\n", ADC1->SQR2);
  printf ("ADC1_SQR3            : %08lx\n", ADC1->SQR3);
  printf ("ADC1_DR              : %08lx\n", ADC1->DR);
  printf ("ADC1_CALFACT         : %08lx\n", ADC1->CALFACT);
  printf ("ADC12_COMMON->CCR    : %08lx\n", ADC12_COMMON->CCR);
  
  // voltage calibration 
  printf ("\n-- internal reference & voltages --\n");
  printf ("VREFINT_CAL_DAT      : %d [-]\n"    , VREFINT_CAL_DAT);
  printf ("VREFINT_CAL_VDDA     : %d\n"        , VREFINT_CAL_VDDA);
  printf ("Vrefint              : %3.3f [-]\n" , vref.raw);
  printf ("VDDA                 : %3.3f [mV]\n", VDDA);

  // DMA1 registers
  printf ("\n-- DMA1 registers --\n");
  printf ("DMA1_Channel1->CNDTR : 0x%08lx\n", DMA1_Channel1->CNDTR);
  printf ("%02lx\n", DMA1_Channel1->CNDTR);
  printf ("DMA1_Channel1->CCR   : 0x%08lx\n", DMA1_Channel1->CCR);
  printf ("ADC1->CFGR           : 0x%08lx\n", ADC1->CFGR);
  
  // DMAMUX registers
  printf ("\n-- DMAMUX registers --\n");
  printf ("DMAMUX1_Channel0->CCR: 0x%08lx\n", DMAMUX1_Channel0->CCR);
  printf ("RCC->AHB1ENR         : 0x%08lx\n", RCC->AHB1ENR);
}



// ---------------- FILTER ROUTINES ----------------

// moving average filter
float moving_average (maf_t *maf, adc_t *adc)
{
  // clip data 
  int32_t dat = (adc->raw > adc->ofs) ? (adc->raw - adc->ofs) : 0;

  // speed up filter & prevent start-up BS values
  if (adc->first)
  {
    for (int i = 0; i < ADC_BUF_SIZE; i++)
      maf->buf[i] = dat;
  
    adc->first = 0;
  }
  else
  {
    // write new value to ring buffer
    maf->buf[maf->ptr++ % ADC_BUF_SIZE] = dat;
  }

  // calculate sum of all buffer entries  
  int32_t sum = 0;

  for (int i = 0; i < ADC_BUF_SIZE; i++)
  {
    sum += maf->buf[i];
  }
  
  // dev only
  if (adc->first == 2)
    printf ("first: %d adc.raw: %ld sum: %ld\n", adc->first, adc->raw, sum);

  // return mean value
  return ((float)sum/ADC_BUF_SIZE);
}

// swap two array elements for median filter
inline void median_swap (uint16_t *a, uint16_t *b)
{
  uint32_t temp = *a;
  *a = *b;
  *b = temp;
}

// median filter (odd number of array elements only please)
float median_filter (mef_t *med, const int16_t dat, const int first) 
//float median_filter (mef_t *med, adc_t *adc)  // <> todo 
{
  uint16_t tmp[ADC_MEDIAN_DEPTH];

  // speed up filter & prevent start-up BS values
  if (first)
  {
    for (int i = 0; i < ADC_MEDIAN_DEPTH; i++)
      med->buf[med->ptr] = dat;      
  }
  else
  {
    // write new value to ring buffer
    med->buf[med->ptr] = dat;      
  }

  // don't use 'med->ptr++ % ADC_MEDIAN_DEPTH' as buffer size is not a power of 2
  if (med->ptr++ >= ADC_MEDIAN_DEPTH-1)
    med->ptr = 0;

  // copy buffer to tmp
  for (int i = 0; i < ADC_MEDIAN_DEPTH; i++)
    tmp[i] = med->buf[i];    

  // sort tmp buffer
  for (int i = 0; i < ADC_MEDIAN_DEPTH-1; i++) 
  {
    for (int j = 0; j < ADC_MEDIAN_DEPTH-i-1; j++) 
    {
      if (tmp[j] > tmp[j+1])
      {
        median_swap (&tmp[j], &tmp[j+1]);
      }
    }
  }

  // return result
  return ((float)tmp[(ADC_MEDIAN_DEPTH-1)/2]);
}

