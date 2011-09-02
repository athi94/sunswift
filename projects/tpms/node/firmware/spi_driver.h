/* --------------------------------------------------------------------------                                 
    SP12T SPI Driver
    File name: spi_driver.h
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

#ifndef __SPI_DRIVER__
#define __SPI_DRIVER__

u08 init_spi();
u08 spi_select_device(u08	device);
void spi_deselect_all();
u08 spi_transfer(u08 out_data);

#endif
