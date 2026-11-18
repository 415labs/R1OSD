/****************************************************************************
 *   Copyright (C) 2009 by Claas Anders "CaScAdE" Rathje                    *
 *   admiralcascade@gmail.com                                               *
 *   Project-URL: http://www.mylifesucks.de/oss/c-osd/                      *
 *                                                                          *
 *   This program is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License.         *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public License      *
 *   along with this program; if not, write to the                          *
 *   Free Software Foundation, Inc.,                                        *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
 ****************************************************************************/

/* ##########################################################################
 * Software SPI to communicate with MAX7456
 * ##########################################################################*/
// For ATMEGA324p
#define MAX_CS_HIGH             PORTB |=  (1 << PB4);
#define MAX_CS_LOW              PORTB &= ~(1 << PB4);
#define MAX_SDIN_HIGH           PORTB |=  (1 << PB5);
#define MAX_SDIN_LOW            PORTB &= ~(1 << PB5);
#define MAX_SCLK_HIGH           PORTB |=  (1 << PB7);
#define MAX_SCLK_LOW            PORTB &= ~(1 << PB7);
//#define MAX_RESET_HIGH          PORTA |=  (1 << PA5);
//#define MAX_RESET_LOW           PORTA &= ~(1 << PA5);


// Character attributes
#define LBC 		0b10000000
#define BLINK 		0b01000000
#define INVERT 		0b00100000
#define STANDARD 	0b00000000


#define PAL			1
#define NTSC		2
#define NO_INPUT	0


void initMax7456();

uint8_t getVideoFormat();


void SPI_MasterInit(void);
char SPI_MasterTransmitReceive(char data);


void fill_char_xy(uint8_t x, uint8_t y, uint8_t length, char character);

/**
 *  write an ascii <string> at <x>/<y> to MAX7456 display memory + ATTRIBUTE (ex blink, invert, etc.)
 */
void write_att_ascii_string(uint8_t x, uint8_t y, char attribute, char *string);


/* ##########################################################################
 * MAX7456 SPI & Display stuff
 * ##########################################################################*/

/**
 * Send a byte through SPI
 */
void spi_send(uint8_t);

/**
 *  Send <byte> to <address> of MAX7456
 */
void spi_send_byte(uint8_t, uint8_t);

/**
 *  write a <character> to <address> of MAX7456 display memory
 */
void write_char(uint16_t, char);

/**
 *  write a character <attribute> to <address> of MAX7456 display memory
 */
void write_char_att(uint16_t, char);
/**
 *  write a <character> at <x>/<y> to MAX7456 display memory
 */
void write_char_xy(uint8_t, uint8_t, char);
/**
 *  write a  character <attribute> at <x>/<y> to MAX7456 display memory
 */
void write_char_att_xy(uint8_t, uint8_t, char attribute);

/**
 *  clear display memory
 *  (also sets 8bit mode)
 */
void clear(void);

/**
 *  write an ascii <character> to <address> of MAX7456 display memory
 */
void write_ascii_char(uint16_t, char);

/**
 *  write an ascii <string> at <x>/<y> to MAX7456 display memory
 */
void write_ascii_string(uint8_t, uint8_t, char*);

/**
 *  write an ascii <string> from progmen at <x>/<y> to MAX7456 display memory
 */
void write_ascii_string_pgm(uint8_t, uint8_t, char*);

/**
 *  write an <string> from progmen at <x>/<y> downwards to MAX7456 display memory
 */
void write_string_pgm_down(uint8_t, uint8_t, char*, uint8_t);


/*
 * for testing write all chars to screen
 */
void write_all_chars();

/**
 * let the MAX7456 learn a new character at <number>
 * with <data>.
 */
void learn_char(uint8_t, unsigned char*);

