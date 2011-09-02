/* Copyright (C) David Snowdon, 2009 <scandal@snowdon.id.au> */ 

/* 
 * This file is part of the UNSWMPPTNG firmware.
 * 
 * The UNSWMPPTNG firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * The UNSWMPPTNG firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with the UNSWMPPTNG firmware.  If not, see <http://www.gnu.org/licenses/>.
 */

/* --------------------------------------------------------------------------
	LED Control
	File name: led.c
	Author: David Snowdon

	Date: 1/7/03
   -------------------------------------------------------------------------- */ 
 
#include <io.h>
#include <signal.h>
#include <iomacros.h>

#include "scandal_led.h"

#include "hardware.h"
    
#define BIT(x) (1<<x)

void yellow_led(u08 on){
	if(!on)
		P3OUT |= LED1;
	else
		P3OUT &= ~LED1;	
}
 
void toggle_yellow_led(void){
	P3OUT ^= LED1;
}

void red_led(u08 on){
	if(!on)
		P3OUT |= LED2;
	else
		P3OUT &= ~LED2;	
}
 
void toggle_red_led(void){
	P3OUT ^= LED2;
}
