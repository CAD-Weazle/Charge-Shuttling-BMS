### project name -- edit this
TARGET = bmstest

### sources -- edit this
CSRC += ./core/system_stm32g4xx.c                                      # EDITED
#CSRC += ./cmath/powten.c ./cmath/atof.c ./cmath/dtoa.c
CSRC += main.c
CSRC += pll.c
CSRC += ports.c
CSRC += ticker.c
CSRC += uart1.c
CSRC += printf.c
CSRC += printl.c
CSRC += cli.c
CSRC += adc.c
CSRC += spi.c
CSRC += i2c.c
CSRC += ili9341.c
CSRC += lcd.c
CSRC += cst026.c
CSRC += gui.c
CSRC += bms.c
CSRC += bms_test.c
#CSRC += tga.c


### add startup file to build -- edit this
ASRC = ./core/startup_stm32g474xx.s                               # EDITED

### linker-loader script -- edit this
LDSCRIPT = STM32G474XX_FLASH.ld                               

# GCC compiler
CCPATH  = d:/gcc/bin/
CC      = $(CCPATH)arm-none-eabi-gcc
OBJCOPY = $(CCPATH)arm-none-eabi-objcopy
OBJDUMP = $(CCPATH)arm-none-eabi-objdump
SIZE    = $(CCPATH)arm-none-eabi-size
NM      = $(CCPATH)arm-none-eabi-nm

CFLAGS  = -DSTM32G474xx                     # -- edit this          # EDITED
CFLAGS += -mcpu=cortex-m4                   # -- edit this
CFLAGS += -std=gnu99 -g -O2 -Wall -T$(LDSCRIPT)
CFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles
CFLAGS += -fsingle-precision-constant
CFLAGS += -mfloat-abi=hard
CFLAGS += -mfpu=fpv4-sp-d16
CFLAGS += -fshort-double
CFLAGS += -fno-builtin-malloc
CFLAGS += -fno-builtin-printf
CFLAGS += -specs=nano.specs
CFLAGS += -specs=nosys.specs
CFLAGS += $(DEFINES)

#CFLAGS += -Xlinker --verbose               # verbose linker output, dev only

### own flags -- edit this
CFLAGS += -Iinc -Iinc/core -Iown_inc

AFLAGS = -Iinc
AFLAGS += -mlittle-endian -mthumb -mthumb-interwork -nostartfiles -mcpu=cortex-m4
AFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=hard

# dependencies
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# rules
.PHONY: lib proj

HEX = $(TARGET).hex
ELF = $(TARGET).elf
DIS = $(TARGET).dis
OBJS = $(CSRC:.c=.o) $(ASRC:.S=.o)

all: proj

#proj: $(ELF) $(DIS) $(HEX)
proj: $(ELF) $(HEX)

$(DIS): $(ELF)
	$(OBJDUMP) -d $(ELF) > $(DIS)

# $(OBJDUMP) -D $(ELF) > $(DIS).all

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex --set-start=0 $(ELF) $(HEX)

$(ELF): $(OBJS)
	$(CC) $(GENDEPFLAGS) $(CFLAGS) $(OBJS) -o $@ -lm -Wl,-Map=$(TARGET).map

%.o: %.c
	@echo $(CC) $<
	@$(CC) $(GENDEPFLAGS) $(CFLAGS) -c $< -o $@

%.o: %.S
	@echo $(CC) $<
	@$(CC) $(GENDEPFLAGS) $(AFLAGS) -c $< -o $@

clean:
	rm -f $(wildcard *.o)
	rm -f $(wildcard cmath/*.o)
	rm -f $(ELF)
	rm -f $(DIS)
	rm -f $(TARGET).bin
	rm -f $(TARGET).map
	rm -f $(TARGET).hex
	rm -f $(wildcard .dep/*)

list:
	@echo
	@echo C sources:
	@echo $(CSRC)
	@echo
	@echo ASM sources:
	@echo $(ASRC)
	@echo
	@echo objects:
	@echo $(OBJS)

include $(wildcard .dep/*)
