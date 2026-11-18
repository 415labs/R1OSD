#include <math.h>
#include "data_struct.h"

// Use this to detect GPS latitude/longitude errors.
#define VALID_MAX_DISTANCE 100000 		// 100km.


// Maths functions




/*
DOP Value 	Rating 	Description
1 	Ideal 	This is the highest possible confidence level to be used for applications demanding the highest possible precision at all times.
2-3 	Excellent 	At this confidence level, positional measurements are considered accurate enough to meet all but the most sensitive applications.
4-6 	Good 	Represents a level that marks the minimum appropriate for making business decisions. Positional measurements could be used to make reliable in-route navigation suggestions to the user.
7-8 	Moderate 	Positional measurements could be used for calculations, but the fix quality could still be improved. A more open view of the sky is recommended.
9-20 	Fair 	Represents a low confidence level. Positional measurements should be discarded or used only to indicate a very rough estimate of the current location.
21-50 	Poor 	At this level, measurements are inaccurate by as much as 300 metres with a 6 meter accurate device (50 dop * 6 meters) and should be discarded.
*/


//======================
float deg2rad(float deg_angle) {
  return (M_PI/180)*deg_angle;
}


// returns distance in meters between two positions, both specified
// as signed decimal-degrees latitude and longitude. Uses great-circle
// distance computation for hypothised sphere of radius 6372795 meters.
// Because Earth is no exact sphere, rounding errors may be upto 0.5%.

float distance(float lat1, float long1, float lat2, float long2, float units_per_meter) {
  float delta;
  float clat2;
  float sdlong;
  float cdlong;
  float slat1;
  float clat1;
  float slat2;
  float denom;

  delta = deg2rad(long1-long2);
  sdlong  = sin(delta);
  cdlong = cos(delta);
  lat1 = deg2rad(lat1);
  lat2 = deg2rad(lat2);
  slat1 = sin(lat1);
  clat1 = cos(lat1);
  slat2 = sin(lat2);
  clat2 = cos(lat2);
  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = delta*delta;
  delta += ((clat2 * sdlong)*(clat2 * sdlong));
  delta = sqrt(delta);
  denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  return delta * 6372795 * units_per_meter;
}



// returns course in degrees (North=0, West=270) relative to north from
// position 1 to position 2, both specified as signed decimal-degrees
// latitude and longitude.
// Renvoie l'angle Nord-Dest-Source

int course(float _sourceLat, float _sourceLong, float _destLat, float _destLong) {
float dlon;
float a1;
float a2;

  dlon = deg2rad(_destLong-_sourceLong);
  _sourceLat = deg2rad(_sourceLat);
  _destLat = deg2rad(_destLat);
  a1 = sin(dlon) * cos(_destLat);
  a2 = sin(_sourceLat) * cos(_destLat) * cos(dlon);
  a2 = cos(_sourceLat) * sin(_destLat) - a2;
  a2 = atan2(a1, a2);
  if (a2 < 0.0)
  {
  	a2 += (2*M_PI);
  }


  return (int)((a2*180)/M_PI);
}


Position_t updateHemispheres( Position_t _position)
{
	// southern hemisphere is negative-valued (S)
	if (_position.latHemisphere == 1) {
	  _position.latitude = 0.0 - _position.latitude;
	}

	// western hemisphere is negative-valued (W)
	if (_position.lonHemisphere == 1) {
	   _position.longitude = 0.0 -  _position.longitude;
	}
	return _position;
}


// Met à jour les valeur max.
Aircraft_t checkForMaxValues(Aircraft_t _Aircraft) {


  if (_Aircraft.navigation.distanceToHome > _Aircraft.statistics.maxDistanceToHome)
  {
  	_Aircraft.statistics.maxDistanceToHome = _Aircraft.navigation.distanceToHome;
  }

  if ( (_Aircraft.navigation.speed > 0) && (_Aircraft.navigation.speed > _Aircraft.statistics.maxSpeed)  )
  {
  	_Aircraft.statistics.maxSpeed = _Aircraft.navigation.speed;
  }

  if ( (_Aircraft.navigation.currentPosition.altitude > _Aircraft.statistics.maxAltitude) )
  {
  	_Aircraft.statistics.maxAltitude = _Aircraft.navigation.currentPosition.altitude;
  }

  if (_Aircraft.battery.current > _Aircraft.battery.maxCurrent)
  {
  	_Aircraft.battery.maxCurrent = _Aircraft.battery.current;
  }

  if ((_Aircraft.battery.voltage < _Aircraft.battery.lowestVoltage) && (_Aircraft.battery.voltage > 0))
  {
  	_Aircraft.battery.lowestVoltage = _Aircraft.battery.voltage;
  }

  return _Aircraft;
}


// source = origine !
Aircraft_t updateBearingDataToHome(Aircraft_t _Aircraft) {
 int angleToHome;
 int absoluteBearingToHome;
 double tmpDistanceToHome;
 double tmpDiffDistance;

  _Aircraft.navigation.currentPosition = updateHemispheres(_Aircraft.navigation.currentPosition); 	// Update latitude/longitude accordind to the hemispheres (NS EW)

  tmpDistanceToHome = distance(_Aircraft.navigation.homePosition.latitude,_Aircraft.navigation.homePosition.longitude,_Aircraft.navigation.currentPosition.latitude,_Aircraft.navigation.currentPosition.longitude, 1);
  if (tmpDistanceToHome < VALID_MAX_DISTANCE) {
   	_Aircraft.navigation.distanceToHome = tmpDistanceToHome;
  }

  angleToHome = course(_Aircraft.navigation.currentPosition.latitude,_Aircraft.navigation.currentPosition.longitude,  _Aircraft.navigation.homePosition.latitude, _Aircraft.navigation.homePosition.longitude); // Note: on inverse source et dest afin d'obtenir l'angle Nord-Source-Dest et non l'angle Nord-Dest-Source
  _Aircraft.navigation.bearingToHome = 360 - _Aircraft.navigation.heading + angleToHome;

  if (_Aircraft.navigation.bearingToHome > 360) {
    _Aircraft.navigation.bearingToHome = _Aircraft.navigation.bearingToHome - 360;
  }

  if (_Aircraft.navigation.bearingToHome > 180) {
    _Aircraft.navigation.bearingToHome = 360 - _Aircraft.navigation.bearingToHome;
    absoluteBearingToHome = (180 - _Aircraft.navigation.bearingToHome) + 180;
	_Aircraft.navigation.bearingToHome = absoluteBearingToHome;
  }


  if ((_Aircraft.navigation.lastPosition.latitude != 0) && (_Aircraft.navigation.lastPosition.longitude != 0)) {
      tmpDiffDistance = distance(_Aircraft.navigation.lastPosition.latitude,_Aircraft.navigation.lastPosition.longitude,_Aircraft.navigation.currentPosition.latitude,_Aircraft.navigation.currentPosition.longitude, 1);
	  if (tmpDiffDistance < VALID_MAX_DISTANCE) {
	    _Aircraft.statistics.totalDistance = _Aircraft.statistics.totalDistance + tmpDiffDistance;
	    _Aircraft.navigation.lastPosition.latitude = _Aircraft.navigation.currentPosition.latitude;
	    _Aircraft.navigation.lastPosition.longitude = _Aircraft.navigation.currentPosition.longitude;
      }
  }

  return _Aircraft;
}



// Memory functions
/*
void checkMemory() {
char memory[10];
int x = 0;
char *v;
 x=1;
 while ((v=malloc(x)) != NULL)
 {
     free(v);
	 x++;
 }
sprintf((char *)memory,"%6.0d",x);
strcpy(line1, memory);
strcat(line1, " B");
sendLinesToOSD();
delay_ms(1000);
}
*/
