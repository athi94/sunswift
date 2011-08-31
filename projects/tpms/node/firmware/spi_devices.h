/* --------------------------------------------------------------------------                                 
    SP12T SPI Driver
    File name: spi_devices.h
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
  
#ifndef __SPIDEVICES__ 
#define __SPIDEVICES__ 

#include <io.h>

#define BIT(x) (1<<x)

#define SP12T			0              
#define MAX6675			1              
#define SPI_NUM_DEVICES         2
#define SPI_DEVICE_NONE		SPI_NUM_DEVICES 

/* SP12T */
#define ENABLE_SP12T()        (P5OUT &= ~BIT(0))
#define DISABLE_SP12T()       (P5OUT |= BIT(0))

#define ENABLE_MAX6675()        (P6OUT &= ~TEMP_CS)
#define DISABLE_MAX6675()       (P6OUT |= TEMP_CS)
#endif
