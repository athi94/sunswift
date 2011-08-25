# Copyright (C) David Snowdon, Robert Reid 2009 <scandal@snowdon.id.au> */ 

# 
# This file is part of the UNSWMPPTNG firmware.
#
# The UNSWMPPTNG firmware is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
 
# The UNSWMPPTNG firmware is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
 
# You should have received a copy of the GNU General Public License
# along with the UNSWMPPTNG firmware.  If not, see <http://www.gnu.org/licenses/>.
# 
 
.PHONY: clean

MAIN_NAME=mpptng
ARCH=msp430
MCU=msp430x149
BUILD_DIR=.
OUTPUT_DIR=../../outputs/

SCANDAL_DIR=../../scandal

AS=msp430-as
CC=msp430-gcc
OBJCOPY=msp430-objcopy
OBJDUMP=msp430-objdump

JTAG=msp430-jtag
#JTAG=jtag.py

CFLAGS=-g -O2 -Wall -Werror -mmcu=$(MCU) -I$(SCANDAL_DIR)/scandal/include -I$(SCANDAL_DIR)/mcp2510 -I. -I../firmware
LDFLAGS=-mmcu=$(MCU) 
ASFLAGS=-mmcu=$(MCU) 

all: $(MAIN_NAME).hex $(MAIN_NAME).lst
	mkdir -p $(OUTPUT_DIR)
	cp $(MAIN_NAME).hex $(OUTPUT_DIR)/$(MAIN_NAME).hex

## Copy the .hex and .elf file
 
$(MAIN_NAME).hex: $(MAIN_NAME).elf
	$(OBJCOPY) -O ihex $< $@

$(MAIN_NAME).lst: $(MAIN_NAME).elf
	$(OBJDUMP) -DS $(MAIN_NAME).elf > ${MAIN_NAME}.lst	

## Link the .o files into the .out (.elf) file. 

$(MAIN_NAME).elf :  $(MAIN_NAME).o spi_driver.o led.o scandal_timer.o scandal_obligations.o $(BUILD_DIR)/scandal_eeprom.o $(BUILD_DIR)/mcp2510.o $(BUILD_DIR)/scandal_error.o $(BUILD_DIR)/scandal_engine.o  $(BUILD_DIR)/scandal_utils.o $(BUILD_DIR)/scandal_message.o fpga.o control.o pv_track.o config.o error.o 
	$(CC) $^ $(LDFLAGS) -o $(MAIN_NAME).elf

#$(MAIN_NAME).elf :  $(MAIN_NAME).o scandal_timer.o led.o adc.o control.o
#	$(CC) $^ $(LDFLAGS) -o $(MAIN_NAME).elf

## How to build .o, .s and .o files from .c, .c and .s files respectively. 

%.o : %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

%.o : %.c 
	$(CC) -c $(CFLAGS) $< -o $@

%.s : %.c
	$(CC) -S $(CFLAGS) $< -o $@

%.o : %.s
	$(AS) -c $(ASFLAGS) $< -o $@

## How to build individual

$(BUILD_DIR)/scandal_eeprom.o : $(SCANDAL_DIR)/mspenv/scandal_eeprom.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/mcp2510.o : $(SCANDAL_DIR)/mcp2510/mcp2510.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/scandal_error.o : $(SCANDAL_DIR)/scandal/source/scandal_error.c
	$(CC) $(CFLAGS) -c -o $@ $<	

$(BUILD_DIR)/scandal_engine.o : $(SCANDAL_DIR)/scandal/source/scandal_engine.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/scandal_utils.o : $(SCANDAL_DIR)/scandal/source/scandal_utils.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/scandal_message.o : $(SCANDAL_DIR)/scandal/source/scandal_message.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.out *.map *.hex *.elf *.lst
	rm -f $(BUILD_DIR)/*.o
	rm -f *~

program: $(MAIN_NAME).hex
	$(JTAG) -eEpvrw $(MAIN_NAME).hex

program_noerase: $(MAIN_NAME).hex
	$(JTAG) -mEpvr $(MAIN_NAME).hex

eeprom.hex: 
	$(JTAG) --upload=0x1000 --size=0x100 --ihex > eeprom.hex

program_eeprom: eeprom.hex
	$(JTAG) --eraseinfo -pvr eeprom.hex

