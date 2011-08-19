/****************************************************************************
 *   $Id:: blinky_main.c 4785 2010-09-03 22:39:27Z nxp21346                        $
 *   Project: LED flashing / ISP test program
 *
 *   Description:
 *     This file contains the main routine for the blinky code sample
 *     which flashes an LED on the LPCXpresso board and also increments an
 *     LED display on the Embedded Artists base board. This project
 *     implements CRP and is useful for testing bootloaders.
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

// define active hardware
#include "driver_config.h"
// include #defines
#include "target_config.h"


#include "timer32.h"
#include "gpio.h"
#include "uart.h"
#include "adc.h"
//#include "scandal.h"


//structural overview
//
//functions 
//--check id, initialise
//--read-voltage, read-temp, calibrate
//receive comms
//send comms
//balance
//timer??

#define DEBUG TRUE

int main (void)
{
	int i = 0, on=0, ID, calb;
  
  
  int voltage = 0;
  
  init();
  ID = readID();
  calb = readCalibration();

  while (1)                                /* Loop forever */
  {
	/* Each time we wake up... */
	/* Check TimeTick to see whether to set or clear the LED I/O pin */
// 		if ( (timer32_0_counter%(LED_TOGGLE_TICKS/COUNT_MAX)) < ((LED_TOGGLE_TICKS/COUNT_MAX)/2) )
// 		{
// 	  		GPIOSetValue( LED_PORT, LED_BIT, LED_OFF );
// 	  		on=0;
// 		} else {
// 	  		GPIOSetValue( LED_PORT, LED_BIT, LED_ON );
// 	  		on=1;		
// 		}
		ADCRead(0);
		if(ADCValue[0] == 0) {
			GPIOSetValue( LED_PORT, LED_BIT, LED_ON );
		//	UARTSend("lol", 3);
		} else if(ADCValue[0] < 512) {
			GPIOSetValue( LED_PORT, LED_BIT, LED_ON );
			on=1;	
		//	UARTSend("b", 1);
		} else {
			GPIOSetValue( LED_PORT, LED_BIT, LED_OFF );
			on=0;
		}
    /* Go to sleep to save power between timer interrupts */
    __WFI();
  }
}

int readVoltage(void) {
	//enable voltage sense mosfet
	GPIOSetValue(VOLT_SENSE_EN_PORT, VOLT_SENSE_EN_PIN, ON);
	//wait?
	//read voltage
	ADCRead(ADC_VOLT);
	//disable voltage sense mosfet
	GPIOSetValue(VOLT_SENSE_EN_PORT, VOLT_SENSE_EN_PIN, OFF);
	
	return ADCValue[ADC_VOLT];	//error checking?
}

int readTemperature(void) {
	//enable voltage sense mosfet
	GPIOSetValue(TEMP_SENSE_EN_PORT, TEMP_SENSE_EN_PIN, ON);
	//wait?
	//read voltage
	ADCRead(ADC_TEMP);
	//disable voltage sense mosfet
	GPIOSetValue(TEMP_SENSE_EN_PORT, TEMP_SENSE_EN_PIN, OFF);
	
	return ADCValue[ADC_TEMP];	//error checking?
}

int readCalibration(void) {
	ADCRead(ADC_CALB);
	return ADCValue[ADC_CALB];
}

//discharge mode = time || voltage, argument = time (s) || voltage (Volts)
int discharge(int argument, int mode) { 
	//start discharger
	//set timer interrupt
	//stop discharge
	GPIOSetValue(DISCHARGE_PORT, DISCHARGE_PIN, ON);
	//delay for time, or perhaps should specify a voltage to discharge to?
	GPIOSetValue(DISCHARGE_PORT, DISCHARGE_PIN, OFF);
	
}

int readID(void) {
    //determine ID based on 6 GPIO pins, is it possible to use the unique micro ID
    int ID = 0;
 /*   ID |= (gpioGetValue(ID_PORT, ID0_PIN));
    ID |= (gpioGetValue(ID_PORT, ID1_PIN)<<1);
    ID |= (gpioGetValue(ID_PORT, ID2_PIN)<<2);
    ID |= (gpioGetValue(ID_PORT, ID3_PIN)<<3);
    ID |= (gpioGetValue(ID_PORT, ID4_PIN)<<4);
    ID |= (gpioGetValue(ID_PORT, ID5_PIN)<<5);
 */   
}

void burstmode(int mode) {
	GPIOSetValue(BURSTMODE_PORT, BURSTMODE_PIN, mode);
}

int init(void) 
{
	GPIOSetDir(ID_PORT, ID0_PIN, INPUT);
	GPIOSetDir(ID_PORT, ID1_PIN, INPUT);
	GPIOSetDir(ID_PORT, ID2_PIN, INPUT);
	GPIOSetDir(ID_PORT, ID3_PIN, INPUT);
	GPIOSetDir(ID_PORT, ID4_PIN, INPUT);
	GPIOSetDir(ID_PORT, ID5_PIN, INPUT);
	
	GPIOSetDir(BURSTMODE_PORT, BURSTMODE_PIN, OUTPUT);
	GPIOSetDir(CAN_EN_PORT, CAN_EN_PIN, OUTPUT);
	GPIOSetDir(DISCHARGE_PORT, DISCHARGE_PIN, OUTPUT);
	GPIOSetDir(VOLT_SENSE_EN_PORT, VOLT_SENSE_EN_PIN, OUTPUT);
	GPIOSetDir(TEMP_SENSE_EN_PORT, TEMP_SENSE_EN_PIN, OUTPUT);
	GPIOSetDir(ORANGE_LED_PORT, ORANGE_LED_PIN, OUTPUT);
	GPIOSetDir(ORANGE_LED_PORT, ORANGE_LED_PIN, OUTPUT);
	GPIOSetDir(RED_LED_PORT, RED_LED_PIN, OUTPUT);
	GPIOSetDir(WAKEUP_PORT, WAKEUP_PIN, INPUT);
	
  
	ADCInit(1);
	GPIOSetDir( LED_PORT, LED_BIT, 1 );
 
   
   /* Basic chip initialization is taken care of in SystemInit() called
   * from the startup code. SystemInit() and chip settings are defined
   * in the CMSIS system_<part family>.c file.
   */

  /* Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS */
  /* You may also want to use the Cortex SysTick timer to do this */
	init_timer32(0, TIME_INTERVAL);
  /* Enable timer 0. Our interrupt handler will begin incrementing
   * the TimeTick global each time timer 0 matches and resets.
   */
	
	UARTInit(9600);
	enable_timer32(0);


  /* Initialize GPIO (sets up clock) */
	GPIOInit();
  /* Set LED port pin to output */
  
   
   
}
