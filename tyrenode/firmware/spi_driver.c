/* --------------------------------------------------------------------------                                 
    SP12T SPI Driver
    File name: spi_driver.c
    Author: Etienne Le Sueur, David Snowdon
    Description: The SPI driver for the SP12T tyre pressure chip

    Copyright (C) Etienne Le Sueur, David Snowdon, 2009. 
    
    Date: 01-10-2009
   -------------------------------------------------------------------------- */

/* 
 * This file is part of Sunswift TyreNode.
 * 
 * Sunswift TyreNode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * Sunswift TyreNode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Sunswift TyreNode.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <io.h>

#include "spi_devices.h" 
#include "scandal_spi.h"

#include "hardware.h"

#include "scandal_led.h"

#define DELAY(x) {		\
	uint32_t i;			\
	for(i=0;i<x;i++)	\
		__asm__ ("nop");\
}

/* Initialize the SPI driver */         

/* Local variables */ 
u08 spi_selected_device; 

/*! Initialise the SPI driver to the default values */ 
u08 init_spi(){
  ME2 |= USPIE1;
  
  U1CTL  = SYNC+MM+CHAR;
  U1TCTL = STC | SSEL1 | SSEL0;
  UBR01 = 0x10;
  UBR11 = 0x00;
  UMCTL1 = 0x00;

  spi_selected_device = SPI_DEVICE_NONE; 
  spi_deselect_all(); 
  return(0); 
}  

/*! Select a particular device */ 
u08 spi_select_device(u08	device){  
  if(spi_selected_device != SPI_DEVICE_NONE) 
    return(1); 
   
  spi_deselect_all(); 
   
  switch(device){ 
  case MAX6675:
    ENABLE_MAX6675();
    break;
  case SP12T:
    ENABLE_SP12T();
    break;
  } 
   
  spi_selected_device = device; 	 
  return(0); 
} 

/*! Deselect all devices */ 
void spi_deselect_all(){ 
  DISABLE_SP12T();
  DISABLE_MAX6675();
  spi_selected_device = SPI_DEVICE_NONE; 
} 

/*! Perform an SPI read/write */ 
u08 spi_transfer(u08 out_data){ 
  u08	value; 

  IFG2 &= ~URXIFG1;

  while((IFG2 & UTXIFG1) == 0)
      ;
  
  TXBUF1 = out_data;

  while((IFG2 & URXIFG1) == 0)
      ;

  value = RXBUF1;

  return(value); 
} 

void init_spi1() {
	spi_deselect_all();
}

#define SPI2_CLK_H()       (P6OUT |= UCLK2)
#define SPI2_CLK_L()       (P6OUT &= ~UCLK2)
#define SPI2_SOMI_VAL()    ((P6IN & SOMI2) >> 3)

uint16_t spi1_transfer() {

	u16 data;
	int i;
	spi_select_device(MAX6675);	// CS high
	SPI2_CLK_L();

	for(i = 0; i < 16; i++) {
		SPI2_CLK_H();
		data << 1;
		data |= SPI2_SOMI_VAL();
		SPI2_CLK_L();
	}
	
	SPI2_CLK_L();
	spi_deselect_all();
	return data;
}
