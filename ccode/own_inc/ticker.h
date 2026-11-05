// file    : ticker.h
// author  : rb
// purpose : header file ticker.c
// date    : 170906
// last    : 261017

#ifndef __TIMER_H__
#define __TIMER_H__

// -- globals
extern volatile uint8_t csec_elapsed;
extern volatile uint8_t dsec_elapsed;
extern volatile uint8_t sec_elapsed;

// -- defines 
#define TIMERCLK                      (APB1CLK*1U) // 100Mc 
#define TIMER_1HZ       (TIMERCLK/1U         - 1U)
#define TIMER_1KHZ      (TIMERCLK/1000U      - 1U)
#define TIMER_10KHZ     (TIMERCLK/10000U     - 1U)
#define TIMER_100KHZ    (TIMERCLK/100000UL   - 1U)
#define TIMER_1MHZ      (TIMERCLK/1000000UL  - 1U)
#define TIMER_10MHZ     (TIMERCLK/10000000UL - 1U)
#define TIMER_20MHZ     (TIMERCLK/20000000UL - 1U)
#define TIMER_40MHZ     (TIMERCLK/40000000UL - 1U)
#define TIMER_48MHZ     (TIMERCLK/48000000UL - 1U)
#define TIMER_50MHZ     (TIMERCLK/50000000UL - 1U)
#define TIMER_100MHZ                          (0U)

#define PWM_30KHZ              (TIMERCLK/30000UL ) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_40KHZ              (TIMERCLK/40000UL ) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_50KHZ              (TIMERCLK/50000UL ) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_60KHZ              (TIMERCLK/60000UL ) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_70KHZ              (TIMERCLK/70000UL ) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_100KHZ             (TIMERCLK/100000UL) // PWM frequency w. Timer clock is TIMERCLK (100Mc)
#define PWM_200KHZ             (TIMERCLK/200000UL) // PWM frequency w. Timer clock is TIMERCLK (100Mc)

#define DCDC_PWMMIN                         (50U)  // minimum dutycyle DC/DC PWM signal [%*10]
#define DCDC_PWMRESET                      (500U)  // reset dutycyle DC/DC PWM signal [%*10]
#define DCDC_PWMMAX                        (980U)  // maximum dutycyle DC/DC PWM signal [%*10]

// note: in buck mode : increasing Timer PWM CCR value gives higher output
// note: in boost mode: decreasing Timer PWM CCR value gives higher output
// note: PWM range 0..1000 (0..100%)
#define DCDC_PWM_BOOSTSAFE                 (950U)  // safe PWM value for DC/DC in boost mode - lowest Vout in boost mode 
#define DCDC_PWM_BUCKSAFE                  (150U)  // safe PWM value for DC/DC in buck mode  - lowest Vout in buck mode 

// ARM core SysTick defines for 1 ms ticker
#define SYSTICK_FREQ                      (1000U)  // number of int's / sec
#define SYSTICK_TWEAK                        (0U)  // tweak value to remove RC oscilator error

// -- prototypes
void init_ticker (void);

void usleep (int32_t usecs);
void msleep (uint32_t msecs);

void pwm_enable  (int ccr);
void pwm_disable (void);

void pwm_set (uint32_t duty_cycle);

void timer1_dump_registers (void);
void timer2_dump_registers (void);
#endif

