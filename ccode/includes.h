//#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>  // needed for 'atof'

#include "stm32g4xx.h"
#include "stm32g4xx_hal_gpio_ex.h"
#include "stm32g4xx_hal_flash.h"
#include "stm32g4xx_hal_dma.h"

#include "./inc/stm32g474xx.h"

#include "version.h"

#include "ports.h"
#include "pll.h"
#include "ticker.h"
#include "uart1.h"
#include "printf.h"
#include "printl.h"
#include "adc.h"
#include "spi.h"
#include "i2c.h"
#include "cli.h"
#include "ili9341.h"
#include "lcd.h"
#include "cst026.h"
#include "gui.h"
#include "bms.h"
#include "bms_test.h"
#include "tga.h"

//#include "tga.h"
//#include "dcdc.h"
