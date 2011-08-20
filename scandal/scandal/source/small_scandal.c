/* --------------------------------------------------------------------------
	Small Scandal
	File name: small_scandal.c
	Author: David Snowdon

	Date: Friday, 6 September 2002

    Copyright (C) David Snowdon 2002. 
 
	Implements a limited subset of the scandal protocol in order to fit into a
	very small microcontroller.
   -------------------------------------------------------------------------- */

/* 
 * This file is part of Scandal.
 * 
 * Scandal is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 
 * Scandal is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with Scandal.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scandal_engine.h"
#include "scandal_types.h"
#include "scandal_timer.h"
#include "scandal_led.h"
#include "scandal_error.h"
#include "scandal_config.h"
#include "scandal_message.h"

u32	heartbeat_timer;

/* Functions */
u08 scandal_init(void){
	init_can();
	sc_init_timer();

	heartbeat_timer = 0;

	return(0);
}

/*! \todo To be implemented */
u08 scandal_get_addr(void){
	return(SCANDAL_DEFAULT_ADDRESS);
}

/*! \todo To be implemented */
u32 scandal_get_mac(void){
	return(1);
}

/* Handle Scandal - to be called regularly (assumed to be once in the main loop)
	Will do nothing in the case where there is nothing to do */
void handle_scandal(void){
	can_poll();

	/* Check weather we're due to send a heartbeat, and if so, send it */
	if(sc_get_timer() - heartbeat_timer > HEARTBEAT_PERIOD){
		scandal_send_heartbeat(0);	/*! \todo Send a more useful status */
		heartbeat_timer = sc_get_timer();
	}
}
