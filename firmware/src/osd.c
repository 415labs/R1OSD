// Bonne source pour l'OSD:
// http://svn.mikrokopter.de/mikrowebsvn/listing.php?repname=Projects&path=%2FC-OSD%2Ftrunk%2F#_C-OSD_trunk_
// http://www.mylifesucks.de/oss/c-osd/
// http://cascade.dyndns.org/~cascade/scripts/max7456/
// http://www.maxim-ic.com/tools/evkit/index.cfm?EVKit=558

#include "adc.h"
#include "data_struct.h"
#include "string_utilities.h"
#include <util/delay.h>
#include "delay_utilities.h"
#include "max7456_software_spi.h"
#include "osd_helpers.h"
#include "variables.h"
#include "utilities.h"
#include "osd.h"
#include <string.h>
#include <avr/pgmspace.h>




void clearWarnings(char posX,char posY, char length) {
  fill_char_xy(posX, posY, length, 0);
  fill_char_xy(posX, posY+1, length, 0);
  fill_char_xy(posX, posY+2, length, 0);
  fill_char_xy(posX, posY+3, length, 0);
  fill_char_xy(posX, posY+4, length, 0);
  fill_char_xy(posX, posY+5, length, 0);
}







void OSDIntro(int delay_millisec) {
  clear();
  delay_ms(100);
  write_ascii_string_pgm(5,  7, PSTR("R1OSD"));
  write_ascii_string_pgm(5,  8, PSTR("FW 3.2"));


  write_ascii_string_pgm(5,  10, PSTR("INPUT: "));

	switch (OSDParameters.videoFormat) {

	case PAL :
		write_ascii_string_pgm(16,  10, PSTR("PAL"));
		break;

	case NTSC :
		write_ascii_string_pgm(16,  10, PSTR("NTSC"));
		break;

	case NO_INPUT :
		write_ascii_string_pgm(16,  10, PSTR("--"));
		break;
	}
  delay_ms(delay_millisec);
  clear();
}



void OSDWaitingForFix(Aircraft_t Aircraft) {
int8_t deltaY;

/*
char strFixMode[3];
if (Aircraft.satInfo.fixMode == 0) { strcpy(strFixMode,"--"); }
if (Aircraft.satInfo.fixMode == 1) { strcpy(strFixMode,"2D"); }
if (Aircraft.satInfo.fixMode == 2) { strcpy(strFixMode,"3D"); }
*/

  if (OSDParameters.videoFormat == PAL) { deltaY = 0; }
  if (OSDParameters.videoFormat == NTSC) { deltaY = 2; }


  write_ascii_string_pgm(4,  7-deltaY, PSTR("WAITING FOR SATELLITES"));


  write_ascii_string(12,  14-deltaY, (char *)itoaPadding(Aircraft.currentTime.minutes, 2, '0'));
  write_ascii_string_pgm(14,  14-deltaY, PSTR(":"));
  write_ascii_string(15,  14-deltaY, (char *)itoaPadding(Aircraft.currentTime.seconds, 2, '0'));

  write_ascii_string_pgm(19,  12-deltaY, PSTR("S"));
  write_ascii_string(20,  12-deltaY, (char *)itoaPadding(Aircraft.satInfo.nbSat, 2, '0'));
  write_ascii_string_pgm(22,  12-deltaY, PSTR("/"));
  write_ascii_string(23,  12-deltaY, (char *)itoaPadding(Aircraft.satInfo.HDOP, 2, '0'));
  write_ascii_string_pgm(25,  12-deltaY, PSTR("/"));
  write_ascii_string(26,  12-deltaY, (char *)itoaPadding(Aircraft.satInfo.PDOP, 2, '0'));

  write_ascii_string(3,  12-deltaY, (char *)itoaPadding(Aircraft.navigation.speed, 3, ' '));
  write_ascii_string_pgm(6,  12-deltaY, PSTR(" kph"));


}

void OSDPositionIsSet(int delay_millisec) {
  clear();
  delay_ms(100);
  write_ascii_string_pgm(3,  7, PSTR("POSITION IS SET - READY"));
  delay_ms(delay_millisec);
  clear();
}



void sendToOSDPostFlightData(Aircraft_t Aircraft) {
  char tmpStr[7];
  int8_t deltaY;
  double tmpDistanceToHome;
  double tmpTotalDistance;


  if (OSDParameters.videoFormat == PAL) { deltaY = 0; }
  if (OSDParameters.videoFormat == NTSC) { deltaY = 2; }

  write_ascii_string_pgm(2,  2, PSTR("Flight review"));
  write_ascii_string_pgm(2,  4, PSTR("Max. speed:"));
  write_ascii_string_pgm(2,  5, PSTR("Max. altitude:"));
  write_ascii_string_pgm(2,  6, PSTR("Max. distance:"));
  write_ascii_string_pgm(2,  7, PSTR("Total distance:"));
  write_ascii_string_pgm(2,  8, PSTR("Flight time:"));

  write_ascii_string(17,  4, (char *)itoaPadding(Aircraft.statistics.maxSpeed, 5, ' '));
  write_ascii_string(17,  5, (char *)itoaPadding(Aircraft.statistics.maxAltitude-Aircraft.navigation.homePosition.altitude, 5, ' '));

  if (Aircraft.statistics.maxDistanceToHome >= 1000) {
    tmpDistanceToHome = (double)Aircraft.statistics.maxDistanceToHome/1000;
	sprintf(tmpStr,"%5.1f",tmpDistanceToHome);
    write_ascii_string(17,  6, (char *)tmpStr);
	write_ascii_string_pgm(23,  6, PSTR("km"));
  } else {
	sprintf(tmpStr,"%5.0f",(double)Aircraft.statistics.maxDistanceToHome);
    write_ascii_string(17,  6,  tmpStr);
	write_ascii_string_pgm(23,  6,  PSTR("m "));
  }

  if (Aircraft.statistics.totalDistance >= 1000) {
    tmpTotalDistance = (double)Aircraft.statistics.totalDistance/1000;
	sprintf(tmpStr,"%5.1f",tmpTotalDistance);
    write_ascii_string(17,  7, (char *)tmpStr);
	write_ascii_string_pgm(23,  7, PSTR("km"));
  } else {
	sprintf(tmpStr,"%5.0f",(double)Aircraft.statistics.totalDistance);
    write_ascii_string(17,  7,  tmpStr);
	write_ascii_string_pgm(23,  7,  PSTR("m "));
  }


  write_ascii_string(19,  8, (char *)itoaPadding(Aircraft.currentTime.minutes, 2, '0'));
  write_ascii_string_pgm(21,  8, PSTR(":"));
  write_ascii_string(22,  8, (char *)itoaPadding(Aircraft.currentTime.seconds, 2, '0'));

  write_ascii_string_pgm(23,  4, PSTR("kph"));
  write_ascii_string_pgm(23,  5, PSTR("m"));


	  if (OSDParameters.currentSensorConnected) {
	  write_ascii_string_pgm(2,  9, PSTR("Max. current:"));
	  write_ascii_string_pgm(2,  10, PSTR("Lowest voltage:"));
	  write_ascii_string_pgm(2,  11, PSTR("Battery use:"));

	  sprintf((char *)tmpStr,"%5.1f",Aircraft.battery.maxCurrent);
	  write_ascii_string(17,  9, (char *)tmpStr);

	  sprintf((char *)tmpStr,"%5.1f",Aircraft.battery.lowestVoltage);
	  write_ascii_string(17,  10, (char *)tmpStr);

	  write_ascii_string(17,  11, (char *)itoaPadding(Aircraft.battery.capacity, 5, ' '));

	  write_ascii_string_pgm(23,  9, PSTR("A"));
	  write_ascii_string_pgm(23,  10, PSTR("V"));
	  write_ascii_string_pgm(23,  11, PSTR("mA"));
	  }

  }


void sendToOSDFlightData(Aircraft_t Aircraft) {
  char tmpStr[6];
  char textAttribute;
  char warnings = 0;

  int8_t deltaY;
  int8_t deltaY2;


  if (OSDParameters.videoFormat == PAL) { deltaY = 0; deltaY2=1;}
  if (OSDParameters.videoFormat == NTSC) { deltaY = 2; deltaY2=0; }



  int diffAltitude;
  textAttribute = STANDARD;
  char strDistToHome[7];


  double tmpDistanceToHome;
  char warningType;

// WarningType:
// 1: altitude: max altitude is reached
// 2: speed: max speed is reached
// 3: distance: max distance is reached
// 4: tension: max voltage is reached
// 5: courant: max current is reached
// 6: capacity: max capacity is reached




  warningType = 0;

  diffAltitude = Aircraft.navigation.currentPosition.altitude - Aircraft.navigation.homePosition.altitude;
  if ( (diffAltitude > OSDParameters.warningMaxAltitude) && OSDParameters.warningsEnabled) { warnings = 1; warningType=1; textAttribute = BLINK; } else {textAttribute = STANDARD; }



  write_att_ascii_string(2,  0+deltaY2, textAttribute, (char *)itoaPadding(diffAltitude, 4, ' '));
  write_att_ascii_string(7,  0+deltaY2, textAttribute, "m");


  if ((Aircraft.navigation.speed > OSDParameters.warningMaxSpeed) && OSDParameters.warningsEnabled) { warnings = 1; warningType=2; textAttribute = BLINK; } else {textAttribute = STANDARD; }
  write_att_ascii_string(3,  1+deltaY2, textAttribute, (char *)itoaPadding(Aircraft.navigation.speed, 3, ' '));
  write_ascii_string_pgm(7,  1+deltaY2, PSTR("kph"));


  if ((Aircraft.navigation.distanceToHome > OSDParameters.warningMaxDistance) && OSDParameters.warningsEnabled) {warnings = 1; warningType=3; textAttribute = BLINK; } else {textAttribute = STANDARD; }
  write_ascii_string_pgm(15,  0+deltaY2, PSTR("Home"));

  // If > 1000 then we use km insted of m for display.
  if (Aircraft.navigation.distanceToHome >= 1000) {
    tmpDistanceToHome = (double)Aircraft.navigation.distanceToHome/1000;
	sprintf(strDistToHome,"%5.1f",tmpDistanceToHome);
    write_att_ascii_string(20,  0+deltaY2, textAttribute, strDistToHome);
	write_att_ascii_string(25,  0+deltaY2, textAttribute, "km");
  } else {
	sprintf(strDistToHome,"%5.0f",(double)Aircraft.navigation.distanceToHome);
    write_att_ascii_string(20,  0+deltaY2, textAttribute, strDistToHome);
	write_att_ascii_string(25,  0+deltaY2, textAttribute, "m ");
  }


	// Variometre
	write_ascii_string(3,  2+deltaY2, (char *)itoaPadding(Aircraft.statistics.climbRate, 3, ' '));
	write_ascii_string_pgm(7,  2+deltaY2, PSTR("m/s"));



  if (OSDParameters.currentSensorConnected) {
	  if ((Aircraft.battery.voltage < OSDParameters.warningMinVoltage) && OSDParameters.warningsEnabled) {warnings = 1; warningType=4; textAttribute = BLINK; } else {textAttribute = STANDARD; }
	  sprintf((char *)tmpStr,"%4.1f",Aircraft.battery.voltage);
	  write_att_ascii_string(2,  13-deltaY, textAttribute, (char *)tmpStr);
	  write_att_ascii_string(6,  13-deltaY, textAttribute, " V");

	  if ((Aircraft.battery.current > OSDParameters.warningMaxCurrent) && OSDParameters.warningsEnabled) { warnings = 1; warningType=5; textAttribute = BLINK; } else {textAttribute = STANDARD; }
	  sprintf((char *)tmpStr,"%4.1f",Aircraft.battery.current);
	  write_att_ascii_string(2,  14-deltaY, textAttribute, (char *)tmpStr);
	  write_att_ascii_string(6,  14-deltaY, textAttribute, " A");


	  write_ascii_string(20,  13-deltaY, (char *)itoaPadding(Aircraft.battery.current * Aircraft.battery.voltage, 5, ' '));
	  write_ascii_string_pgm(25,  13-deltaY, PSTR(" W"));

	  if ((Aircraft.battery.capacity > OSDParameters.warningMaxCapacity) && OSDParameters.warningsEnabled) { warnings = 1; warningType=6; textAttribute = BLINK; } else {textAttribute = STANDARD; }
	  write_att_ascii_string(20,  14-deltaY, textAttribute, (char *)itoaPadding(Aircraft.battery.capacity, 5, ' '));
	  write_att_ascii_string(25,  14-deltaY, textAttribute, " mA");
  }


  write_ascii_string(12,  14-deltaY, (char *)itoaPadding(Aircraft.currentTime.minutes, 2, '0'));
  write_ascii_string_pgm(14,  14-deltaY, PSTR(":"));
  write_ascii_string(15,  14-deltaY, (char *)itoaPadding(Aircraft.currentTime.seconds, 2, '0'));


  if (Aircraft.statistics.GPSIsFixed == 1) { write_char_xy(18,  1+deltaY2,   getCompassArrow(Aircraft.navigation.bearingToHome , 0xa0)); }



// WarningType:
// 1: altitude: max altitude is reached
// 2: speed: max speed is reached
// 3: distance: max distance is reached
// 4: tension: max voltage is reached
// 5: courant: max current is reached
// 6: capacity: max capacity is reached




  if (warnings) {
	write_att_ascii_string(11, 5, BLINK, "WARNING");
	switch(warningType) {
		case 1:
			write_ascii_string_pgm(11, 6, PSTR("ALTITUDE   "));
		break;

		case 2:
			write_ascii_string_pgm(11, 6, PSTR("HI SPEED   "));
		break;

		case 3:
			write_ascii_string_pgm(11, 6, PSTR("TOO FAR    "));
		break;

		case 4:
			write_ascii_string_pgm(11, 6, PSTR("LOW VOLTAGE"));
		break;

		case 5:
			write_ascii_string_pgm(11, 6, PSTR("HI CURRENT "));
		break;

		case 6:
			write_ascii_string_pgm(11, 6, PSTR("LOW BATTERY"));
		break;

	}


  } else {
	write_ascii_string_pgm(11, 5, PSTR("       "));
	write_ascii_string_pgm(11, 6, PSTR("           "));
  }



  }


