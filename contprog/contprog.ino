// PapaLink AVR Controller Firmware
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
// This program runs on a USB Controller that in one end is
// connected to the PapaLink (i2c) bus and the other end to
// a PC with USB. Tested with Arduino Nano Every.
//
// For further information
// https://devjoni.com/wiki/doku.php?id=papalink

#include <Wire.h>

const int SERIAL_BYTES_IN = 6;

void setup() {
	Wire.begin();
	Wire.setTimeout(1000);
	//Serial.begin(115200);
	
	Serial.begin(9600);
	Serial.setTimeout(1000);
}


void loop() {
	byte rbuffer[SERIAL_BYTES_IN+1] = {0};

	int n_in = Serial.readBytes(rbuffer, SERIAL_BYTES_IN);

	if (n_in != SERIAL_BYTES_IN) {
		return;
	}

	byte action = rbuffer[0];
	
	if (action == 1) {
	// normal say
		byte bytestowrite = rbuffer[1];
		byte address = rbuffer[2];
		byte key = rbuffer[3];

		Wire.beginTransmission(address);

		if (bytestowrite == 0) {
			Wire.write(key);
		} else if (bytestowrite == 1) {
			byte out[] = {key, rbuffer[4]};
			Wire.write(out, 2);
		} else if (bytestowrite == 2) {
			byte out[] = {key, rbuffer[4], rbuffer[5]};
			Wire.write(out, 3);
		}

		Wire.endTransmission();

	} else if (action == 2) {
	// scan device and report back

		Wire.setTimeout(10);
		
		byte address = rbuffer[1];
		Wire.beginTransmission(address);
		byte failure = Wire.endTransmission();
		

		Serial.write(failure);
		
		Wire.setTimeout(1000);
	} else if (action == 3) {
	// Ask a value
		
		byte address = rbuffer[1];
		byte key = rbuffer[2];
		byte readkeyname = rbuffer[3];
		Wire.beginTransmission(address);
		
		delay(1);
		if (readkeyname != 0) {
			Wire.write(248);
			delay(1);
		}
		Wire.write(key);
		delay(1);
		
		if (readkeyname != 0) {
			Wire.requestFrom(address, 1, true);
			while (!Wire.available()) {
				delay(1);
			}
			byte bytes_to_read = Wire.read();
			Serial.write(bytes_to_read);
			delay(1);

			byte readbyte;
			
			Wire.requestFrom(address, bytes_to_read, true);
			
			// Read key name
			for (byte i=0; i<bytes_to_read; i++) {
				while (!Wire.available()) {
					delay(1);
				}
				readbyte = Wire.read();
				Serial.write(readbyte);
			}
		}

		// Read key value
		Wire.requestFrom(address, 1, true);
		while (!Wire.available()) {
			delay(1);
		}
	
		bytes_to_read = Wire.read();
		Serial.write(bytes_to_read);	

		Wire.requestFrom(address, bytes_to_read, true);

		for (byte i=0; i<bytes_to_read; i++) {
			while (!Wire.available()) {
				delay(1);
			}
			readbyte = Wire.read();
			Serial.write(readbyte);
		}

		Wire.endTransmission();
	} else if (action == 4 or action == 6) {
	// Get the protocol version: act==4
	// Get program version: act==6
		
		byte address = rbuffer[1];

		Wire.beginTransmission(address);

		if (action == 4) {
			Wire.write(255);
		} else {
			Wire.write(252);
		}	
		delay(1);

		Wire.requestFrom(address, 1, true);
		while (!Wire.available()) {
			delay(1);
		}

		byte readbyte;
		readbyte = Wire.read();
		Serial.write(readbyte);

		Wire.endTransmission();

	} else if (action == 5) {
	// Get program name
		
		byte address = rbuffer[1];

		Wire.beginTransmission(address);
		Wire.write(253);
		delay(1);

		Wire.requestFrom(address, 1);
		while (!Wire.available()) {
			delay(1);
		}

		byte bytes_to_read = Wire.read();
		Serial.write(bytes_to_read);

		Wire.requestFrom(address, bytes_to_read);

		byte readbyte;

		for (byte i=0; i<bytes_to_read; i++) {
			while (!Wire.available()) {
				delay(1);
			}
			readbyte = Wire.read();
			Serial.write(readbyte);
		}

		Wire.endTransmission();
	
	} else if (action == 7) {
	
		byte address = rbuffer[1];

		Wire.beginTransmission(address);
		Wire.write(251);

		Wire.requestFrom(address, 16);
		byte readbyte;
		for (byte i=0; i<16; i++) {
			while (!Wire.available()) {
				delay(1);
			}
			readbyte = Wire.read();
			Serial.write(readbyte);
		}

		Wire.endTransmission();
	}
}
