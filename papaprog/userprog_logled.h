// PapaLink AVR Device LogLED Firmware
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
// This program is designed to run on an Arduino Nano Every board
// inside a LogLED device.
//
// Note: EEPROM has limited write cycles (~10,000?).
// EEPROM for power-off remaining settings
//
// byte 22	Is BNC mode active
// 			0 off
// 			1 on (run led from BNCs)
// byte 23	Is safe mode on
// 			0 off
// 			1 on prevent burning
// byte 24	Number of bits in a driver
// byte 25	High bit digital-out index (D_HIGH)
// byte 26	Number of drivers
//
// For further information
// https://devjoni.com/wiki/doku.php?id=papalink
// https://devjoni.com/wiki/doku.php?id=logled


#define OSA_PROG_NAME "dj-logled-311"
//#define OSA_PROG_NAME "dj-logled-116"
//#define OSA_PROG_NAME "dj-logled-111"
//#define OSA_PROG_NAME "dj-logled-103"

#define OSA_PROG_VERSION 1

#include <EEPROM.h>

// Exposed keys and values
const int osa_N_keys = 13;
const char *osa_keys[osa_N_keys] = 
	{"brightness", "_savechanges", "bnc", "safemode",
		"_nbits", "_hbit", "_ndrivs"};
  int osa_values[osa_N_keys] = {0, 0, 0, 1, 11, 12, 3};
//int osa_values[osa_N_keys] = {0, 0, 0, 1, 16, 12, 1};
//int osa_values[osa_N_keys] = {0, 0, 0, 1, 11, 12, 1};
//int osa_values[osa_N_keys] = {0, 0, 0, 1, 3, 12, 1};

const int KNUM_BRIGHTNESS = 0;
const int KNUM_SAVECHANGES = 1;
const int KNUM_BNC = 3;
const int KNUM_SAFEMODE = 4;
const int KNUM_NBITS = 5;
const int KNUM_HBIT = 6;
const int KNUM_NDRIVS = 7;

// Called from the Arduino setup()
void osa_setup() {
	
	if (2 <= osa_values[KNUM_NDRIVS] <= 3) {
		pinMode(A7, OUTPUT);
		digitalWrite(A7, LOW);
	}
	if (osa_values[KNUM_NDRIVS] == 3) {
		pinMode(A6, OUTPUT);
		digitalWrite(A6, LOW);
	}

	// All except brightness and savechanges read from EEPROM at init
	for (int i=2; i<osa_N_keys; i++) {
		byte value = EEPROM.read(20+i);
		
		// Unmodified values are 255 in EEPROM. If got one 255,
		// then keep the initial value
		if (value != 255) {
			osa_values[i] = (int)value;
		}
	}
	
	// Initialize digital out
	int N_bits = osa_values[KNUM_NBITS];
	int high_bit = osa_values[KNUM_HBIT];
	for (int i=0; i<N_bits; i++) {
		int pin = i+(high_bit+1-N_bits);
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}


int to_driverval(int in0, int in1) {
	int out = (in0-512) + (in1-10) * 16;
	if (out < 0) {
		out = 0;
	} else if (out > 8191) {
		out = 8191;
	}
	return out;
}

// Regular full scale set function
void logled_regular_set(int value) {
	
	// Logled 311, 11 bit drivers 3x
	int N_bits = osa_values[KNUM_NBITS];
	int high_bit = osa_values[KNUM_HBIT];
	int N_drivers = osa_values[KNUM_NDRIVS];
	int kopio = value;

	for (int i=0; i<N_bits; i++) {
		int pin = i+(high_bit+1-N_bits);
		if (kopio & 1 == 1) {
			digitalWrite(pin, HIGH);
		} else {
			digitalWrite(pin, LOW);
		}
		kopio = kopio / 2;
	}
	
	if (N_drivers >= 2) {
		if (kopio & 1 == 1) {
			digitalWrite(A6, HIGH);
		} else {
			digitalWrite(A6, LOW);
		}
	}

	if (N_drivers >= 3) {
		kopio = kopio / 2;
		if (kopio & 1 == 1) {
			digitalWrite(A7, HIGH);
		} else {
			digitalWrite(A7, LOW);
		}
	}
	osa_values[0] = value;
}



void osa_loop() {
	
	// Loop only neede for the BNC operation mode
	if (osa_values[KNUM_BNC] == 0) {
		return;
	}

	// Regular input mode, do two analogue reads
	int in0 = analogRead(A2); // val
	int in1 = analogRead(A3); // gain
	
	logled_regular_set(
		to_driverval(in0, in1)
	);
}



// Called from the I2C receive function
void osa_receive(int i_key, int value) {
	
	if (i_key == 0) {
		// Set LED
		logled_regular_set(value);
		// osa_values[0] written at set
		return;

	} else if (i_key >= 2 and osa_values[1] > 0) {
		// Write keys to EEPROM if savechanges on
		EEPROM.write(20+i_key, value);
	}
	// All keys write to the table
	// Controller/PC side has to make sure no craziness
	// is being written
	osa_values[i_key] = value;
}


