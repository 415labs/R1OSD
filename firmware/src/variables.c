/* variables.c */

#include "variables.h"
#include "avr/eeprom.h"



OSDParameters_t OSDParameters;




void getAllSettingsFromEEPROM() {
	// Get 8 bits parameters
	OSDParameters.loggingExportFormat = eeprom_read_byte((uint8_t*)0x01);
	OSDParameters.loggingInterval = eeprom_read_byte((uint8_t*)0x02);
	OSDParameters.loggerMode = eeprom_read_byte((uint8_t*)0x03);
	// OSDParameters.debugEnabled = eeprom_read_byte((uint8_t*)0x04);
	OSDParameters.OSDDisplayMode = eeprom_read_byte((uint8_t*)0x05);
	OSDParameters.minSatForFix = eeprom_read_byte((uint8_t*)0x06);
	OSDParameters.minHDOPForFix = eeprom_read_byte((uint8_t*)0x07);
	OSDParameters.minVDOPForFix = eeprom_read_byte((uint8_t*)0x08);
	OSDParameters.warningsEnabled = eeprom_read_byte((uint8_t*)0x09);
	OSDParameters.currentSensorConnected = eeprom_read_byte((uint8_t*)0x0A);



	// Now read 16 bits parameters
	OSDParameters.gpsBaudrate = eeprom_read_word((uint16_t*)0x0B);
	OSDParameters.warningMaxDistance = eeprom_read_word((uint16_t*)0x0D);
	OSDParameters.warningMaxAltitude = eeprom_read_word((uint16_t*)0x0F);
	OSDParameters.warningMaxSpeed = eeprom_read_word((uint16_t*)0x11);
	OSDParameters.warningMaxCurrent = eeprom_read_word((uint16_t*)0x13);
	OSDParameters.warningMaxCapacity = eeprom_read_word((uint16_t*)0x15);
	OSDParameters.warningMinVoltage = eeprom_read_word((uint16_t*)0x17);

	OSDParameters.minHDOPForValidData = eeprom_read_byte((uint8_t*)0x19);

}




void saveAllSettingsToEEPROM() {


	// Store 8 bits parameters
	eeprom_write_byte((uint8_t*)0x01, OSDParameters.loggingExportFormat);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x02, OSDParameters.loggingInterval);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x03, OSDParameters.loggerMode);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x04, OSDParameters.debugEnabled);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x05, OSDParameters.OSDDisplayMode);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x06, OSDParameters.minSatForFix);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x07, OSDParameters.minHDOPForFix);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x08, OSDParameters.minVDOPForFix);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x09, OSDParameters.warningsEnabled);
	while(eeprom_is_ready()==0);

	eeprom_write_byte((uint8_t*)0x0A, OSDParameters.currentSensorConnected);
	while(eeprom_is_ready()==0);

	// Now store 16 bits parameters
	eeprom_write_word((uint16_t*)0x0B, OSDParameters.gpsBaudrate);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x0D, OSDParameters.warningMaxDistance);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x0F, OSDParameters.warningMaxAltitude);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x11, OSDParameters.warningMaxSpeed);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x13, OSDParameters.warningMaxCurrent);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x15, OSDParameters.warningMaxCapacity);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x17, OSDParameters.warningMinVoltage);
	while(eeprom_is_ready()==0);

	eeprom_write_word((uint16_t*)0x19, OSDParameters.minHDOPForValidData);
	while(eeprom_is_ready()==0);
}
