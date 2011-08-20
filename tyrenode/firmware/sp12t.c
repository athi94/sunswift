/* --------------------------------------------------------------------------                                 
    TyreNode SP12T control routines
    File name: sp12t.c
    Author: Etienne Le Sueur, David Snowdon
    Description: Routines for accessing the SP12T
                 presssure sensor chip.

    Copyright (C) Etienne Le Sueur, David Snowdon, Simon Li, 2009. 
    
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

typedef unsigned char u08;

#include "spi_devices.h"
#include "spi_driver.h"
#include "sp12t.h"
#include "hardware.h"

#define DELAY(x) {		\
	uint32_t i;			\
	for(i=0;i<x;i++)	\
		__asm__ ("nop");\
}

/* set up the first command.
 * this is read pressure, so you must read the pressure
 * first 
 */
void init_tyresensor(void)
{
	init_spi();
//	spi_select_device(SP12T);	// CS high
//	DELAY(3);
//	spi_transfer(SP12T_MEASURE_P);	// send initial measure command
//	spi_deselect_all();		// CS low
}

void get_sp12t_data(struct sp12t_data *store)
{
	uint16_t i, pressure;
	u08 hi,lo;
	
	spi_select_device(SP12T);	// CS high
	DELAY(3);
	spi_transfer(SP12T_MEASURE_P);	// send initial measure command
	spi_deselect_all();		// CS low

	// about 6.7ms delay (need 6)
	DELAY(5000); // wait for pressure measure

	spi_select_device(SP12T);	// CS high
	spi_transfer(SP12T_RCPDH);	// send get hi command (reads back command)
	spi_deselect_all();		// CS low

	DELAY(10);
	spi_select_device(SP12T);	// CS high
	hi = spi_transfer(SP12T_RCPDL);	// receive high, ask for low
	spi_deselect_all();		// CS low

	DELAY(10);
	spi_select_device(SP12T);	// CS high
	lo = spi_transfer(SP12T_MEASURE_T); // get pressure low byte and send measure temp command
	spi_deselect_all();		// CS low

	// process the pressure bytes
	pressure = 0; 
	for(i=0; i<8; i++) {
		pressure <<= 1; 
		if(hi & 0x01)
			pressure |= 0x01; 
		hi >>= 1; 
	}
	for(i=0; i<8; i++) {
		pressure <<= 1; 
		if(lo & 0x01)
			pressure |= 0x01; 
		lo >>= 1; 
	}

	store->pressure = pressure;

	// about a 1.5 ms delay
	DELAY(1250); // wait for temp measure

	spi_select_device(SP12T);
	spi_transfer(SP12T_RCTMP); // send receive temp command, receive back measure temp command
	spi_deselect_all();
	
	DELAY(10);
	spi_select_device(SP12T);
	lo = spi_transfer(SP12T_MEASURE_V); // send initial measure voltage command, receive temp
	spi_deselect_all();
	
	store->air_temp = 0;
	for(i=0; i<8; i++) {
		store->air_temp <<= 1; 
		if(lo & 0x01)
			store->air_temp |= 0x01; 
		lo >>= 1; 
	}

	DELAY(12500); // wait for voltage measure
	
	spi_select_device(SP12T);
	spi_transfer(SP12T_RCV); // send the receive compensated voltage command
	spi_deselect_all();

	DELAY(10);
	spi_select_device(SP12T);
	lo = spi_transfer(SP12T_STANDBY); // get the voltage, and go into standby
	spi_deselect_all();
	
	store->batt_voltage = 0;
	for(i=0; i<8; i++) {
		store->batt_voltage <<= 1; 
		if(lo & 0x01)
			store->batt_voltage |= 0x01; 
		lo >>= 1; 
	}
	
}
