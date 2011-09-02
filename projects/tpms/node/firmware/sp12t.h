/* --------------------------------------------------------------------------                                 
    TyreNode SP12T control routines
    File name:sp12t.h 
    Author: Etienne Le Sueur, David Snowdon
    Description: Routines for accessing the SP12T
                 presssure sensor chip.

    Copyright (C) Etienne Le Sueur, David Snowdon, Simon Li 2009. 
    
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

#ifndef __SP12T__
#define __SP12T__

/* -------------------------------------------------------------------------
   Data Structures
   ------------------------------------------------------------------------- */

typedef struct sp12t_data {
	uint16_t pressure;
	uint8_t air_temp;
	uint8_t batt_voltage;
} sp12t_data_t;

/* -------------------------------------------------------------------------
   Function Prototypes
   ------------------------------------------------------------------------- */
void init_tyresensor(void);
void get_sp12t_data(struct sp12t_data *store);

/* -------------------------------------------------------------------------
   COMMAND SET
   ------------------------------------------------------------------------- */
#define SP12T_MEASURE_P		0x8c	// Measure pressure, return command code (reversed)
#define SP12T_RCPDH		0x34	// Read compensated pressure, high byte (reversed)
#define SP12T_RCPDL		0xb4	// Read compensated pressure, low byte (reversed)
#define SP12T_RRPDH		0x10	// Read raw pressure, high byte (reversed)
#define SP12T_RRPDL		0x20	// Read raw pressure, low byte (reversed)
#define SP12T_READIDH		0x3d	// Read ID, high byte
#define SP12T_READIDM		0x38	// Read ID, mid byte
#define SP12T_READIDL		0x34	// Read ID, low byte
#define SP12T_MEASURE_T		0xb5	// Measure temperature, return command code (reversed)
#define SP12T_RCTMP		0x0d	// Read compensated temperature (reversed)
#define SP12T_MEASURE_V		0xA4	// Measure supply voltage, return cc (reversed)
#define SP12T_RCV		0x94	// Read compensated supply voltage (reversed)
#define SP12T_RSR		0xb5	// Read status register
#define SP12T_READ		0xb8
#define SP12T_OTP_PWR		0xf6
#define SP12T_RADDR		0xc1
#define SP12T_STANDBY		0x80	// go into standby (reversed)
#define SP12T_LDOTP		0x3d
#define SP12T_SETWK		0x2e
#define SP12T_GETWK		0x2f

#endif
