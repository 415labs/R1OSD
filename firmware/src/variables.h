#ifndef H_DATA
#define H_DATA
#include <stdint.h>


// Global variables here.
/* variables.h */

#define DEFAULT_GPSBAUDRATE 38400				// 38400 or 57600 bauds
#define DEFAULT_OSDDISPLAYMODE  3 				// OSD Display type: 4=FULL  3=STANDARD  2=MINIMALIST 1=DISABLED
#define DEFAULT_LOGGINGEXPORTFORMAT 1			// 1=KML 2=Raw NMEA
#define DEFAULT_LOGGINGINTERVAL	1				// In seconds
#define DEFAULT_MAXDISTANCE 300					// In meters
#define DEFAULT_MAXALTITUDE 100					// In meters
#define DEFAULT_MAXCAPACITY	1500				// In mA
#define DEFAULT_MINVOLTAGE 9					// In V
#define DEFAULT_MINSATFORFIX 4					// In number of sat 1-10
#define DEFAULT_MINHDOPFORFIX 4					// In HDOP index 1-10 (1 is very good)
#define DEFAULT_MINVDOPFORFIX 4					// In VDOP index 1-10 (1 is very good)
#define DEFAULT_LOGGERMODE 2					// Logger mode: 1 = Always logging 2 = flying sessions only
#define DEFAULT_MAXSPEED	60					// Max speed (kph)
#define DEFAULT_MAXCURRENT	30					// Max current (A)
#define DEFAULT_DEBUGENABLED 0					// No debug
#define DEFAULT_WARNINGSENABLED 0				// Warnings are enabled (displayed) if 1
#define DEFAULT_VIDEOFORMAT	1					// 1=PAL 2=NTSC
#define DEFAULT_MINHDOPFORVALIDDATA	4			// Minimum HDOP value to consider data as valid.

#define CURRENTCHECK_FOR_SENSOR_PRESENCE 15		// If current > 15A right after start, it probably means that the current sensor is not connected.

volatile struct {
	volatile int8_t flip;
	volatile uint8_t key;
} navbutton;



typedef struct {
	uint8_t  loggingExportFormat;	// KML or NMEA
	uint8_t loggingInterval;		// GPS Logging interval, in seconds. 0-255
	uint8_t loggerMode;			// GPS Logging mode: 1=Always on 2=Flying sessions
 	uint8_t debugEnabled;			// Debug enabled 1=debug enabled 0=disabled
	uint8_t OSDDisplayMode; 		// OSD Display type: 1=FULL  2=STANDARD  3=MINIMALIST 4=DISABLED
	uint8_t minSatForFix;			// Minimum number of sat for Fix 0-10
	uint8_t minHDOPForFix; 		// Minimum HDOP for fix 1-9
	uint8_t minVDOPForFix; 		// Minimum VDOP for fix 1-9
	uint8_t minHDOPForValidData; 		// Minimum HDOP for valid data.
	uint8_t warningsEnabled;		// 1 if warnings are displayed, 0 if not.
	uint8_t currentSensorConnected; 	// 1 if current sensor is connected
	uint8_t videoFormat;			// 1=PAL NTSC
    uint16_t gpsBaudrate; 			// GPS Baudrate (38400 or 57600 bauds).
	int16_t warningMaxDistance;		// Max distance before displaying warning
	int16_t warningMaxAltitude;			// Max altitude before displaying warning
	uint16_t warningMaxSpeed; 				// Max speed before displaying warning
	uint16_t warningMaxCurrent; 			// Max current before displaying warning
	uint16_t warningMaxCapacity;			// Max capacity (battery use) before displaying warning

	uint16_t warningMinVoltage;			// Min voltage before displaying warning 0-35
} __attribute__((packed)) OSDParameters_t;

void getAllSettingsFromEEPROM();
void saveAllSettingsToEEPROM();


extern OSDParameters_t OSDParameters;



#endif /* guard */
