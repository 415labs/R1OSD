#include "rc5.h"
#include "max7456_software_spi.h"
#include "menus.h"
#include "delay_utilities.h"
#include <avr/wdt.h>
#include "variables.h"
#include "string_utilities.h"
#include "avr/eeprom.h"
#include <avr/pgmspace.h>


// store stats description in progmem to save space
const char mainMenu_0[] PROGMEM = "Main menu";
const char mainMenu_1[] PROGMEM = " OSD Settings...";
const char mainMenu_2[] PROGMEM = " GPS Logger settings...";
const char mainMenu_3[] PROGMEM = " Statistics...";
const char mainMenu_4[] PROGMEM = " Tools...";
const char mainMenu_5[] PROGMEM = " EXIT";
const char * const mainMenu_ptr[6] PROGMEM = {mainMenu_0, mainMenu_1, mainMenu_2, mainMenu_3, mainMenu_4, mainMenu_5};


const char OSDsettingsMenu_0[] PROGMEM = "OSD Settings";
const char OSDsettingsMenu_1[] PROGMEM = " OSD mode:";
const char OSDsettingsMenu_2[] PROGMEM = " Warnings...";
const char OSDsettingsMenu_3[] PROGMEM = " BACK";
const char * const OSDsettingsMenu_ptr[4] PROGMEM = {OSDsettingsMenu_0, OSDsettingsMenu_1, OSDsettingsMenu_2, OSDsettingsMenu_3 };


const char OSDMode_0[] PROGMEM = "disabled  ";
const char OSDMode_1[] PROGMEM = "minimalist";
const char OSDMode_2[] PROGMEM = "standard  ";
const char OSDMode_3[] PROGMEM = "enhanced  ";
const char * const OSDMode_ptr[4] PROGMEM = {OSDMode_0, OSDMode_1, OSDMode_2, OSDMode_3 };


const char warnings_0[] PROGMEM = "Warnings (max)";
const char warnings_1[] PROGMEM = " altitude";
const char warnings_2[] PROGMEM = " distance";
const char warnings_3[] PROGMEM = " speed";
const char warnings_4[] PROGMEM = " current";
const char warnings_5[] PROGMEM = " voltage";
const char warnings_6[] PROGMEM = " battery";
const char warnings_7[] PROGMEM = " warnings";
const char warnings_8[] PROGMEM = " reset all values";
const char warnings_9[] PROGMEM = " BACK";
const char * const warnings_ptr[10] PROGMEM = {warnings_0, warnings_1, warnings_2, warnings_3, warnings_4, warnings_5, warnings_6, warnings_7, warnings_8, warnings_9 };


const char GPSLoggerMenu_0[] PROGMEM = "GPS Logger";
const char GPSLoggerMenu_1[] PROGMEM = " Interval";
const char GPSLoggerMenu_2[] PROGMEM = " Format";
const char GPSLoggerMenu_3[] PROGMEM = " Mode";
const char GPSLoggerMenu_4[] PROGMEM = " Format SD";
const char GPSLoggerMenu_5[] PROGMEM = " BACK";
const char * const GPSLoggerMenu_ptr[6] PROGMEM = {GPSLoggerMenu_0, GPSLoggerMenu_1, GPSLoggerMenu_2, GPSLoggerMenu_3, GPSLoggerMenu_4, GPSLoggerMenu_5};



const char toolsMenu_0[] PROGMEM = "Tools";
const char toolsMenu_1[] PROGMEM = " IR remode key display";
const char toolsMenu_2[] PROGMEM = " Load default settings";
const char toolsMenu_3[] PROGMEM = " BACK";
const char * const toolsMenu_ptr[4] PROGMEM = {toolsMenu_0, toolsMenu_1, toolsMenu_2, toolsMenu_3};




OSDParameters_t OSDParameters;

// ---------------------------------------------------------------------------------------------
// Check if the user switched the navbutton to one of the available positions: UP, DOWN or ENTER
// Also check if the used pressed a key on the IR remote.
//----------------------------------------------------------------------------------------------
char checkInputRequestFromUser()
{
char key = 0;

 if (navbutton.flip >= 0) 		// Did the user pressed an IR remote key or navbutton ?
 {
   delay_ms(NAVBUTTON_DELAY);
   key = navbutton.key;
   navbutton.flip = -1;
 }


  if (rc5.flip >= 0)
  {
    delay_ms(IR_KEY_DELAY);
    rc5.flip = -1;
    key = rc5.code;
  }

  return key;
}





void formatSDCard() {
	clear();
	write_ascii_string_pgm(startX,  startY, PSTR("TBD: SDCARD"));
	delay_ms(2000);
	clear();
}


void displayGPSLoggerMenu(void) {
char inMenu = 1;
char indexPos = 1;
char updateDisplay = 1;
char parametersHaveChanged = 0;


clear();
while (inMenu)
	{

	if (updateDisplay) {
    	write_ascii_string_pgm(startX,  startY-1, (char *)GPSLoggerMenu_ptr[0]);
        write_ascii_string_pgm(startX,  startY+1, (char *)GPSLoggerMenu_ptr[1]);
        write_ascii_string_pgm(startX,  startY+2, (char *)GPSLoggerMenu_ptr[2]);
        write_ascii_string_pgm(startX,  startY+3, (char *)GPSLoggerMenu_ptr[3]);
        write_ascii_string_pgm(startX,  startY+4, (char *)GPSLoggerMenu_ptr[4]);
    	write_ascii_string_pgm(startX,  startY+5, (char *)GPSLoggerMenu_ptr[5]);
		write_ascii_string_pgm(startX,  indexPos+startY, PSTR(">"));
        write_ascii_string(startX+12,  startY+1, (char *)itoaPadding(OSDParameters.loggingInterval, 2, '0'));
		write_ascii_string_pgm(startX+14,  startY+1, PSTR("s"));
		if (OSDParameters.loggingExportFormat == 1) { write_ascii_string_pgm(startX+12,  startY+2, PSTR("KML (Google E.)")); } else { write_ascii_string_pgm(startX+12,  startY+2, PSTR("NMEA           ")); }
		if (OSDParameters.loggerMode == 1) {write_ascii_string_pgm(startX+12,  startY+3, PSTR("Always on   ")); } else {write_ascii_string_pgm(startX+12,  startY+3, PSTR("Fly sessions")); }
		updateDisplay = 0;
	}


	     switch (checkInputRequestFromUser() ) {
		 	case DOWN_KEY:
			  if (indexPos < 5) {
			  indexPos++;
			  } else { indexPos = 1; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 1) {
			  indexPos--;
			  } else { indexPos = 5; }
			  updateDisplay = 1;
            break;


		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 1) { parametersHaveChanged=1; if (OSDParameters.loggingInterval < 60) { OSDParameters.loggingInterval++;} else {OSDParameters.loggingInterval = 1;}  }
			  if (indexPos == 2) { parametersHaveChanged=1;  if (OSDParameters.loggingExportFormat == 1) { OSDParameters.loggingExportFormat = 2;} else {OSDParameters.loggingExportFormat = 1;}  }
			  if (indexPos == 3) { parametersHaveChanged=1;  if (OSDParameters.loggerMode == 1) { OSDParameters.loggerMode = 2;} else {OSDParameters.loggerMode = 1;}  }
			  if (indexPos == 4) { formatSDCard();  }
			  if (indexPos == 5) {
			  		inMenu = 0;
					if (parametersHaveChanged) {
						saveAllSettingsToEEPROM();
					}
					displayMenu_Main();
				}
			  updateDisplay = 1;
            break;


		}
    wdt_reset();
	}
	clear();
}


void resetWarningsValues() {
	OSDParameters.warningMaxDistance = 0;
	OSDParameters.warningMaxAltitude = 0;
	OSDParameters.warningMaxSpeed = 0;
	OSDParameters.warningMaxCurrent = 0;
	OSDParameters.warningMaxCapacity = 0;
	OSDParameters.warningMinVoltage = 0;
}



void displayWarningsMenu(void) {
char inMenu = 1;
char indexPos = 1;
char updateDisplay = 1;
char parametersHaveChanged = 0;

clear();
while (inMenu)
	{
	if (updateDisplay) {
    	write_ascii_string_pgm(startX,  startY-1, (char *)warnings_ptr[0]);
        write_ascii_string_pgm(startX,  startY+1, (char *)warnings_ptr[1]);
        write_ascii_string_pgm(startX,  startY+2, (char *)warnings_ptr[2]);
        write_ascii_string_pgm(startX,  startY+3, (char *)warnings_ptr[3]);
        write_ascii_string_pgm(startX,  startY+4, (char *)warnings_ptr[4]);
    	write_ascii_string_pgm(startX,  startY+5, (char *)warnings_ptr[5]);
        write_ascii_string_pgm(startX,  startY+6, (char *)warnings_ptr[6]);
    	write_ascii_string_pgm(startX,  startY+7, (char *)warnings_ptr[7]);
		write_ascii_string_pgm(startX,  startY+8, (char *)warnings_ptr[8]);
		write_ascii_string_pgm(startX,  startY+9, (char *)warnings_ptr[9]);

		if (OSDParameters.warningsEnabled == 0)
				{ write_ascii_string_pgm(startX+21,  startY+7, PSTR("OFF")); } else {  write_ascii_string_pgm(startX+21,  startY+7, PSTR("ON "));}

		write_ascii_string_pgm(startX,  indexPos+startY, PSTR(">"));

        write_ascii_string(startX+15,  startY+1, (char *)itoaPadding(OSDParameters.warningMaxAltitude, 5, ' '));
        write_ascii_string(startX+15,  startY+2, (char *)itoaPadding(OSDParameters.warningMaxDistance, 5, ' '));
        write_ascii_string(startX+15,  startY+3, (char *)itoaPadding(OSDParameters.warningMaxSpeed, 5, ' '));
		write_ascii_string(startX+15,  startY+4, (char *)itoaPadding(OSDParameters.warningMaxCurrent, 5, ' '));
        write_ascii_string(startX+15,  startY+5, (char *)itoaPadding(OSDParameters.warningMinVoltage, 5, ' '));
        write_ascii_string(startX+15,  startY+6, (char *)itoaPadding(OSDParameters.warningMaxCapacity, 5, ' '));

		write_ascii_string_pgm(startX+21,  startY+1, PSTR("m"));
		write_ascii_string_pgm(startX+21,  startY+2, PSTR("m"));
		write_ascii_string_pgm(startX+21,  startY+3, PSTR("kph"));
		write_ascii_string_pgm(startX+21,  startY+4, PSTR("A"));
		write_ascii_string_pgm(startX+21,  startY+5, PSTR("V"));
		write_ascii_string_pgm(startX+21,  startY+6, PSTR("mA"));

		updateDisplay = 0;
		}




	     switch (checkInputRequestFromUser() ) {
		 	case DOWN_KEY:
			  if (indexPos < 9) {
			  indexPos++;
			  } else { indexPos = 1; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 1) {
			  indexPos--;
			  } else { indexPos = 9; }
			  updateDisplay = 1;
            break;


		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 1) { parametersHaveChanged = 1;  if (OSDParameters.warningMaxAltitude < 5000) { OSDParameters.warningMaxAltitude = OSDParameters.warningMaxAltitude + 50;} else {OSDParameters.warningMaxAltitude = 50;}  }
			  if (indexPos == 2) { parametersHaveChanged = 1; if (OSDParameters.warningMaxDistance < 50000) { OSDParameters.warningMaxDistance = OSDParameters.warningMaxDistance + 100;} else {OSDParameters.warningMaxDistance = 100;}  }
			  if (indexPos == 3) { parametersHaveChanged = 1; if (OSDParameters.warningMaxSpeed < 500) { OSDParameters.warningMaxSpeed = OSDParameters.warningMaxSpeed + 10;} else {OSDParameters.warningMaxSpeed = 10;}  }
			  if (indexPos == 4) { parametersHaveChanged = 1; if (OSDParameters.warningMaxCurrent < 50) { OSDParameters.warningMaxCurrent++;} else {OSDParameters.warningMaxCurrent = 1;}  }
			  if (indexPos == 5) { parametersHaveChanged = 1; if (OSDParameters.warningMinVoltage < 30) { OSDParameters.warningMinVoltage++;} else {OSDParameters.warningMinVoltage = 1;}  }
			  if (indexPos == 6) { parametersHaveChanged = 1; if (OSDParameters.warningMaxCapacity < 10000) { OSDParameters.warningMaxCapacity = OSDParameters.warningMaxCapacity + 100;} else {OSDParameters.warningMaxCapacity = 100; }  }
			  if (indexPos == 7) { parametersHaveChanged = 1; if (OSDParameters.warningsEnabled == 0) { OSDParameters.warningsEnabled = 1; } else { OSDParameters.warningsEnabled = 0; } }
			  if (indexPos == 8) { parametersHaveChanged = 1; resetWarningsValues(); }
			  if (indexPos == 9) {
			  	inMenu = 0;
				if (parametersHaveChanged) {
					saveAllSettingsToEEPROM();
				}
			  }
			  updateDisplay = 1;
            break;
		}
  	wdt_reset();
	}
	clear();
}



void resetToDefaultValues() {
	OSDParameters.gpsBaudrate = DEFAULT_GPSBAUDRATE;
	OSDParameters.OSDDisplayMode = DEFAULT_OSDDISPLAYMODE;
	OSDParameters.loggingExportFormat = DEFAULT_LOGGINGEXPORTFORMAT;
	OSDParameters.loggerMode = DEFAULT_LOGGERMODE;
	OSDParameters.loggingInterval = DEFAULT_LOGGINGINTERVAL;
	OSDParameters.warningMaxDistance = DEFAULT_MAXDISTANCE;
	OSDParameters.warningMaxAltitude = DEFAULT_MAXALTITUDE;
	OSDParameters.warningMaxCapacity = DEFAULT_MAXCAPACITY;
	OSDParameters.warningMinVoltage = DEFAULT_MINVOLTAGE;
	OSDParameters.warningMaxCurrent = DEFAULT_MAXCURRENT;
	OSDParameters.warningMaxSpeed = DEFAULT_MAXSPEED;
	OSDParameters.minSatForFix = DEFAULT_MINSATFORFIX;
	OSDParameters.minHDOPForFix = DEFAULT_MINHDOPFORFIX;
	OSDParameters.minVDOPForFix = DEFAULT_MINVDOPFORFIX;
	OSDParameters.warningsEnabled = DEFAULT_WARNINGSENABLED;
}

void viewGPSData() {
// TODO
	write_ascii_string_pgm(10,  10, PSTR("TO BE DONE"));
}





void displayOSDSettingsMenu(void) {
char inMenu = 1;
char indexPos = 1;
char updateDisplay = 1;
char parametersHaveChanged = 0;



clear();
while (inMenu)
	{

	if (updateDisplay) {
    	write_ascii_string_pgm(startX,  startY-1, (char *)OSDsettingsMenu_ptr[0]);
        write_ascii_string_pgm(startX,  startY+1, (char *)OSDsettingsMenu_ptr[1]);
        write_ascii_string_pgm(startX,  startY+2, (char *)OSDsettingsMenu_ptr[2]);
        write_ascii_string_pgm(startX,  startY+3, (char *)OSDsettingsMenu_ptr[3]);

		write_ascii_string_pgm(startX,  indexPos+startY, PSTR(">"));


					if (OSDParameters.OSDDisplayMode == 1) {
				    	write_ascii_string_pgm(startX+11,  startY+1, (char *)OSDMode_ptr[0]);
					}

					if (OSDParameters.OSDDisplayMode == 2) {
				    	write_ascii_string_pgm(startX+11,  startY+1, (char *)OSDMode_ptr[1]);
					}

					if (OSDParameters.OSDDisplayMode == 3) {
				    	write_ascii_string_pgm(startX+11,  startY+1, (char *)OSDMode_ptr[2]);
					}

					if (OSDParameters.OSDDisplayMode == 4) {
				    	write_ascii_string_pgm(startX+11,  startY+1, (char *)OSDMode_ptr[3]);
					}
		updateDisplay = 0;
		}


	     switch (checkInputRequestFromUser() ) {
		 	case DOWN_KEY:
			  if (indexPos < 3) {
			  indexPos++;
			  } else { indexPos = 1; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 1) {
			  indexPos--;
			  } else { indexPos = 3; }
			  updateDisplay = 1;
            break;


		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 1) {
			    if (OSDParameters.OSDDisplayMode < 4) { OSDParameters.OSDDisplayMode++; } else { OSDParameters.OSDDisplayMode = 1; }
			    parametersHaveChanged = 1;
			  updateDisplay = 1;
			}

			  if (indexPos == 2) { displayWarningsMenu(); }
			  if (indexPos == 3) {
			  		inMenu = 0;
					if (parametersHaveChanged) {
						saveAllSettingsToEEPROM();
					}
					displayMenu_Main();
				}
			  updateDisplay = 1;
            break;
		}
    wdt_reset();
	}
	clear();
}



/*
void displayFlightStatisticsMenu(void) {
char inMenu = 1;
char indexPos = 9;


clear();
while (inMenu)
	{
    	write_ascii_string(startX,  startY-1, flightStatisticsMenu[0]);
        write_ascii_string(startX,  startY+1, flightStatisticsMenu[1]);
        write_ascii_string(startX,  startY+2, flightStatisticsMenu[2]);
        write_ascii_string(startX,  startY+3, flightStatisticsMenu[3]);
        write_ascii_string(startX,  startY+4, flightStatisticsMenu[4]);
    	write_ascii_string(startX,  startY+5, flightStatisticsMenu[5]);
        write_ascii_string(startX,  startY+6, flightStatisticsMenu[6]);
        write_ascii_string(startX,  startY+7, flightStatisticsMenu[7]);
        write_ascii_string(startX,  startY+9, flightStatisticsMenu[8]);
        write_ascii_string(startX,  startY+10, flightStatisticsMenu[9]);

		write_ascii_string(startX,  indexPos+startY, ">");

	// New RC5 frame arrived?
       if (rc5.flip >= 0) {
		 delay_ms(IR_KEY_DELAY);
		 rc5.flip = -1; // Reset RC5.flip status


	     switch (rc5.code) {
		 	case DOWN_KEY:
			  if (indexPos < 10) {
			  indexPos++;
			  } else { indexPos = 9; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 9) {
			  indexPos--;
			  } else { indexPos = 10; }
			  updateDisplay = 1;
            break;

		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 9) { ;; }		// TODO: Call displayStats
			  if (indexPos == 10) { inMenu = 0; displayMenu_Main(); }
			  updateDisplay = 1;
            break;
	   	 }
		}
    wdt_reset();
	}
	clear();
}
*/



void displayMenu_Main() {
char inMenu = 1;
char indexPos = 1;
char updateDisplay = 1;


clear();
while (inMenu)
	{

	if (updateDisplay) {
    	write_ascii_string_pgm(startX,  startY-1, (char *)mainMenu_ptr[0]);
        write_ascii_string_pgm(startX,  startY+1, (char *)mainMenu_ptr[1]);
        write_ascii_string_pgm(startX,  startY+2, (char *)mainMenu_ptr[2]);
        write_ascii_string_pgm(startX,  startY+3, (char *)mainMenu_ptr[3]);
        write_ascii_string_pgm(startX,  startY+4, (char *)mainMenu_ptr[4]);
        write_ascii_string_pgm(startX,  startY+5, (char *)mainMenu_ptr[5]);
		write_ascii_string_pgm(startX,  indexPos+startY, PSTR(">"));
		updateDisplay = 0;
	}


	     switch (checkInputRequestFromUser() ) {
		 	case DOWN_KEY:
			  if (indexPos < 5) {
			  indexPos++;
			  } else { indexPos = 1; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 1) {
			  indexPos--;
			  } else { indexPos = 5; }
			  updateDisplay = 1;
            break;

		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 1) { inMenu = 0; displayOSDSettingsMenu();; }
			  if (indexPos == 2) { inMenu = 0; displayGPSLoggerMenu(); }
			  if (indexPos == 3) { clear(); write_ascii_string_pgm(5,  5, PSTR("Call stats")); delay_ms(1000); clear(); }
			  if (indexPos == 4) { clear(); displayMenu_Tools(); }
			  if (indexPos == 5) { inMenu = 0; }
			  updateDisplay = 1;
            break;

		}
    wdt_reset();
	}
	clear();
}






void displayRC5Codes()
{
// char tmpStr[6];

    clear();
	write_ascii_string_pgm(2,  3, PSTR("IR REMOTE: PRESS A KEY"));
	write_ascii_string_pgm(2,  5, PSTR("UP:"));
	write_ascii_string_pgm(2,  6, PSTR("MUTE:"));
	write_ascii_string_pgm(2,  7, PSTR("DOWN:"));

	write_ascii_string_pgm(15,  5, PSTR("--"));
	write_ascii_string_pgm(15,  6, PSTR("--"));
	write_ascii_string_pgm(15,  7, PSTR("--"));

while (1) {

  if (rc5.flip >= 0)
  {
 	delay_ms(IR_KEY_DELAY);

	switch (rc5.code) {
		case UP_KEY:
		  write_ascii_string_pgm(15,  5, PSTR("OK"));
		break;

		case ENTER_KEY:
		  write_ascii_string_pgm(15,  6, PSTR("OK"));
		break;

		case DOWN_KEY:
		  write_ascii_string_pgm(15,  7, PSTR("OK"));
		break;


	}

	delay_ms(500);

	write_ascii_string_pgm(15,  5, PSTR("--"));
	write_ascii_string_pgm(15,  6, PSTR("--"));
	write_ascii_string_pgm(15,  7, PSTR("--"));
	rc5.flip = -1;

  }
  wdt_reset();
 }
}








void displayMenu_Tools() {
char inMenu = 1;
char indexPos = 1;
char updateDisplay = 1;


clear();
while (inMenu)
	{

	if (updateDisplay) {
    	write_ascii_string_pgm(startX,  startY-1, (char *)toolsMenu_ptr[0]);
        write_ascii_string_pgm(startX,  startY+1, (char *)toolsMenu_ptr[1]);
        write_ascii_string_pgm(startX,  startY+2, (char *)toolsMenu_ptr[2]);
        write_ascii_string_pgm(startX,  startY+3, (char *)toolsMenu_ptr[3]);
		write_ascii_string_pgm(startX,  indexPos+startY, PSTR(">"));
		updateDisplay = 0;
	}


	     switch (checkInputRequestFromUser() ) {
		 	case DOWN_KEY:
			  if (indexPos < 3) {
			  indexPos++;
			  } else { indexPos = 1; }
			  updateDisplay = 1;
            break;

		 	case UP_KEY:
			  if (indexPos > 1) {
			  indexPos--;
			  } else { indexPos = 3; }
			  updateDisplay = 1;
            break;

		 	case ENTER_KEY:       // EXIT to main menu
			  if (indexPos == 1) { inMenu = 0; displayRC5Codes(); }
			  if (indexPos == 2) { inMenu = 0; resetToDefaultValues();  clear(); write_ascii_string_pgm(4,  5, PSTR("Default values loaded")); delay_ms(2000); }
			  if (indexPos == 3) { inMenu = 0; }
			  updateDisplay = 1;
            break;

		}
    wdt_reset();
	}
	clear();
}




