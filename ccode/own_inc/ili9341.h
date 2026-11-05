// file    : ili9341.h
// author  : st
// purpose : header file for ili9341.c
// date    : 240323
// last    : 240403
//

#ifndef _ILI9341_H_
#define _ILI9341_H_

#define ILI9341_ID                        0x9341  // chip ID

#define ILI9341_PIXEL_HEIGHT               (320)  // number pixel x direction
#define ILI9341_PIXEL_WIDTH                (240)  // number pixel y direction

// ILI9341 Registers  
// Level 1 Commands 
#define ILI9341_CMD_SWRESET                  0x01  // Software Reset 
#define ILI9341_CMD_READ_DISPLAY_ID          0x04  // Read display identification information 
#define ILI9341_CMD_RDDST                    0x09  // Read Display Status 
#define ILI9341_CMD_RDDPM                    0x0a  // Read Display Power Mode 
#define ILI9341_CMD_RDDMADCTL                0x0b  // Read Display MADCTL 
#define ILI9341_CMD_RDDCOLMOD                0x0c  // Read Display Pixel Format 
#define ILI9341_CMD_RDDIM                    0x0d  // Read Display Image Format 
#define ILI9341_CMD_RDDSM                    0x0e  // Read Display Signal Mode 
#define ILI9341_CMD_RDDSDR                   0x0f  // Read Display Self-Diagnostic Result 
#define ILI9341_CMD_SPLIN                    0x10  // Enter Sleep Mode 
#define ILI9341_CMD_SLEEP_OUT                0x11  // Sleep out register 
#define ILI9341_CMD_PTLON                    0x12  // Partial Mode ON 
#define ILI9341_CMD_NORMAL_MODE_ON           0x13  // Normal Display Mode ON 
#define ILI9341_CMD_DINVOFF                  0x20  // display inversion OFF 
#define ILI9341_CMD_DINVON                   0x21  // display inversion ON 
#define ILI9341_CMD_GAMMA                    0x26  // gamma register 
#define ILI9341_CMD_DISPLAY_OFF              0x28  // display off register 
#define ILI9341_CMD_DISPLAY_ON               0x29  // display on register 
#define ILI9341_CMD_COLUMN_ADDR              0x2a  // colomn address register  
#define ILI9341_CMD_PAGE_ADDR                0x2b  // page address register  
#define ILI9341_CMD_GRAM                     0x2c  // GRAM register    
#define ILI9341_CMD_RGBSET                   0x2d  // Color SET    
#define ILI9341_CMD_RAMRD                    0x2e  // Memory Read    
#define ILI9341_CMD_PLTAR                    0x30  // Partial Area    
#define ILI9341_CMD_VSCRDEF                  0x33  // Vertical Scrolling Definition    
#define ILI9341_CMD_TEOFF                    0x34  // Tearing Effect Line OFF    
#define ILI9341_CMD_TEON                     0x35  // Tearing Effect Line ON    
#define ILI9341_CMD_MAC                      0x36  // Memory Access Control register
#define ILI9341_CMD_VSCRSADD                 0x37  // Vertical Scrolling Start Address    
#define ILI9341_CMD_IDMOFF                   0x38  // Idle Mode OFF    
#define ILI9341_CMD_IDMON                    0x39  // Idle Mode ON    
#define ILI9341_CMD_PIXEL_FORMAT             0x3a  // Pixel Format register 
#define ILI9341_CMD_WRITE_MEM_CONTINUE       0x3c  // Write Memory Continue    
#define ILI9341_CMD_READ_MEM_CONTINUE        0x3e  // Read Memory Continue    
#define ILI9341_CMD_SET_TEAR_SCANLINE        0x44  // Set Tear Scanline    
#define ILI9341_CMD_GET_SCANLINE             0x45  // Get Scanline    
#define ILI9341_CMD_WDB                      0x51  // Write Brightness Display register 
#define ILI9341_CMD_RDDISBV                  0x52  // Read Display Brightness    
#define ILI9341_CMD_WCD                      0x53  // Write Control Display register
#define ILI9341_CMD_RDCTRLD                  0x54  // Read CTRL Display    
#define ILI9341_CMD_WRCABC                   0x55  // Write Content Adaptive Brightness Control    
#define ILI9341_CMD_RDCABC                   0x56  // Read Content Adaptive Brightness Control    
#define ILI9341_CMD_WRITE_CABC               0x5e  // Write CABC Minimum Brightness    
#define ILI9341_CMD_READ_CABC                0x5f  // Read CABC Minimum Brightness    
#define ILI9341_CMD_READ_ID1                 0xda  // Read ID1 
#define ILI9341_CMD_READ_ID2                 0xdb  // Read ID2 
#define ILI9341_CMD_READ_ID3                 0xdc  // Read ID3 
                                                   
// Level 2 Commands                                
#define ILI9341_CMD_RGB_INTERFACE            0xb0  // RGB Interface Signal Control 
#define ILI9341_CMD_FRMCTR1                  0xb1  // Frame Rate Control (In Normal Mode) 
#define ILI9341_CMD_FRMCTR2                  0xb2  // Frame Rate Control (In Idle Mode) 
#define ILI9341_CMD_FRMCTR3                  0xb3  // Frame Rate Control (In Partial Mode) 
#define ILI9341_CMD_INVTR                    0xb4  // Display Inversion Control 
#define ILI9341_CMD_BPC                      0xb5  // Blanking Porch Control register 
#define ILI9341_CMD_DFC                      0xb6  // Display Function Control register 
#define ILI9341_CMD_ETMOD                    0xb7  // Entry Mode Set 
#define ILI9341_CMD_BACKLIGHT1               0xb8  // Backlight Control 1 
#define ILI9341_CMD_BACKLIGHT2               0xb9  // Backlight Control 2 
#define ILI9341_CMD_BACKLIGHT3               0xba  // Backlight Control 3 
#define ILI9341_CMD_BACKLIGHT4               0xbb  // Backlight Control 4 
#define ILI9341_CMD_BACKLIGHT5               0xbc  // Backlight Control 5 
#define ILI9341_CMD_BACKLIGHT7               0xbe  // Backlight Control 7 
#define ILI9341_CMD_BACKLIGHT8               0xbf  // Backlight Control 8 
#define ILI9341_CMD_POWER1                   0xc0  // Power Control 1 register 
#define ILI9341_CMD_POWER2                   0xc1  // Power Control 2 register 
#define ILI9341_CMD_VCOM1                    0xc5  // VCOM Control 1 register 
#define ILI9341_CMD_VCOM2                    0xc7  // VCOM Control 2 register 
#define ILI9341_CMD_NVMWR                    0xd0  // NV Memory Write 
#define ILI9341_CMD_NVMPKEY                  0xd1  // NV Memory Protection Key 
#define ILI9341_CMD_RDNVM                    0xd2  // NV Memory Status Read 
#define ILI9341_CMD_READ_ID4                 0xd3  // read ID4 
#define ILI9341_CMD_PGAMMA                   0xe0  // positive Gamma Correction register 
#define ILI9341_CMD_NGAMMA                   0xe1  // negative Gamma Correction register 
#define ILI9341_CMD_DGAMCTRL1                0xe2  // digital Gamma Control 1 
#define ILI9341_CMD_DGAMCTRL2                0xe3  // digital Gamma Control 2 
#define ILI9341_CMD_INTERFACE                0xf6  // interface control register 
                                                   
// Extend register commands                        
#define ILI9341_CMD_POWERA                   0xcb  // power control A register 
#define ILI9341_CMD_POWERB                   0xcf  // power control B register 
#define ILI9341_CMD_DTCA                     0xe8  // driver timing control A 
#define ILI9341_CMD_DTCB                     0xea  // driver timing control B 
#define ILI9341_CMD_POWER_SEQ                0xed  // power on sequence register 
#define ILI9341_CMD_3GAMMA_EN                0xf2  // 3 gamma enable register 
#define ILI9341_CMD_PRC                      0xf7  // pump ratio control register 

// -- prototypes
void init_ili9341 (void);

void ili9341_wr_cmd   (uint8_t dat);
void ili9341_wr_dat   (uint8_t dat);
void ili9341_wr_dat16 (uint16_t dat);

void ili9341_hwreset (void);
void ili9341_swreset (void);

void ili9341_select   (void);
void ili9341_deselect (void);

void ili9341_cmd (void);
void ili9341_dat (void);

void ili9341_backlight_on     (void);
void ili9341_backlight_off    (void);
void ili9341_backlight_toggle (void);

void ili9341_brightness (uint8_t dat); 

#endif





