/* --------------------------------------------------------------------------                                 
    LED Control
    File name: led.c
    Author: Etienne Le Sueur, David Snowdon
    Description: LED control routines

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
#include <signal.h>
#include <iomacros.h>

#include "hardware.h"
#include "led.h"

void led(u08 on){
	if(!on)
		P5OUT |= LED;
	else
		P5OUT &= ~LED;	
}
 
void toggle_led(void){
	P5OUT ^= LED;
}
