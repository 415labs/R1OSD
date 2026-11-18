// -----------------------
//
// GPS Related functions
//
// -----------------------

// GPS FIX
#define KNOTS_TO_KPH	1.852			// Conversion factor, from knots to km/h
#define MIN_NBSAT 5						// N bde sat mini visibles pour fix valide
#define MIN_HDOP 4						// Dilution minimale acceptable (horizontale). 1=good 9=very bad.
#define MIN_PDOP 4						// Dilution minimale acceptable (verticale). 1=good 9=very bad.
#define	MAXSPEED_FOR_DISPLAY  3		    // En dessous de cette vitesse, on affiche certaines infos. Au dessus, on affiche plus rien.
#define MIN_DISTANCE_TO_HOME    30		// Used to define the 'flightMode' status.
#define MAX_SPEEDFORFIX		5			// En dessus de cette valeur on ne fixe pas la position.
#define MIN_SUCCESSIVE_FIX	10			// Nombre d'etat FIX successifs necessaire pour passer en etat "ready to fly"
#define FIXDELAY	150					// Delai entre chaques FIX successifs.

#define GPRMC_TYPE	0
#define GPGGA_TYPE	1
#define GPGSA_TYPE 	2

// #define VALID_MAX_ALTITUDE	9999
// #define VALID_MAX_SPEED	999


// Parse raw NMEABuffer, and populates GPS_DATA_str.*
// NMEAType can be: GPRMC_TYPE, GPGGA_TYPE or GPGSA_TYPE
void parseNMEAData(int NMEAType, char *NMEABuffer);


// Convert GPS_data_str values (string) to GPS_data_dec_t (decimal) values.
GPS_data_dec_t NMEADataToDec(GPS_data_str_t GPS_data_str);

// -----------------------------------------
// Permet de convertir le format DDDD.MMMM (degres-minutes) du format NMEA
// vers le format DD.MM.SS (degres-minutes-secondes) attendu par les
// routines de calcul de coordonnées
// -----------------------------------------
double DDDDMMMMtoDDMMSS(char *DDDDMMMM_str);


// Convert GPS latitude/longitude to pure degrees [dd.dddd] format.
double 	sexaToDec(double _sexagesimalCoordinates);

// Get a NMEA string, calculates its CRC, and compares with the given 2-bytes hex CRC at the end of the string (after *).
// If CRC is correct, return 1. If CRC is incorrect, return 0.
char checkCRC(char *NMEABuffer);
