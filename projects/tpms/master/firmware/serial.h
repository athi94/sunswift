/* --------------------------------------------------------------------------                                 
    Serial Driver
    File name: serial.c
    Author: Robert Reid, Etienne Le Sueur, David Snowdon
    Description: Serial port communications for MSP430

    Copyright (C) Robert Reid, Etienne Le Sueur, David Snowdon, 2009. 
    
    Date: 10-09-2003
   -------------------------------------------------------------------------- */

/* 
 * This file is part of Sunswift TyreMaster.
 * 
 * Sunswift TyreMaster is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * Sunswift TyreMaster is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Sunswift TyreMaster.  If not, see <http://www.gnu.org/licenses/>.
 */

void init_uart();
void uart0_tx(char * buf, int buf_len);
