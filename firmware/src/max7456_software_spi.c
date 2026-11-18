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

#include <avr/io.h>
#include <util/delay.h>
#include "delay_utilities.h"
#include <avr/pgmspace.h>
#include "max7456_software_spi.h"

/* Newer avr-libc versions name the ATmega324P SPI registers with a "0"
 * suffix (SPCR0, SPDR0, ...). Map the classic names so the code builds
 * with both old (2009 WinAVR) and current toolchains. */
#if !defined(SPCR) && defined(SPCR0)
#define SPCR SPCR0
#define SPDR SPDR0
#define SPSR SPSR0
#define SPE  SPE0
#define MSTR MSTR0
#define SPR0 SPR00
#define SPIF SPIF0
#endif
#include "variables.h"



#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))
#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


void initMax7456() {
    //Pushing NEW chars to the MAX7456
    spi_send_byte(0x00, 0b00000000);

	if (OSDParameters.videoFormat == PAL) {
	    // PAL + enable display immediately (VM0)
    	spi_send_byte(0x00, 0b01001000);
	}

	if (OSDParameters.videoFormat == NTSC) {
	    // NTSC + enable display immediately (VM0)
    	spi_send_byte(0x00, 0b00001000);
	}


	if (OSDParameters.videoFormat == NO_INPUT) {
	    // Default is PAL + enable display immediately (VM0)
    	spi_send_byte(0x00, 0b01001000);
	}


    // clear all display-mem (DMM)
    spi_send_byte(0x04, 0b00000100);

    // clearing takes 12uS according to maxim so lets wait longer
    _delay_us(120);

    // 8bit mode
    spi_send_byte(0x04, 0b01000000);

    // clear display memory and set to 8bit mode
    clear();
    delay_ms(20);
}




uint8_t getVideoFormat() {
uint8_t videoMode = 0;
uint8_t data = 0;

  SPI_MasterInit();
  MAX_CS_LOW;
  SPI_MasterTransmitReceive(0xA0);
  delay_ms(10);
  data = SPI_MasterTransmitReceive(0);
  MAX_CS_HIGH
  SPCR = 0;

  // Input source is PAL Format
  if (bit_get(data,BIT(0))) { videoMode = PAL; }

  // Input source is NTSC Format
  if (bit_get(data,BIT(1))) { videoMode = NTSC; }

  // No input source connected
  if ( (bit_get(data,BIT(0)) == 0) && (bit_get(data,BIT(1)) == NO_INPUT) )
   { videoMode = 0; }

  return videoMode;
}



void SPI_MasterInit(void)
{
    // set up Atmega Ports
    DDRB |= (1 << PB4); // PB4 output (/CS)
    MAX_CS_HIGH
    DDRB |= (1 << PB5); // PB5 output (MOSI)
    MAX_SDIN_LOW
    DDRB |= (1 << PB7); // PB7 output (SCLK)
    MAX_SCLK_LOW

/* Enable SPI, Master, set clock rate fck/16 */
   SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
   delay_ms(10);
}

char SPI_MasterTransmitReceive(char data)
{
/* Start transmission */
SPDR = data; //put data in SPDR
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)));
return SPDR;
}


/* ##########################################################################
 * MAX7456 SPI & Display stuff
 * ##########################################################################*/

/**
 * Send a byte through SPI
 */
void spi_send(uint8_t byte) {
    for (int8_t i = 7; i >= 0; i--) {
        if (((byte >> i) & 1)) {
            MAX_SDIN_HIGH
        } else {
            MAX_SDIN_LOW
        }
        MAX_SCLK_HIGH
        MAX_SCLK_LOW
    }
}

/**
 *  Send <byte> to <address> of MAX7456
 */
void spi_send_byte(uint8_t address, uint8_t byte) {
    // start sending
    MAX_CS_LOW

    spi_send(address);
    spi_send(byte);

    // end sending
    MAX_CS_HIGH
}

/**
 *  write a <character> to <address> of MAX7456 display memory
 */
void write_char(uint16_t address, char character) {
    spi_send_byte(0x05, (address & 0xFF00) >> 8); // DMAH
    spi_send_byte(0x06, (address & 0x00FF)); // DMAL
    spi_send_byte(0x07, character); // DMDI
}

/**
 *  write a character <attribute> to <address> of MAX7456 display memory
 */
void write_char_att(uint16_t address, char attribute) {
    // the only important part is that the DMAH[1] is set
    // so we add 2 which binary is the 2nd lowest byte
    spi_send_byte(0x05, ((address & 0xFF00) >> 8) | 2); // DMAH
    spi_send_byte(0x06, (address & 0x00FF)); // DMAL
    spi_send_byte(0x07, attribute); // DMDI
}
/**
 *  write an ascii <string> at <x>/<y> to MAX7456 display memory + attributes
 */
void write_att_ascii_string(uint8_t x, uint8_t y, char attribute, char *string) {
    while (*string) {
		write_char_att_xy(x, y, attribute);
        write_ascii_char(((x++)+(y * 30)), *string);
        string++;
    }
}




void fill_char_xy(uint8_t x, uint8_t y, uint8_t length, char character) {
uint8_t i;
	for (i=0; i<length; i++) {
	 write_char_xy(x+i, y, character);
	}
}


/**
 *  write a <character> at <x>/<y> to MAX7456 display memory
 */
void write_char_xy(uint8_t x, uint8_t y, char character) {
    uint16_t address = y * 30 + x;
    write_char(address, character);
}

/**
 *  write a  character <attribute> at <x>/<y> to MAX7456 display memory
 */
void write_char_att_xy(uint8_t x, uint8_t y, char attribute) {
    uint16_t address = y * 30 + x;
    write_char_att(address, attribute);
}

/**
 *  clear display memory
 *  (also sets 8bit mode)
 */
void clear(void) {
    /*
	uint16_t memory_address = 0;
    for (unsigned int a = 0; a < 480; a++) {
        write_char(memory_address++, 0);
    }
    */

	    // clear all display-mem (DMM)
    spi_send_byte(0x04, 0b01000100);

    // clearing takes 12uS according to maxim so lets wait longer
    _delay_us(20);
}

/**
 *  write an ascii <character> to <address> of MAX7456 display memory
 */
void write_ascii_char(uint16_t address, char c) {
    if (c == 32) c = 0; // remap space
    else if (c > 48 && c <= 57) c -= 48; // remap numbers
    else if (c == '0') c = 10; // remap zero
    else if (c >= 65 && c <= 90) c -= 54; // remap big letters
    else if (c >= 97 && c <= 122) c -= 60; // remap small letters
    else if (c == '(') c = 63; // remap
    else if (c == ')') c = 64; // remap
    else if (c == '.') c = 65; // remap
    else if (c == '?') c = 66; // remap
    else if (c == ';') c = 67; // remap
    else if (c == ':') c = 68; // remap
    else if (c == ',') c = 69; // remap
    else if (c == '\'') c = 70; // remap
    else if (c == '/') c = 71; // remap
    else if (c == '"') c = 72; // remap
    else if (c == '-') c = 73; // remap minus
    else if (c == '<') c = 74; // remap
    else if (c == '>') c = 75; // remap
    else if (c == '@') c = 76; // remap
    write_char(address, c);
}



/**
 *  write an ascii <string> at <x>/<y> to MAX7456 display memory
 */
void write_ascii_string(uint8_t x, uint8_t y, char *string) {
    while (*string) {
        write_ascii_char(((x++)+(y * 30)), *string);
        string++;
    }
}

/**
 *  write an ascii <string> from progmen at <x>/<y> to MAX7456 display memory
 */
void write_ascii_string_pgm(uint8_t x, uint8_t y, char *string) {
        while (pgm_read_byte(string) != 0x00)
                write_ascii_char(((x++)+(y * 30)), pgm_read_byte(string++));
}

/**
 *  write an <string> from progmen at <x>/<y> downwards to MAX7456 display memory
 */
void write_string_pgm_down(uint8_t x, uint8_t y, char *string, uint8_t length) {
        while (length--)
                write_char((x+(y++ * 30)), pgm_read_byte(string++));
}



/**
 * for testing write all chars to screen
 */
void write_all_chars() {
    uint16_t x = 3, y = 2, t = 0;
    while (t < 256) {
        write_char_xy(x++, y, t++);
        if (x > 25) {
            y++;
            x = 3;
        }
    }
}

/**
 * let the MAX7456 learn a new character at <number>
 * with <data>.
 */
void learn_char(uint8_t number, unsigned char* data) {
    // select character to write (CMAH)
    spi_send_byte(0x09, number);

    for (uint8_t i = 0; i < 54; i++) {
        // select 4pixel byte of char (CMAL)
        spi_send_byte(0x0A, i);

        // write 4pixel byte of char (CMDI)
        spi_send_byte(0x0B, data[i]);
    }

    // write to the NVM array from the shadow RAM (CMM)
    spi_send_byte(0x08, 0b10100000);

    // according to maxim writing to nvram takes about 12ms, lets wait longer
    _delay_ms(120);
}


