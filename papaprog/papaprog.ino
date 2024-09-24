// PapaLink AVR Device Firmware
// Copyright (C) 2024 DEV Joni / Joni Kemppainen
// SPDX-License-Identifier: GPL-3.0-only
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// DESCRIPTION
// -----------
// Protocol Version 0 (alpha)
//
// Special registers on all pages
//
//   255	Protocol version
//   254	(reserved for N_pages)
//   253	(reserved for i_page)
//   252	I2C address | runtime reconfigurable
//   251	Program name | set by userprog
//   250	Program version | set by userprog
//   249        UUID
//   248	"Read key name" bit | runtime set
//
//   0-247	(free 248 registers for userprog keys)
//
// EEPROM reservations
// 
//   byte 0		I2C address (runtime reconfigurable)
//   byte 1-17 		128-bit UUID address (16 bytes)
//   byte 18-19		(resereved for future use)
//   byte 20-255	(free)
//
//
// For further information:
// https://devjoni.com/wiki/doku.php?id=papalink


const int PLNUM_PROTOCOL_VERSION = 255;
const int PLNUM_I2C_ADDRESS = 252;
const int PLNUM_PROGRAM_NAME = 251;
const int PLNUM_PROGRAM_VERSION = 250;
const int PLNUM_UUID = 249;
const int PLNUM_READKEYNAME_BIT = 248;

#include <EEPROM.h>
#include <Wire.h>

// Change this line to change the uploaded firmware
//
//#include "userprog_skeleton.h"
//#include "userprog_logled.h"
//

// I2C interface settings
#define OSA_PROTOCOL_VERSION 0	// Communication version
#define OSA_I2C_MAXBYTES 3	// Reserved memory for the message buffer

byte osa_i2c_address;
byte osa_uuid[16];

// Reading related variables
int osa_requested_key = -1;
byte osa_readstate = -1;


void _osa_set_i2c_address(byte value) {
	if (value < 1) {
		value = 1;
	} else if (value > 127) {
		value = 127;
	}
	EEPROM.write(0, value);
	osa_i2c_address = value;
}

void setup() {

	osa_i2c_address = EEPROM.read(0);
	if (osa_i2c_address > 127) {
		_osa_set_i2c_address(random(10,120));
	}
	
	// Read UUID from EEPROM and generate one if has not one
	bool has_uuid = false;
	for (int i=0; i<16; i++) {
		osa_uuid[i] = EEPROM.read(i+1);
		// If even one that differs from the untouched 255,
		// then we have an UUID and we settle with it
		if (osa_uuid[i] != 255) {
			has_uuid = true;
		}
	}
	if (has_uuid == false) {
		for (int i=0; i<16; i++) {
			osa_uuid[i] = random(0,255);
			EEPROM.write(i+1, osa_uuid[i]);
		}
	}

	Wire.begin(osa_i2c_address);
	Wire.onReceive(osa_i2c_receive);
	Wire.onRequest(osa_i2c_request);

	osa_setup();
}

void loop() {
	osa_loop();
}

bool is_valid_ikey(int i_key) {
	if (i_key >= osa_N_keys or i_key < 0) {
		return false;
	}
	return true;
}

// When controller sends data to us over i2c
void osa_i2c_receive(int bytes) {

	byte message[OSA_I2C_MAXBYTES+1];
	
	for (int i_byte = 0; i_byte < bytes; i_byte++) {
		byte abyte = Wire.read();
		message[i_byte] = abyte;
	}

	// The 1st byte is the key of the index
	int i_key = message[0];	
	int value = 0;
	
	if (bytes==1) {
		
		// One byte write to READKEYNAME_BIT sets to
		// read to next key fully with the keyname also
		// Requested key has to be set with a further write
		// to the wanted register... ->
		if (i_key == PLNUM_READKEYNAME_BIT) {
			osa_readstate = 0;
		}
		else {
			// -> ... that is here
			osa_requested_key = message[0];
			// If READKEYNAME_BIT not set then set read state
			if (osa_readstate == -1) {
				osa_readstate = 2;
			}
		}
	} else if (bytes==2) {
		if (i_key == PLNUM_I2C_ADDRESS) {
			Wire.end();
			_osa_set_i2c_address(message[1]);
			Wire.begin(osa_i2c_address);
		} else if (is_valid_ikey(i_key)) {
			value = message[1];
			osa_receive(i_key, value);
			//osa_values[i_key] = value;
		}
	} else if (bytes==3) {

		// The 2nd and 3rd byte set the int value
		if (is_valid_ikey(i_key)) {
			value = (((int)message[1])*256) + ((int)message[2]);
			osa_receive(i_key, value);
			//osa_values[i_key] = value;
		}
	}
}


void _osa_finish_read() {
	osa_requested_key = -1;
	osa_readstate = -1;
}

// When master reads data from us over i2c
void osa_i2c_request() {

	// A) SPECIAL KEYS

	// Protocol version
	if (osa_requested_key == PLNUM_PROTOCOL_VERSION) {
		Wire.write(OSA_PROTOCOL_VERSION);
		_osa_finish_read();
		return;
	// I2C address
	} else if (osa_requested_key == PLNUM_I2C_ADDRESS) {
		Wire.write(osa_i2c_address);
		_osa_finish_read();
		return;
	// Program name
	} else if (osa_requested_key == PLNUM_PROGRAM_NAME) {
		if (osa_readstate == 0 || osa_readstate == 2) {
			Wire.write(strlen(OSA_PROG_NAME));
			osa_readstate = 1;
			return;
		} else if (osa_readstate == 1)
			Wire.write(OSA_PROG_NAME);
			_osa_finish_read();
			return;
		return;
	// Program version
	} else if (osa_requested_key == PLNUM_PROGRAM_VERSION) {
		Wire.write(OSA_PROG_VERSION);
		_osa_finish_read();
		return;
	// Device UUID
	} else if (osa_requested_key == PLNUM_UUID) {
		Wire.write(osa_uuid, 16);
		_osa_finish_read();
		return;
	}
	else if (
	(osa_requested_key >= osa_N_keys) or
	(osa_requested_key < 0) or
	(osa_readstate == -1 )) {
		Wire.write(0);
		_osa_finish_read();
		return;
	}
	

	// B) REGULAR KEYS - read by 4 steps if readkeyname_bit is set

	if (osa_readstate == 0) {
		// Write how many bytes the key name
		byte n_bytes = strlen(osa_keys[osa_requested_key]);
		Wire.write(n_bytes);

	} else if (osa_readstate == 1) {
		Wire.write(osa_keys[osa_requested_key]);

	} else if (osa_readstate == 2) {
		byte n_bytes = sizeof(osa_values[osa_requested_key]);
		Wire.write(n_bytes);

	} else if (osa_readstate == 3) {

		// Write bytes
		byte n_bytes = sizeof(osa_values[osa_requested_key]);
		
		if (n_bytes == 1) {
			// uint8
			Wire.write(osa_values[osa_requested_key]);
		} else if (n_bytes == 2) {
			// uint16

			int value = osa_values[osa_requested_key];
			float div = ((float)value) / 256.0;
			byte m1 = value/256;
			byte m2 = 256 * (div-((float)(value/256)));
			
			byte writearray[3] = {m1, m2};
			Wire.write(writearray, n_bytes);
			
		}
		// Reading finishes
		_osa_finish_read();
		return;
	}

	osa_readstate += 1;
}
