/****************************************************************************
 *   $Id:: i2c_main.c 4785 2010-09-03 22:39:27Z nxp21346                    $
 *   Project: NXP LPC11xx I2C example
 *
 *   Description:
 *     This file contains I2C test modules, main entry, to test I2C APIs.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#include <project/driver_config.h>
#include <project/target_config.h>

#include <arch/timer32.h>
#include <arch/gpio.h>
#include <arch/uart.h>
#include <arch/type.h>
#include <arch/can.h>
#include <arch/i2c.h>
#include <math.h>

#include <scandal/timer.h>

void setup_ports(void) {
	// Initializing the GPIO's
	GPIOInit();

	// LEDs
	GPIOSetDir(2,8,1); //Green LED, Out
	//GPIOSetValue(2,8,0); //Green LED, Low (on)
	//GPIOSetValue(2,8,1); //Green LED, High (off)

	GPIOSetDir(2,7,1); //Yel LED, Out
	//GPIOSetValue(2,7,0); //Yel LED, Low (on)
	//GPIOSetValue(2,7,1); //Yel LED, Low (off)
}

int main(void)
{
	sc_time_t my_timer = sc_get_timer();

	setup_ports();

	scandal_init();

	UARTInit(115200); //Init UART at 115200bps

	red_led(1);
	yellow_led(1);

	int i = 0;

	UART_printf("Welcome to the template project! This is coming out over UART1\n\r");
	UART_printf("The 2 debug LEDs should blink at a rate of 1HZ\n\r");

	while (1) {
		if(sc_get_timer() >= my_timer + 1000) {
			UART_printf("This is the 1 second timer... %d\n\r", i++);
			my_timer = sc_get_timer();
			toggle_yellow_led();
			toggle_red_led();
		}
	}
}
