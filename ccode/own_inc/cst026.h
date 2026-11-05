// file    : cst026.h
// author  : rb
// purpose : header file for cst026.c 
// date    : 260717
// last    : 260928

#ifndef __CST026_H__
#define __CST026_H__

// -- globals 
typedef struct
{
           uint16_t x_pos;            // touch oiint x-coordinate
           uint16_t y_pos;            // touch oiint y-coordinate
  volatile uint8_t  new_touch;        // touch event flag, set in ISR/cleared in polled loop
           uint8_t  pressed;          // screen touched - bit 7 in hi-nibble x-coordinate
           uint8_t  release;          // touch released - bit 6 in hi-nibble x-coordinate
} touch_t;

// -- hi-nibble x-coordinate mask bits
#define CST026_MASK_PRESSED                  0x80  // pressed flag mask
#define CST026_MASK_RELEASE                  0x40  // release flag mask

// -- defines CST026 registers
#define CST026_I2C_ADR                       0x38  // 7-bit I2C address - 8-bit write address = 0x70
                                            
#define CST026_CHIP_ID                       0x64  // CIPHER
#define CST026_VENDOR_ID                     0x11  // VENDORID - not working, always 0x00 - don' test code on this!
                                            
#define CST026_REG_DEVICE_MODE               0x00  // device mode (0: working, 4: factory)
#define CST026_REG_GESTURE_ID                0x01  // gesture ID               - always 0x00
#define CST026_REG_TD_STATUS                 0x02  // number of touch points (0..2)
                                             
#define CST026_REG_TOUCH1_XH                 0x03  // touch1 x hi-part (hi-nibble holds event flags) 
#define CST026_REG_TOUCH1_XL                 0x04  // touch1 x lo-part
#define CST026_REG_TOUCH1_YH                 0x05  // touch1 y hi-part (hi-nibble holds event flags) 
#define CST026_REG_TOUCH1_YL                 0x06  // touch1 y lo-part
                                             
#define CST026_REG_TOUCH2_XH                 0x09  // touch2 x hi-part (hi-nibble holds event flags)
#define CST026_REG_TOUCH2_XL                 0x0a  // touch2 x lo-part
#define CST026_REG_TOUCH2_YH                 0x0b  // touch2 y hi-part (hi-nibble holds event flags)
#define CST026_REG_TOUCH2_YL                 0x0c  // touch2 y lo-part
                                            
#define CST026_REG_TOUCH1_WEIGHT             0x07  // touch1 weight            - always 0x00
#define CST026_REG_TOUCH1_AREA               0x08  // touch1 misc/area         - always 0x00
#define CST026_REG_TOUCH2_WEIGHT             0x0d  // touch2 weight            - always 0x00
#define CST026_REG_TOUCH2_AREA               0x0e  // touch2 misc/area         - always 0xff
                                            
#define CST026_REG_TH_GROUP                  0x80  // touch threshold          - alwaus 0x00
#define CST026_REG_PERIODACTIVE              0x88  // report rate active mode  - always 0x00
#define CST026_REG_PERIODMONITOR             0x89  // report rate monitor mode - always 0x00
                                                                                       
#define CST026_REG_LIB_VERSION               0xa1  // library version          - always 0x00
#define CST026_REG_CIPHER                    0xa3  // chip ID / cipher         - always 0x00
#define CST026_REG_GMODE                     0xa4  // interrupt mode           - always 0x00
#define CST026_REG_FIRMID                    0xa6  // firmware ID              - always 0x00
#define CST026_REG_VENDORID                  0xa8  // vendor ID                - always 0x00
#define CST026_REG_STATE                     0xbc  // current state            - always 0x00
                                                 
// -- prototypes
void init_cst026 (void);

void cst026_update (void);

void cst026_dump          (void);
void cst026_dump_register (void);

#endif


