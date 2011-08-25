/* --------------------------------------------------------------------------																 
  TrackBat
  File name: trackbat.c
  Author: Etienne Le Sueur, David Snowdon, Robert Reid
  Description: main trackbat handlers and setup

  Copyright (C) Etienne Le Sueur, 2011. 

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Sunswift TrackBat. If not, see <http://www.gnu.org/licenses/>.
 */

#include <io.h>
#include <signal.h>
#include <iomacros.h>
#include <string.h>

#include "scandal_timer.h"
#include "scandal_led.h"
#include "scandal_can.h"
#include "scandal_engine.h"
#include "scandal_spi.h"
#include "spi_devices.h"
#include "scandal_devices.h"
#include "scandal_utils.h"
#include "scandal_message.h"

#include "hardware.h"
#include "scandal_uart.h"

#define WDTCTL_INIT	 WDTPW|WDTHOLD

void init_ports(void){
	P1OUT = 0x00;
	P1SEL = 0x00;
	P1DIR = 0x00;
	P1IES = 0x00;
	P1IE  = 0x00; 

	P2OUT = 0x00;
	P2SEL = 0x00;
	P2DIR = 0x00;
	P2IES = CAN_INT;
	P2IE  = 0x00;

	P3OUT = 0x00;
	P3SEL = 0x00;
	P3DIR = YELLOWLED | REDLED;

	P4OUT = CAN_RESET;
	P4SEL = 0x00;
	P4DIR = CAN_RESET;

	P5OUT = CAN_CS;
	P5SEL = SIMO1 | SOMI1 | UCLK1;
	P5DIR = CAN_CS | SIMO1 | UCLK1;

	P6SEL = MEAS_VIN_PIN | MEAS_VOUT1_PIN | MEAS_VOUT2_PIN | MEAS_VOUT3_PIN | MEAS_VOUT4_PIN;	
}

void init_clock(void){
	volatile unsigned int i;

	/* XTAL = LF crystal, ACLK = LFXT1/1, DCO Rset = 4, XT2 = ON */
	BCSCTL1 = 0x04;

	/* Clear OSCOFF flag - start oscillator */
	_BIC_SR( OSCOFF );
	do{
		/* Clear OSCFault flag */
		IFG1 &= ~OFIFG; 
		/* Wait for flag to set */
		for( i = 255; i > 0; i-- )
			;
	} while(( IFG1 & OFIFG ) != 0);

	/* Set MCLK to XT2CLK and SMCLK to XT2CLK */
	BCSCTL2 = 0x88; 
}

/*--------------------------------------------------
	Interrupt handing for CAN stuff 
	--------------------------------------------------*/
void enable_can_interrupt(){
	P2IE = CAN_INT;
}

void disable_can_interrupt(){
	P2IE = 0x00;
}

interrupt (PORT2_VECTOR) port2int(void) {
	toggle_red_led();
	can_interrupt();
	P2IFG = 0x00;
}

/* Main function */
int main(void)
{
	sc_time_t my_timer;

	init_ports();

	yellow_led(0);
	red_led(0);

	dint();
	WDTCTL = WDTCTL_INIT;	 //Init watchdog timer
	init_clock();
	sc_init_timer();
	scandal_init(); 
	eint();
 
	my_timer = sc_get_timer();

	while(1) {
		handle_scandal();

		if(sc_get_timer() >= my_timer + 1000) {
			my_timer = sc_get_timer();
			toggle_yellow_led();
		}
	}
}
