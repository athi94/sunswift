/* --------------------------------------------------------------------------                                 
    TyreNode MAX6675 control routines
    File name: max6675.c
    Author: Etienne Le Sueur, David Snowdon
    Description: Routines for accessing the MAX 6675
                 thermocouple chip for brake disc temp
                 sensing

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

typedef unsigned char u08;

#include "spi_devices.h"
#include "spi_driver.h"
#include "max6675.h"
#include "hardware.h"

#define DELAY(x) {		\
	uint32_t i;			\
	for(i=0;i<x;i++)	\
		__asm__ ("nop");\
}

void get_max6675_data(uint16_t *store)
{
	*store = spi1_transfer();	// send initial measure command
}
