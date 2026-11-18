// Bonne source pour l'OSD:
// http://svn.mikrokopter.de/mikrowebsvn/listing.php?repname=Projects&path=%2FC-OSD%2Ftrunk%2F#_C-OSD_trunk_
// http://www.mylifesucks.de/oss/c-osd/
// http://cascade.dyndns.org/~cascade/scripts/max7456/
// http://www.maxim-ic.com/tools/evkit/index.cfm?EVKit=558
// RC5: utiliser ca:
// http://www.gjlay.de/pub/c-code/rc5.html
// Pour la carte SD: http://svn.mikrokopter.de/mikrowebsvn/filedetails.php?repname=Projects&path=%2FFollowMe%2Ftrunk%2Fsdc.c
// mieux: http://instruct1.cit.cornell.edu/courses/ee476/FinalProjects/s2009/jsm66_mpk28/jsm66_mpk28/index.html
// http://www.dharmanitech.com/2009/01/sd-card-interfacing-with-atmega8-fat32.html
// Infos tres interessantes sur la precision GPS.
// http://users.erols.com/dlwilson/gps.htm
// Carte SD: http://www.roland-riegel.de/sd-reader/
// MAV: http://tom.pycke.be/mav/89/micropilot-mp2028-review-part-1



// UART1: GPS
// UART0: PC

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>		//Librairie pour la gestion des interruptions
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>     // Pour stocker des variables dans program en flash (.text) et non .data
#include "max7456_software_spi.h"
#include "adc.h"
#include "bit_utilities.h"
#include "string_utilities.h"
#include "delay_utilities.h"
#include "data_struct.h"
#include "gps.h"
#include "main.h"
#include "osd.h"
#include "gpsMaths.h"
#include <inttypes.h>
#include "menus.h"
#include "rc5.h"
#include "variables.h"
#include <avr/eeprom.h>



// MACROS
#define cbi(x,y) x&= ~(1<<y) //cbi(x,y)
#define sbi(x,y)   x|= (1<<y)//sbi(x,y)

#define UART0_RX_INT_ENABLED	UCSR0B = UCSR0B | (1<<RXCIE0);
#define UART1_RX_INT_ENABLED	UCSR1B = UCSR1B | (1<<RXCIE1);

#define UART0_RX_INT_DISABLED	UCSR0B = UCSR0B | (0<<RXCIE0);
#define UART1_RX_INT_DISABLED	UCSR1B = UCSR1B | (0<<RXCIE1);


// UTILITIES
#define FOSC 20000000
#define BAUD 38400
#define MYUBRR FOSC/16/BAUD-1

#define CS_SDCARD_HIGH             PORTB |=  (1 << PB0);
#define CS_SDCARD_LOW              PORTB &= ~(1 << PB0);


// if DEBUG = 1 then use UART0 (PC) as GPS input source.
// otherwise, GPS on uart1 will be used.
#define DEBUG 0

// if DEBUG_TO_SERIAL =1 then output NMEA sentences to serial port (uart0).
#define DEBUG_TO_SERIAL 0


char requestIRScan = 0;
volatile Aircraft_t Aircraft;
OSDParameters_t OSDParameters;

volatile GPS_data_str_t GPS_DATA_str;
volatile GPS_data_dec_t GPS_DATA_dec;

static volatile char incrementClock = 1;
static volatile char planeLaunched = 0;

char displayMode = 0;
char lastDisplayMode = 0;

// RC5
volatile uint16_t rc5_code = 0;
volatile uint8_t rc5_first_half;




static volatile struct {
    uint8_t *rx_buf;
    uint16_t rx_buf_sz;
    uint16_t head;
	volatile uint8_t rxd_buffer_locked;
} serial;


static volatile uint8_t rxbuf[80];

static volatile s16 lastAltitude;


// Timer setup
void    setupTimer1();
void flipLed(int iterations, int delay_miliseconds);
static FILE mystdout = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);


/* ##########################################################################
 * Interrupt handler
 * ##########################################################################*/
/**
 * handler for undefined Interrupts
 * if not defined AVR will reset in case any unhandled interrupts occur
 */
ISR(__vector_default) {
        asm("nop");
}


ISR(TIMER1_COMPA_vect)
{
   if (incrementClock) Aircraft.currentTime.seconds++;
   if (Aircraft.currentTime.seconds == 60) {
     Aircraft.currentTime.minutes ++;
     Aircraft.currentTime.seconds = 0;
   }
   if (Aircraft.currentTime.minutes == 60) {
     Aircraft.currentTime.minutes = 0;
   }

  // Update battery capacity every 1s
  Aircraft.battery.capacity=Aircraft.battery.capacity+(Aircraft.battery.current/3.6);
  Aircraft.statistics.climbRate =  Aircraft.navigation.currentPosition.altitude - lastAltitude;
  lastAltitude = Aircraft.navigation.currentPosition.altitude;
}


#if DEBUG == 1

// Input data on UART0 (for GPS Simulator use).
ISR(USART0_RX_vect)
{
	uint8_t byte;

	//  Get the character from the UART data register
	byte = UDR0;

	if (serial.rxd_buffer_locked == 1) return;

	// Skip any first char that isn't a dollar sign
	if ((serial.head == 0) && (byte != '$')) return;

	// Store the char in the buffer
	serial.rx_buf[serial.head++] = byte;

	// If end-of-sentence, lock the buffer.
	if (byte == '\r')
	{
		serial.rx_buf[serial.head-1] = 0;
		serial.head = 0;
		serial.rxd_buffer_locked = 1;
	}

	// Prevent buffer overrun
	else if (serial.head == (serial.rx_buf_sz))
	{
	  	serial.head = 0;
	    serial.rxd_buffer_locked = 0;
	}
}
#endif




#if DEBUG == 0
// Input data on UART1 (for GPS use).
ISR(USART1_RX_vect)
{

	uint8_t byte;

	//  Get the character from the UART data register
	byte = UDR1;

	if (serial.rxd_buffer_locked == 1) return;

	// Skip any first char that isn't a dollar sign
	if ((serial.head == 0) && (byte != '$')) return;

	// Store the char in the buffer
	serial.rx_buf[serial.head++] = byte;

	// If end-of-sentence, lock the buffer.
	if (byte == '\r')
	{
		serial.rx_buf[serial.head-1] = 0;
		serial.head = 0;
		serial.rxd_buffer_locked = 1;
	}

	// Prevent buffer overrun
	else if (serial.head == (serial.rx_buf_sz))
	{
	  	serial.head = 0;
	    serial.rxd_buffer_locked = 0;
	}
}
#endif


// Initialize the serial rx buffer.
void serial_rx_buffer(uint8_t data[], uint16_t sz )
{
    serial.rx_buf = data;
    serial.rx_buf_sz = sz;
	serial.rxd_buffer_locked = 0; // unlock the buffer.
	serial.head = 0;
}


// Setup the timers.
void setupTimer1() {
// Bonne info: timer calculator:
// http://www.et06.dk/atmega_timers/
 //----------------------------------------------------
 // Set up timer 1 to generate an interrupt every 1 s
 //----------------------------------------------------
 TCCR1A = 0x00;
 TCCR1B = (_BV(WGM12) | _BV(CS10) | _BV(CS12)); // @ 20MHz et 1s, prescaler de 1024 (cf datasheet atmega p.132).
 TCCR1C = 0; // NEW
 OCR1A = 0x4C4B; // @ 20MHz et 1s
 TIMSK1 = _BV(OCIE1A);
}


// Put a char on uart0
void uart0_putchar(unsigned char c)
{
  loop_until_bit_is_set(UCSR0A,UDRE0);
  UDR0 = c;
  if(c == '\n') uart0_putchar('\r'); // newline -> CRLF
}


// Put a char on uart1
void uart1_putchar(unsigned char c)
{
  loop_until_bit_is_set(UCSR1A,UDRE1);
  UDR1 = c;
  if(c == '\n') uart1_putchar('\r'); // newline -> CRLF
}

/*
// Get a char on uart0
uint8_t uart0_getchar(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
    return(UDR0);
}

// Get a char on uart1
uint8_t uart1_getchar(void)
{
    while( !(UCSR1A & (1<<RXC1)) );
    return(UDR1);
}
*/


/**
 * send a <string> throught usart1
 */
void usart1_puts(char *s) {
    while (*s) {
        uart1_putchar(*s);
        s++;
    }
}



// Send commands to the Locosys LS200033 GPS.
void setupGPS() {
// Set update frequency to 5Hz
usart1_puts("$PMTK300,200,0,0,0,0*2F\r\n");

// Active fields: $GPRMC, $GPGGA and $GPGSA
usart1_puts("$PMTK314,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n");
}




void ioinit (void)
{
  // Set portD for input
  // Except TXPC and TXGPS as outputs.
  DDRD =  0b00001010;
  // Active les pullup du portd
  PORTD = 0xFF;

  // Set portC for input, except PC2 as output (led)
  DDRC = 0b00000100;
  // Disable pullups
  PORTC = 0x00;

  /* set PORTB directions */
  DDRB = 0b10111011;


  /* set PORTA for input except PA5 for output (reset du max)*/
  DDRA = 0b00100000;
  PORTA = 0;


  //USART0 Baud rate
  UBRR0H = ((MYUBRR)>>8);
  UBRR0L = MYUBRR;

  // Note: l'int sur RX est desactivee pour le moment.
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);


  //USART1 Baud rate
  UBRR1H = ((MYUBRR)>>8);
  UBRR1L = MYUBRR;

  // Note: l'int sur RX est desactivee pour le moment.
  UCSR1B = (1<<RXEN1)|(1<<TXEN1);


  // Masque des interruptions: active int2 // Capteur IR
  EIMSK |= _BV(INT2);		// Met le bit int0 à l'etat haut

  // Interruption sur front  descendant donc dans le registre MCUCR ISC01 = 1 ISC00 =0
  // Vu que MCUCR à comme valeur par defaut 0 on a juste à mettre ISC01 à 1 Cf : datasheet
  // voir ici: http://wiki.jelectronique.com/avr
  EICRA |= _BV(ISC21);


   // Definition des interruptions externes
   PCMSK3 |= (1<<PCINT29); //  tell pin change mask to listen to PD5
   PCMSK3 |= (1<<PCINT30); //  tell pin change mask to listen to PD6
   PCMSK3 |= (1<<PCINT31); //  tell pin change mask to listen to PD7
   PCICR |= (1<<PCIE3); // enable PCINT interrupt in the  interrupt mask



 // set up Atmega Ports
    DDRB |= (1 << PB4); // PB4 output (/CS)
    MAX_CS_HIGH
    DDRB |= (1 << PB5); // PB5 output (SDIN)
    MAX_SDIN_LOW
    DDRB |= (1 << PB7); // PB7 output (SCLK)
    MAX_SCLK_LOW

    DDRA &=~_BV(0);  			// Input on PA0 (HiZ) // mesure de courant sur sonde courant->tension
    DDRA &=~_BV(1);  			// Input on PA1 (HiZ) // mesure de tension sur pont diviseur
	stdout = &mystdout; //Required for printf init
}






// Get battery data
void getBatteryInfo()
{
 int i;
 adc_init(1);      // AD on PA1
 Aircraft.battery.voltage = (((double)adc_read() * ((1/DIV_FACTOR)  * VREF)) / ADC_RESOLUTION);
 adc_init(0);		// AD on PC1
 Aircraft.battery.current = 0;

 for (i=0; i<ADC_ITERATIONS; i++) {
   Aircraft.battery.current = Aircraft.battery.current + ((double)adc_read()  * VREF) / ADC_RESOLUTION;
 }
 Aircraft.battery.current = Aircraft.battery.current  / ADC_ITERATIONS;

 Aircraft.battery.current = 50+((Aircraft.battery.current - 0.5)/-0.04); // cf courbe de conversion tension courant du capteur a effet hall allegro 50A

 // In order to avoid displaying negative values when measuring near 0V/0A.
 if (Aircraft.battery.current < 0) { Aircraft.battery.current = 0; }
 if (Aircraft.battery.capacity < 0) { Aircraft.battery.capacity = 0; }
}



// Gestion du navbutton.
ISR(PCINT3_vect)
{

	// One of pins PD5, PD6 or PD7 just changed states (but we don't know which one).
	//
	if (bit_is_clear(PIND, 5) ) { // poll the pin
	  navbutton.flip = 1;
	  navbutton.key = DOWN_KEY;
	 }

	if (bit_is_clear(PIND, 6) ) { // poll the pin
	  navbutton.flip = 1;
	  navbutton.key = ENTER_KEY;
	 }

	if (bit_is_clear(PIND, 7) ) { // poll the pin
	  navbutton.flip = 1;
	  navbutton.key = UP_KEY;
	 }
}


// Flip the  led
void flipLed(int iterations, int delay_miliseconds) {
int i;
for (i=0; i<iterations; i++) {
   bit_flip(PORTC,BIT(2));	// Flippe la  led
   delay_ms(delay_miliseconds);
   }

}









void getDataFromGPS() {

char GPRMC_OK = 0;
char GPGGA_OK = 0;
char GPGSA_OK = 0;

		while (GPRMC_OK == 0) {
 	    while (serial.rxd_buffer_locked == 0)  {;;}
		if ((strncmp((char *)rxbuf,"$GPRMC",6) == 0) && (checkCRC((char *)rxbuf) == 1) ) {GPRMC_OK = 1;} else { serial.head = 0; serial.rxd_buffer_locked = 0; }
		}
		GPRMC_OK = 0;


 		parseNMEAData(GPRMC_TYPE, (char *)rxbuf);

		#if DEBUG_TO_SERIAL == 1
		  printf("GPRMC=[%s]\n",rxbuf);
		#endif


		while (GPGGA_OK == 0) {
 	    while (serial.rxd_buffer_locked == 0)  {;;}
		if ((strncmp((char *)rxbuf,"$GPGGA",6) == 0) && (checkCRC((char *)rxbuf) == 1)) {GPGGA_OK = 1;} else { serial.head = 0; serial.rxd_buffer_locked = 0; }
		}
		GPGGA_OK = 0;

 		parseNMEAData(GPGGA_TYPE, (char *)rxbuf);

		#if DEBUG_TO_SERIAL == 1
		  printf("GPGGA=[%s]\n",rxbuf);
		#endif

		while (GPGSA_OK == 0) {
 	    while (serial.rxd_buffer_locked == 0)  {;;}
		if ((strncmp((char *)rxbuf,"$GPGSA",6) == 0) && (checkCRC((char *)rxbuf) == 1)) {GPGSA_OK = 1;} else { serial.head = 0; serial.rxd_buffer_locked = 0; }
		}
		GPGSA_OK = 0;

 		parseNMEAData(GPGSA_TYPE, (char *)rxbuf);

		#if DEBUG_TO_SERIAL == 1
		  printf("GPGSA=[%s]\n",rxbuf);
		#endif

		serial.rxd_buffer_locked = 0;



	// If data is valid, then process.
	if (GPS_DATA_str.satFix == 'A') {
		GPS_DATA_dec  = NMEADataToDec(GPS_DATA_str);


		// Ici on teste si la dilution horizontale actuelle est inferieure à un certain seuil.
		// Si c'est le cas, on considere la position horizontale comme correcte.
		/*
		DOP Value 	Rating 	Description
		1 	Ideal 	This is the highest possible confidence level to be used for applications demanding the highest possible precision at all times.
		2-3 	Excellent 	At this confidence level, positional measurements are considered accurate enough to meet all but the most sensitive applications.
		4-6 	Good 	Represents a level that marks the minimum appropriate for making business decisions. Positional measurements could be used to make reliable in-route navigation suggestions to the user.
		7-8 	Moderate 	Positional measurements could be used for calculations, but the fix quality could still be improved. A more open view of the sky is recommended.
		9-20 	Fair 	Represents a low confidence level. Positional measurements should be discarded or used only to indicate a very rough estimate of the current location.
		21-50 	Poor 	At this level, measurements are inaccurate by as much as 300 metres with a 6 meter accurate device (50 dop * 6 meters) and should be discarded.
		*/

		if (GPS_DATA_dec.HDOP <= OSDParameters.minHDOPForValidData) {
		    Aircraft.navigation.currentPosition.latitude = GPS_DATA_dec.Latitude;
			if (GPS_DATA_dec.latD[0] == 'N') { Aircraft.navigation.currentPosition.latHemisphere = 0; }
			if (GPS_DATA_dec.latD[0] == 'S') { Aircraft.navigation.currentPosition.latHemisphere = 1; }

		    Aircraft.navigation.currentPosition.longitude = GPS_DATA_dec.Longitude;
			if (GPS_DATA_dec.lonD[0] == 'E') { Aircraft.navigation.currentPosition.lonHemisphere = 0; }
			if (GPS_DATA_dec.lonD[0] == 'W') { Aircraft.navigation.currentPosition.lonHemisphere = 1; }

		    Aircraft.navigation.currentPosition.altitude = GPS_DATA_dec.Altitude;
		    Aircraft.satInfo.nbSat = GPS_DATA_dec.nbSat;
		    Aircraft.satInfo.HDOP = GPS_DATA_dec.HDOP;
		    Aircraft.satInfo.PDOP = GPS_DATA_dec.PDOP;
		    Aircraft.navigation.heading = GPS_DATA_dec.trackDegrees;
			Aircraft.navigation.speed = GPS_DATA_dec.speedKph;

			Aircraft.satInfo.fixMode = GPS_DATA_str.fixMode-49;  // NO, 2D or 3D.
		}  else { // Ici on peut eventuellement compter les erreurs sur le HDOP.
			Aircraft.statistics.HDOPErrors++;
			}
   }
}





void resetAircraftTimer() {
	Aircraft.currentTime.seconds = 0;
	Aircraft.currentTime.minutes = 0;
}


void updateDisplay() {
  // We select the display mode here. Depending on flight conditions.

  displayMode = 99;

	// waiting for fix screen
	if (Aircraft.statistics.GPSIsFixed == 0) {
		displayMode = 1;
		incrementClock = 1;
		if (lastDisplayMode != displayMode) clear();
		lastDisplayMode = displayMode;
   		OSDWaitingForFix(Aircraft);
	}



	// inflight screen
	if ( (Aircraft.statistics.GPSIsFixed  == 1) && (Aircraft.navigation.speed >= MAXSPEED_FOR_DISPLAY) ) {
  	    if ((planeLaunched == 0) && (Aircraft.navigation.distanceToHome > MIN_DISTANCE_TO_HOME))
		{
		  Aircraft.currentTime.seconds = 0;
		  Aircraft.currentTime.minutes = 0;
		  planeLaunched = 1;
		}
		displayMode = 2;
		incrementClock = 1;
		if (lastDisplayMode != displayMode) clear();
		lastDisplayMode = displayMode;
 	  	sendToOSDFlightData(Aircraft);
	}


	// postflight (stats) screen after a flight
	if ( (Aircraft.statistics.GPSIsFixed == 1) && (Aircraft.navigation.speed < MAXSPEED_FOR_DISPLAY) && (planeLaunched == 1) ) {
		displayMode = 3;
		incrementClock = 0;
		if (lastDisplayMode != displayMode) clear();
		lastDisplayMode = displayMode;
		sendToOSDPostFlightData(Aircraft);
	}

/*
	// ready for flight screen
	if (  (planeLaunched == 0) && (Aircraft.statistics.GPSIsFixed == 1) ) {
		write_ascii_string(12,  8, "READY");
	} else {
		write_ascii_string(12,  8, "-----");
	}
*/
	// just in case...
    if (displayMode == 99) {
	   incrementClock = 1;
	   sendToOSDFlightData(Aircraft);
	}
}



void setHomePosition() {
    Aircraft.navigation.homePosition.latitude =  Aircraft.navigation.currentPosition.latitude;
    Aircraft.navigation.homePosition.latHemisphere = Aircraft.navigation.currentPosition.latHemisphere;
    Aircraft.navigation.homePosition.longitude = Aircraft.navigation.currentPosition.longitude;
    Aircraft.navigation.homePosition.lonHemisphere = Aircraft.navigation.currentPosition.lonHemisphere;
	Aircraft.navigation.homePosition.altitude = Aircraft.navigation.currentPosition.altitude;
    Aircraft.navigation.homePosition = updateHemispheres(Aircraft.navigation.homePosition); 		// Update latitude/longitude accordind to the hemispheres (NS EW)
    Aircraft.navigation.lastPosition.latitude = Aircraft.navigation.currentPosition.latitude;
    Aircraft.navigation.lastPosition.longitude = Aircraft.navigation.currentPosition.longitude;
}







int main (void) {

uint8_t fixCounter;




// Load R1OSD default system parameters
// Will load from EEPROM when this function will be ready !
// resetToDefaultValues();

  setupTimer1();
  ioinit(); 			// Setup IO pins and defaults
  // DEBUG
  wdt_enable(WDTO_4S);

  printf("R1OSD Startup\n");
  // Set max 7456 reset line low (max is off)
  bit_clear(PORTA, BIT(5));
  delay_ms(500);
  // Set max 7456 reset line high (max is on)
  bit_set(PORTA, BIT(5));
  delay_ms(500);
  // Initialize Hardware
  // Allow any RC5 device address
  rc5_init (RC5_ALL);
  sei();


  // Get video input format
  OSDParameters.videoFormat = getVideoFormat();		// 1=video in pal, 2=video in NTSC, 0=no input


   //OSDParameters.videoFormat = PAL; // Force PAL
   //OSDParameters.videoFormat = NTSC; // Force NTSC

  initMax7456();
  delay_ms(100);
  OSDIntro(2000);
  setupGPS();
  delay_ms(100);

  // Reset timer.
  resetAircraftTimer();


  getAllSettingsFromEEPROM();



  getBatteryInfo();
  if (Aircraft.battery.current > CURRENTCHECK_FOR_SENSOR_PRESENCE ) { OSDParameters.currentSensorConnected=0; } else  { OSDParameters.currentSensorConnected=1; }


  fixCounter = 0;


  Aircraft.statistics.maxDistanceToHome = 0;
  Aircraft.statistics.maxSpeed = 0;
  Aircraft.statistics.maxAltitude = 0;
  Aircraft.battery.maxCurrent = 0;
  Aircraft.battery.lowestVoltage = 999;
  Aircraft.statistics.maxDistanceToHome = 0;
  Aircraft.statistics.totalDistance = 0;






// init rx buffer
serial_rx_buffer((uint8_t*)rxbuf, sizeof(rxbuf)/sizeof(uint8_t));
serial.head = 0;
serial.rxd_buffer_locked = 0;

#if DEBUG == 1
	// On debugge depuis le PC, donc on active l'int de RX sur uart0.
	UART0_RX_INT_ENABLED
#else
	// On recupere les donnes depuis le GPS (UART1), donc on active l'int de RX sur uart1.
	UART1_RX_INT_ENABLED
#endif


wdt_reset();


// Il faut aller chercher cette valeur en eeprom et dans les menus, et non pas en statique...
OSDParameters.minHDOPForValidData = 4;



navbutton.flip = 0;

while(1) {

  if (checkInputRequestFromUser() == ENTER_KEY) {
    clear();
	displayMenu_Main(Aircraft);
  }

  getDataFromGPS();		// Get all the data from GPS, do NMEA parsing and check values.
  getBatteryInfo();

  if (Aircraft.statistics.GPSIsFixed == 1) {
   Aircraft = updateBearingDataToHome(Aircraft);
   Aircraft = checkForMaxValues(Aircraft);
  }

  if ( (Aircraft.satInfo.fixMode == 2)  && (Aircraft.satInfo.PDOP <= MIN_PDOP) && (Aircraft.satInfo.HDOP <= MIN_HDOP)  && (Aircraft.statistics.GPSIsFixed == 0 ) &&  (Aircraft.satInfo.nbSat >= MIN_NBSAT) && (Aircraft.navigation.speed < MAX_SPEEDFORFIX))
  {
  	fixCounter++;
	delay_ms(FIXDELAY);
  }

  if ( (fixCounter > MIN_SUCCESSIVE_FIX) && (Aircraft.satInfo.PDOP <= MIN_PDOP) && (Aircraft.satInfo.HDOP <= MIN_HDOP)  && (Aircraft.statistics.GPSIsFixed == 0 ) &&  (Aircraft.satInfo.nbSat >= MIN_NBSAT) && (Aircraft.navigation.speed < MAX_SPEEDFORFIX)) {
    // Now the GPS position is considered as fixed.
	Aircraft.statistics.GPSIsFixed = 1;
	// Reset timer.
	resetAircraftTimer();
    setHomePosition();
 	Aircraft =  updateBearingDataToHome(Aircraft);
	OSDPositionIsSet(2000);
  }


   updateDisplay();

  flipLed(2, 10);
  wdt_reset();
}



}
