/* --------------------------------------------------------------------------                                 
    TyreNode Main
    File name: tyre-node.c
    Author: Etienne Le Sueur, David Snowdon
    Description: The main tyre-node program

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
#include <string.h>

#include "hardware.h"
#include "scandal_uart.h"
#include "sp12t.h"
#include "max6675.h"
#include "led.h"

#define TYRE_ID		10	// which tyre?
//#define TYRE_ID		20	// which tyre?
//#define TYRE_ID		30	// which tyre?
//#define TYRE_ID		40	// which tyre?

static volatile u32 ms;

#define DELAY(x) {		\
	uint32_t i;			\
	for(i=0;i<x;i++)	\
		__asm__ ("nop");\
}

void init_clock(void);

void init_ports(void)
{
	P3OUT = 0x00;
	P3SEL = TX;
	P3DIR = TX;

	P4OUT = PWR;
	P4SEL = 0x00;
	P4DIR = PWR;

	P5OUT = TYRE_CS | LED;
	P5SEL = SIMO1 | SOMI1 | UCLK1;
	P5DIR = TYRE_CS | SIMO1 | UCLK1 | LED;

	P6OUT = TEMP_CS;
	P6SEL = 0x00;
	P6DIR = (TEMP_CS | UCLK2);
}

void enable(u08 yes)
{

	if(!yes) {
		P1OUT = 0x00;
		P2OUT = 0x00;
		P3OUT = 0x00;
		P4OUT = 0x00;
		P5OUT = LED;
		P6OUT = 0x00;
		P1SEL = 0x00;
		P2SEL = 0x00;
		P3SEL = 0x00;
		P4SEL = 0x00;
		P5SEL = 0x00;
		P6SEL = 0x00;
		P1DIR = 0xFF;
		P2DIR = 0xFF;
		P3DIR = 0xFF & ~TX;
		P4DIR = 0xFF;
		P5DIR = 0xFF & ~LED;
		P6DIR = 0xFF;

		ME1 = 0x00;
		ME2 = 0x00;

		// this selects the LF clock (32.768kHz) 
		BCSCTL2 = SELM_3 | SELS;
		// turn external 7meg crystal off
		BCSCTL1 |= XT2OFF;
	} else {
		init_ports();	
		init_clock();	
		led(0);
		UART_Init();
		init_tyresensor();
		UART_baud_rate(2400, CLOCK_SPEED);
	}

}

void init_clock(void)
{
	volatile unsigned int i;

	/* XTAL = LF crystal, ACLK = LFXT1/1, DCO Rset = 4, XT2 = ON */
	BCSCTL1 = 0x04 | DIVA_3;
	
	/* Clear OSCOFF flag - start oscillator */
	_BIC_SR( OSCOFF );

	do {
		/* Clear OSCFault flag */
		IFG1 &= ~OFIFG; 
		/* Wait for flag to set */
		for( i = 255; i > 0; i-- )
			;
	} while(( IFG1 & OFIFG ) != 0);
	
	/* Set MCLK to XT2CLK and SMCLK to XT2CLK */
	BCSCTL2 = 0x88; 
}

/* Using my own timer here, because we don't want an interrupt every
 * 1000ms.
 */
void init_timer(void)
{
	ms = 0;

	TACTL = TACLR | ID_DIV1 | TASSEL_ACLK;

	TACCTL0 |= CCIE;

//	TACCR0 = 65535;	
	TACCR0 = 32767;	

	TACTL |= MC_UPTO_CCR0;
}

interrupt (TIMERA0_VECTOR) timera_int(void)
{
	ms += 1000;
	_BIC_SR_IRQ(LPM3_bits); // come out of low power mode.
}

/* Main function */
int main(void)
{
	sp12t_data_t store;
	uint16_t disc_temp;
//	uint32_t my_timer = 0;

	dint();
	init_ports();		// Init the ports for SP12T and Transmitter
	
	init_clock();
	init_timer();
	UART_Init();
	init_tyresensor();
	eint();

	led(0);


	/* Set the tyremaster to use	4800 baud */
	UART_baud_rate(2400, CLOCK_SPEED);

	while (1) {

	//	if(ms >= my_timer + 1000) {	
	//		my_timer = ms;
	//		toggle_led();
	//	}

//		led(1);
		
		P6OUT &= ~TEMP_CS;
		P6OUT |= TEMP_CS;

		enable(1);
		DELAY(120000);
//		led(0);

		uint8_t chksum;
		get_max6675_data(&disc_temp);	
		get_sp12t_data(&store);
		chksum = TYRE_ID + (uint8_t)(store.pressure>>8) + (uint8_t)(store.pressure & 0xFF) + store.air_temp + store.batt_voltage + (uint8_t)(disc_temp >> 8) + (uint8_t)(disc_temp & 0xFF);
		UART_SendByte(0xAA);			//preamble
		UART_SendByte(TYRE_ID);			//tyre_id
		UART_SendByte(store.pressure & 0xFF);	//pressure byte 0
		UART_SendByte(store.pressure >> 8);	//pressure byte 1
		UART_SendByte(store.air_temp);		//air_temp byte 0
		UART_SendByte(store.batt_voltage);	//batt_voltage byte 0
		UART_SendByte(disc_temp & 0xFF);	//disc_temp byte 0
		UART_SendByte(disc_temp >> 8);		//disc_temp byte 1
		UART_SendByte(chksum);	//chksum

		while( (U0TCTL & TXEPT) == 0)
			;

//		DELAY(1000);
	
//		led(0);
		enable(0);
	//	_BIS_SR(LPM3_bits); // go into low power mode and wake up at next timer tick.
		LPM3;
	}
}
