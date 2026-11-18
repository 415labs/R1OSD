#include <stdint.h>


#define u8 uint8_t
#define s8 int8_t
#define u16 uint16_t
#define s16 int16_t
#define u32 uint32_t
#define s32 int32_t

/*
typedef struct {
    int gpsBaudrate; 			// GPS Baudrate (38400 or 57600 bauds).
	char OSDDisplayMode; 		// OSD Display type: 1=FULL  2=STANDARD  3=MINIMALIST 4=DISABLED
	char loggingExportFormat;	// 1=KML 2=Raw NMEA
	char setLoggingInterval;	// GPS Logging interval, in seconds. 0-255
	int maxDistance;			// Max distance before displaying warning
	int maxAltitude;			// Max altitude before displaying warning
	int maxCapacity;			// Max capacity (battery use) before displaying warning
	double minVoltage;			// Min voltage before displaying warning 0-35
	char minSatForFix;			// Minimum number of sat for Fix 0-10
	char minHDOPForFix; 		// Minimum HDOP for fix 1-9
	char minVDOPForFix; 		// Minimum VDOP for fix 1-9
} __attribute__((packed)) OSDParameters_t;
*/



typedef struct {
    char minutes; 	 	// Minutes
    char seconds;  		// Seconds
	char hour; 			// Hour
} __attribute__((packed)) Time_t;


typedef struct {
    double voltage; 		// Battery Instant voltage in mV
    double current; 		// Battery Instant Current in mA
	u16 capacity;		// Battery capacity, in mA
	double maxCurrent; 		// Max current (A)
	double lowestVoltage; 	// Lowest voltage (V)
} __attribute__((packed)) Battery_t;



typedef struct {
	s16 maxAltitude;		// Max reached altitude (m)
	double maxCurrent; 		// Current peak (mA)
	u16 maxSpeed; 			// Max speed (kph)
	char GPSIsFixed;		// GPS Is fixed ? 1=yes, 0=no
	u32 maxDistanceToHome; 	// Max distance (m)
	u32 totalDistance; 		// Total distance (m)
	u32 HDOPErrors;			// HDOP Errors
	s16	climbRate; 			// Climb rate in m/s
} __attribute__((packed)) Statistics_t;


typedef struct {
    double latitude; 		// Latitude
	u8 latHemisphere; 		// Hemisphere (0=N, 1=S)
    double longitude; 		// Longitude
    u8 lonHemisphere; 		// Hemisphere (0=E, 1=W)
    s16 altitude; 			// Altitude (m)
} __attribute__((packed)) Position_t;


typedef struct {
    u16 speed; 					// Max speed (kph)
    u16 heading; 				// Heading (0 or 360=North, 180=south, in degrees)
	Position_t lastPosition; 	// Last position (for calculating altitude / distance).
	Position_t homePosition; 	// Home
	Position_t currentPosition; // Current aircraft's position
    u32 distanceToHome; 		// Distance to Home (in m)
	s16 bearingToHome; 			// Bearing to home (in degrees). Ex: if +90, then aircraft must turn +90 degrees right to aim the home position.
} __attribute__((packed)) Navigation_t;



typedef struct {
	u8 nbSat;   		// nb of sat
	u8 HDOP;			// HDOP	Horizontal Dilution Of Position: 9=bad, 1=very good
	u8 PDOP;			// PDOP Horizontal Dilution Of Position: 9=bad, 1=very good
	u8 fixMode; 		// 0=no fix 1=2D fix 3=3D fix.
} __attribute__((packed)) satInfo_t;



typedef struct {
	Navigation_t navigation;	// Navigation data
	Battery_t battery;			// Battery data
	Statistics_t statistics;	// Statistics
	satInfo_t satInfo;			// Satellites in view info
	Time_t currentTime; 		// Time
} __attribute__((packed)) Aircraft_t;






typedef struct {
    double Longitude;
    double Latitude;
    u8 nbSat;   		// nb of sat
	u8 HDOP;			// HDOP
	u8 PDOP;			// PDOP
    char latD[2]; 			// N/S
	char lonD[2]; 			// E/W
    u16 Altitude; 		// in m
	u16 trackDegrees; 	// In degrees
	u16 speedKph; 	    // Speed in kph
} __attribute__((packed)) GPS_data_dec_t;




typedef struct {
   char satFix;		// A=Valid V=Invalid
   char fixMode; 	// 0=no fix 1=2D 2=3D
   char strNbSat[3];
   char strSpeedKnots[6];
   char strHDOP[5]; // Dilution of position (horizontale)
   char strPDOP[5]; // Dilution of position (horizontale)
   char strLatitude[10];
   char latD[2];
   char strLongitude[11];
   char lonD[2];
   char strAltitude[8];
   char trackDegrees[6];
} __attribute__((packed)) GPS_data_str_t;


