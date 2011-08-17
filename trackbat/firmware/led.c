/* --------------------------------------------------------------------------                                 
    TrackBat
    File name: led.c
    Author: Etienne Le Sueur, David Snowdon
    Description: led control

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

#include <io.h>
#include <signal.h>
#include <iomacros.h>

#include "scandal_led.h"

#include "hardware.h"
    
#define BIT(x) (1<<x)

void yellow_led(u08 on){
	if(!on)
		P3OUT |= YELLOWLED;
	else
		P3OUT &= ~YELLOWLED;	
}
 
void toggle_yellow_led(void){
	P3OUT ^= YELLOWLED;
}

void red_led(u08 on){
	if(!on)
		P3OUT |= REDLED;
	else
		P3OUT &= ~REDLED;	
}
 
void toggle_red_led(void){
	P3OUT ^= REDLED;
}
