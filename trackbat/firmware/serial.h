/* --------------------------------------------------------------------------                                 
    TrackBat
    File name: serial.h
    Author: Etienne Le Sueur, David Snowdon, Robert Reid
    Description: serial routines

    Copyright (C) Etienne Le Sueur, David Snowdon, 2011. 
    
    Date: 7-5-2011
   -------------------------------------------------------------------------- */

/* 
 * This file is part of Sunswift TrackBat.
 * 
 * Sunswift TrackBat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * Sunswift TrackBat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Sunswift TrackBat.  If not, see <http://www.gnu.org/licenses/>.
 */

void init_uart();
void uart0_tx(char * buf, int buf_len);
