#include "data_struct.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "gps.h"
#include "main.h"
#include <avr/pgmspace.h>     // Pour stocker des variables dans program en flash (.text) et non .data
#include "utilities.h"

extern volatile GPS_data_str_t GPS_DATA_str;




// Checksum:
// It's the usual method. XOR all the data bytes between the $ and checksum.
// $PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28 <CR><LF>
// Checksum calculator: http://www.hhhh.org/wiml/proj/nmeaxor.html
// Format: "$GPXXX,XXXXXXXXXXXXXXXXXXXXX*AB\r\n"
// AB=CRC, hex format.
//
char checkCRC(char *NMEABuffer) {
	char result = 0;
	unsigned char startChar;
	unsigned char hexCRC[3];

    startChar = 0;
	startChar=0;
	while (NMEABuffer[startChar] != '*')
	{
	startChar++;
	}

	hexCRC[0] = NMEABuffer[startChar+1];
	hexCRC[1] = NMEABuffer[startChar+2];
	hexCRC[2] = 0;

	if (Hex2BinDigit(hexCRC) != genCRC(NMEABuffer)) {
	// Given CRC is false.
	result = 0;
	} else {
	// Given CRC is OK.
	result = 1;
	}

	return result;
}


// GPS Related functions

// Types:
// 0 = GPRMC
// 1 = GPGGA
// 2 = GPGSA

void parseNMEAData(int NMEAType, char *NMEABuffer)
{
  int i,j;
  int x;

//
// GPRMC
//
if (NMEAType == GPRMC_TYPE) {
	x=0;
	while (NMEABuffer[x]!=',')
	{
	x++;
	}


	x++;
	while (NMEABuffer[x]!=',')
	{
	x++;
	}


	// Get the fix validity (A = vAlid, V=inValid).
	x++;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.satFix = NMEABuffer[x];
	x++;
	}


	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strLatitude[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strLatitude[j] = '\0';


	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.latD[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.latD[j] = '\0';


	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strLongitude[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strLongitude[j] = '\0';

	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.lonD[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.lonD[j] = '\0';


	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strSpeedKnots[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strSpeedKnots[j] = '\0';


	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.trackDegrees[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.trackDegrees[j] = '\0';

}


//
// GPGGA
//
if (NMEAType == GPGGA_TYPE) {
	x=0;
	i=0;
	for (i=0; i<6; i++) {
		while (NMEABuffer[x]!=',')
		{
		x++;
		}
		x++;
	}


	j=0;
	while (NMEABuffer[x]!=',')
	{
	// fixQuality[j] = GPGGA[x];
	x++;
	j++;
	}
	// fixQuality[j] = '\0';
    // printf("fix quality=[%s]\n",fixQuality);


    x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strNbSat[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strNbSat[j] = '\0';



	// Recupere HDOP
    x++;
	j=0;
    while (NMEABuffer[x]!=',')
	{
	  GPS_DATA_str.strHDOP[j] = NMEABuffer[x];
	  x++;
	  j++;
	}

	GPS_DATA_str.strHDOP[j] = '\0';

    if (GPS_DATA_str.strHDOP[0] == '\0') {
	strcpy((char *)GPS_DATA_str.strHDOP, "9");
	}

	x++;
	j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strAltitude[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strAltitude[j] = '\0';
}


// GPGSA
if (NMEAType == GPGSA_TYPE) {
	x=0;
	i=0;

	for (i=0; i<2; i++) {
		while (NMEABuffer[x]!=',')
		{
		x++;
		}
		x++;
	}


		while (NMEABuffer[x]!=',')
		{
			GPS_DATA_str.fixMode = NMEABuffer[x];
		x++;
		}
		x++;


	for (i=0; i<12; i++) {
		while (NMEABuffer[x]!=',')
		{
		x++;
		}
		x++;
	}


    j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strPDOP[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strPDOP[j] = '\0';

    if (GPS_DATA_str.strPDOP[0] == '\0') {
	strcpy((char *)GPS_DATA_str.strPDOP, "9");
	}

	x++;

    j=0;
	while (NMEABuffer[x]!=',')
	{
	GPS_DATA_str.strHDOP[j] = NMEABuffer[x];
	x++;
	j++;
	}
	GPS_DATA_str.strHDOP[j] = '\0';

    if (GPS_DATA_str.strHDOP[0] == '\0') {
	strcpy((char *)GPS_DATA_str.strHDOP, "9");
	}
 }

}



// -----------------------------------------
// Permet de convertir le format DDDD.MMMM (degres-minutes) du format NMEA
// vers le format DD.MM.SS (degres-minutes-secondes) attendu par les
// routines de calcul de coordonnées
// -----------------------------------------

double DDDDMMMMtoDDMMSS(char *DDDDMMMM_str) {
double DDMMSS;
double degrees;
double minutes;
  DDMMSS = atof(DDDDMMMM_str);
  minutes = modf(DDMMSS, &degrees);
  minutes = minutes*0.6;
  DDMMSS = (degrees  + minutes);
return DDMMSS;
}



// Convert GPS latitude/longitude to pure degrees [dd.dddd] format.
double sexaToDec(double _sexagesimalCoordinates)
{
double degrees = 0;
double minutesfrac = 0;
double secondsfrac = 0;

 // convert to pure degrees [dd.dddd] format
 minutesfrac = modf(_sexagesimalCoordinates/100, &degrees);
 secondsfrac = modf(minutesfrac*100, &secondsfrac);
 minutesfrac = (minutesfrac - (secondsfrac/100));
 secondsfrac = secondsfrac * 100;
 minutesfrac = minutesfrac * 100;
 _sexagesimalCoordinates = degrees + (minutesfrac/60)  + (secondsfrac/3600);
return _sexagesimalCoordinates;
}



GPS_data_dec_t NMEADataToDec(GPS_data_str_t GPS_data_str)
{
  GPS_data_dec_t GPS_data_dec;

  GPS_data_dec.Latitude = DDDDMMMMtoDDMMSS(GPS_data_str.strLatitude);
  GPS_data_dec.Longitude = DDDDMMMMtoDDMMSS(GPS_data_str.strLongitude);
  GPS_data_dec.nbSat  = atoi((char*)GPS_data_str.strNbSat);

  // if (atoi((char *)GPS_data_str.strAltitude) < VALID_MAX_ALTITUDE) { GPS_data_dec.Altitude = atoi((char *)GPS_data_str.strAltitude); }
  GPS_data_dec.Altitude = atoi((char *)GPS_data_str.strAltitude);


  // GPS_data_dec.Altitude = atoi((char *)GPS_data_str.strAltitude);
  strcpy(GPS_data_dec.lonD,GPS_data_str.lonD);
  strcpy(GPS_data_dec.latD,GPS_data_str.latD);

  // if ((GPS_data_dec.trackDegrees >= 0) && (GPS_data_dec.trackDegrees <= 360)) { }
  GPS_data_dec.trackDegrees= atoi((char *)GPS_data_str.trackDegrees);

  // if ((atof((char *)GPS_data_str.strSpeedKnots) * KNOTS_TO_KPH) < VALID_MAX_SPEED) { GPS_data_dec.speedKph = (atof((char *)GPS_data_str.strSpeedKnots) * KNOTS_TO_KPH); }
  GPS_data_dec.speedKph = (atof((char *)GPS_data_str.strSpeedKnots) * KNOTS_TO_KPH);

  GPS_data_dec.Latitude = sexaToDec(GPS_data_dec.Latitude);
  GPS_data_dec.Longitude = sexaToDec(GPS_data_dec.Longitude);
  GPS_data_dec.HDOP = round(atoi((char *)GPS_data_str.strHDOP));
  GPS_data_dec.PDOP = round(atoi((char *)GPS_data_str.strPDOP));
  return GPS_data_dec;
}


