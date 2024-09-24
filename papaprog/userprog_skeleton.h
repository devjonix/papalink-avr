// PapaLink AVR Device {PRODUCT} Firmware
// Copyright (C) {YEAR} {AUTHOR_NAME}
//
// DESCRIPTION
// -----------
// This skeleton file is a good starting point for creating your
// own PapaLink compatible firmware
// 
// Tips for PapaLink protocol V1:
// - papaprog reservations
// 	* EEPROM bytes 0-19 taken (20-255 free)
// 	* osa_keys/osa_values 248-255 taken (0-247 free)
//
// For further reading
// {links to some website or resource explaining more}

// Name of this program
#define OSA_PROG_NAME "CREATOR-PRODUCT-MODEL"

// Version of this program
#define OSA_PROG_VERSION 1

// Include EEPROM if needs permanent settings for poweroff
//#include <EEPROM.h>


// Exposed key/value pairs over the protocol
const int osa_N_keys = 2;
const char *osa_keys[osa_N_keys] = {"temp1", "humid1"};
int osa_values[osa_N_keys] = {0, 0};


// Similar to arduino's setup()
void osa_setup() {
	// Here initialize digital out etc. maybe load EEPROM	
}


// Similar to arduino's loop()
void osa_loop() {
	// You may want to check osa_values here
	// and act accordingly (or then do nothing)
}



// Called from the I2C receive function
void osa_receive(int i_key, int value) {
	
	// Here you want to act on osa_values changes
	// at the time they are made. Cannot spend too
	// much time here or it causes I2C troubles.
	//
	// All keys good to write to the osa table
	// Driver/PC side to make sure no craziness is being written
	osa_values[i_key] = value;
}


