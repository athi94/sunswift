#directory definitions
ROOT = ../../..
SCANDAL = $(ROOT)/scandal
ARCH = $(SCANDAL)/src/arch/lpc11c14
UTILS = $(ROOT)/utils
LINKER = $(ARCH)/linker
CMSIS = $(ARCH)/cmsis
DRIVERS = $(ARCH)/driver
STARTUP = $(ARCH)/startup
BUILD=. # never set to . otherwise clean will nuke everything
SRC = ./src

#define utilities
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
START_PROGRAMMER = dfu-util
FLASH_CHIP = crt_emu_lpc11_13_nxp
CHECKSUM = checksum
REMOVE = rm -f

#architecture
MCU = cortex-m0
UC = LPC11C14/301

#define files
PROJECT = main

LINKSCRIPT = lpc1114_flash_crp.ld
STARTUPSCRIPT = cr_startup_lpc11

OBJECTS  = $(PROJECT).o
OBJECTS += core_cm0.o system_LPC11xx.o
OBJECTS += gpio.o timer32.o uart.o can.o i2c.o
OBJECTS += engine.o error.o manager.o maths.o message.o utils.o
OBJECTS += $(STARTUPSCRIPT).o

#compiler flags
OPTIMISATION = 0
DEBUG = 

CFLAGS  = -mcpu=$(MCU) -DLPC11C14 -D__NEWLIB__ -D__USE_CMSIS -D__CODE_RED -mthumb -O$(OPTIMISATION) $(DEBUG)
CFLAGS += -I$(SCANDAL)/include # for scandal includes
CFLAGS += -I$(ARCH)/include # for arch drivers
CFLAGS += -I$(ARCH) # for cmsis
CFLAGS += -I./include # for cmsis to use system_LPC11xx.h
CFLAGS += -lm #Added lm for math.h

LSCRIPT = $(LINKER)/$(LINKSCRIPT)
LFLAGS  = -nostdlib  -Xlinker -Map=$(PROJECT).map -Xlinker --gc-sections -L $(LINKER) -T "$(LSCRIPT)"

.PHONY: all clean program

all: $(PROJECT).axf $(PROJECT).bin

$(PROJECT).axf: $(OBJECTS)
	@$(CC) $(CFLAGS) $(LFLAGS) $(OBJECTS) -o $(PROJECT).axf 

###compile object files individually
$(PROJECT).o: src/$(PROJECT).c
	@echo $(CFLAGS)
	@$(CC) $(CFLAGS) -c -o $@ $<

core_cm0.o: $(CMSIS)/core_cm0.c
	@$(CC) $(CFLAGS) -c -o $@ $<

engine.o: $(SCANDAL)/src/engine.c
	@$(CC) $(CFLAGS) -c -o $@ $<

system_LPC11xx.o: $(CMSIS)/system_LPC11xx.c
	@$(CC) $(CFLAGS) -c -o $@ $<

gpio.o: $(DRIVERS)/gpio.c
	@$(CC) $(CFLAGS) -c -o $@ $<

timer32.o: $(DRIVERS)/timer32.c
	@$(CC) $(CFLAGS) -c -o $@ $<

uart.o: $(DRIVERS)/uart.c
	@$(CC) $(CFLAGS) -c -o $@ $<

can.o: $(DRIVERS)/can.c
	@$(CC) $(CFLAGS) -c -o $@ $<

i2c.o: $(DRIVERS)/i2c.c
	@$(CC) $(CFLAGS) -c -o $@ $<

$(STARTUPSCRIPT).o: $(STARTUP)/$(STARTUPSCRIPT).c
	@$(CC) $(CFLAGS) -c -o $@ $<

#$(BUILD)/LPC1xxx_startup.o: $(STARTUP)/LPC1xxx_startup.c

program: $(PROJECT).axf
	@# see http://support.code-red-tech.com/CodeRedWiki/CommandLineFlashProgramming
	-$(START_PROGRAMMER) -d 0x471:0xdf55 -c 0 -t 2048 -R -D $(UTILS)/LPCXpressoWIN.enc
	@sleep 1
	@#program using .axf
	$(FLASH_CHIP) -wire=winusb -p$(UC) -flash-load-exec=$(BUILD)/$(PROJECT).axf
	@#program using .bin, doesn't seem to work atm
	@#sudo $(FLASH_CHIP) -wire=winusb -p$(UC) -flash-load=$(BUILD)/$(PROJECT).bin -load-base=0x0000

clean:

$(PROJECT).bin: $(PROJECT).axf
	@$(SIZE) $(PROJECT).axf; $(OBJCOPY) -O binary $(PROJECT).axf $(PROJECT).bin ; $(CHECKSUM) -p $(UC) -d $(PROJECT).bin;
	@echo ""
