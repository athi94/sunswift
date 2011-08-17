/* --------------------------------------------------------------------------                                 
    TyreMaster Main
    File name: tyre-master.c
    Author: Etienne Le Sueur, David Snowdon
    Description: The main tyre-master program

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
#include "tyre.h"

#define WDTCTL_INIT     WDTPW|WDTHOLD

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
  P3SEL = RX;
  P3DIR = 0x00;

  P4OUT = 0x00;
  P4SEL = 0x00;
  P4DIR = 0x00;

  P5OUT = CAN_CS;
  P5SEL = SIMO1 | SOMI1 | UCLK1;
  P5DIR = CAN_CS | SIMO1 | UCLK1 | YELLOWLED | REDLED;

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
  P1IE = CAN_INT;
}

void disable_can_interrupt(){
  P1IE = 0x00;
}

interrupt (PORT1_VECTOR) port1int(void) {
  toggle_red_led();
  can_interrupt();
  P1IFG = 0x00;
}

/* Main function */
int main(void)
{
	sc_time_t my_timer;

	P5OUT = CAN_CS;
	dint();
	WDTCTL = WDTCTL_INIT;               //Init watchdog timer
	init_ports();
	init_clock();
	sc_init_timer();
	UART_Init();
	scandal_init(); 
	eint();
  
	/* Set the tyremaster to use  4800 baud */
	UART_baud_rate(2400, CLOCK_SPEED);
	
	my_timer = sc_get_timer();

	while(1) {
		handle_scandal();
		
		if(UART_is_received()) {
			uint8_t preamble, tyre_id, air_temp, batt_voltage, chksum, chksum_data, tyre_pressure_0, tyre_pressure_1;
			uint16_t tyre_pressure;

			preamble = UART_ReceiveByte();

			if(preamble == 0xAA) {
				
				tyre_id = UART_ReceiveByte(); 			// the tyre
				tyre_pressure_0 = UART_ReceiveByte();		// pressure low byte
				tyre_pressure_1 = UART_ReceiveByte();		// pressure hi byte
				air_temp = UART_ReceiveByte();			// air temp
				batt_voltage = UART_ReceiveByte();		// batt voltage
				chksum = UART_ReceiveByte();			// chksum
			
				chksum_data = tyre_id + air_temp + tyre_pressure_0 + tyre_pressure_1 + batt_voltage;

				if(chksum == chksum_data) { 
					tyre_pressure = (tyre_pressure_1 << 8) | tyre_pressure_0;
					
					toggle_red_led();
					scandal_send_channel(TELEM_LOW, tyre_id + TYREMASTER_PRESSURE,
							((uint32_t)tyre_pressure)*2970);
					scandal_send_channel(TELEM_LOW, tyre_id + TYREMASTER_AIR_TEMP,
							((uint32_t)(air_temp-50))*1000);
					scandal_send_channel(TELEM_LOW, tyre_id + TYREMASTER_BATT_VOLTAGE,
							((double)batt_voltage)*18.4+1730);
				} else {
					scandal_send_channel(TELEM_LOW, tyre_id + 4,
						(int)chksum);
					scandal_send_channel(TELEM_LOW, tyre_id + 5,
						(int)chksum_data);
				}
			}
		}

		if(sc_get_timer() >= my_timer + 1000) {
			my_timer = sc_get_timer();
			toggle_yellow_led();
		}

	}

}
