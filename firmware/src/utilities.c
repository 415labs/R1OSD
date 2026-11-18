// utilities.c
#include "utilities.h"
#include <string.h>


unsigned char Hex2BinDigit(unsigned char* hex)
{
unsigned char bin;

bin = (hex[0] <= '9' ? hex[0] - '0' : hex[0] - '0' - 7) * 0x10 ;
bin += hex[1] <= '9' ? hex[1] - '0' : hex[1] - '0' - 7 ;

return bin;
}




// Calcs the CRC of any NMEA sentence.
// Format: $GPGGA,165724.000,0543.1333,N,04509.2333,E,1,7,0.00,800.0,M,,M,,*47
// Returns the CRC as a decimal value, whereas NMEA CRC value is hex (47 is hex).
char genCRC(char *data) {
 int i;
 int CRC;
 char curChar;

 i = 0;
 CRC = 0;
 while ((data[i] != '*') && (i<=(strlen(data))) ) {
     if (data[i] == '$') {
	 i++;
	 CRC = 0;
	 }
     curChar =  data[i];
	 CRC = curChar ^ CRC;
     i++;
   }

  return CRC;
}


char isEven(int number)
{
   return ((number-1)%2);
}
