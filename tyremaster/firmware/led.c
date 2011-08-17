/* --------------------------------------------------------------------------                                 
    LED Control
    File name: led.c
    Author: Etienne Le Sueur, David Snowdon

    Copyright (C) Etienne Le Sueur, David Snowdon, 2009. 
    
    Date: 01-10-2009
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
 
#include <io.h>
#include <signal.h>
#include <iomacros.h>

#include "scandal_led.h"

#include "hardware.h"
    
#define BIT(x) (1<<x)

void yellow_led(u08 on){
	if(!on)
		P5OUT |= YELLOWLED;
	else
		P5OUT &= ~YELLOWLED;	
}
 
void toggle_yellow_led(void){
	P5OUT ^= YELLOWLED;
}

void red_led(u08 on){
	if(!on)
		P5OUT |= REDLED;
	else
		P5OUT &= ~REDLED;	
}
 
void toggle_red_led(void){
	P5OUT ^= REDLED;
}
