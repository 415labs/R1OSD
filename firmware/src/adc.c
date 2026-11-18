#include "adc.h"
#include <avr/io.h>





void adc_init(int input) {
  // set analog to digital converter
  // for external reference (5v), single ended input ADC0
  ADMUX = input; // On choisit l'entree souhaitee (ex: 0 pour pc0);
  // set analog to digital converter
  // to be enabled, with a clock prescale of 1/128
  // so that the ADC clock runs at 115.2kHz.
  ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

int adc_read() {
  int adc_tmp;
  // set ADSC bit to get the *next* conversion started
  ADCSRA |= (1<<ADSC);

  // read from ADC, waiting for conversion to finish
  // wait for it to be cleared
  while(ADCSRA & (1<<ADSC)) {
    // do nothing... just hold your breath.
  }
  // bit is cleared, so we have a result.

  // read from the ADCL/ADCH registers, and combine the result
  // Note: ADCL must be read first (datasheet pp. 259)
  adc_tmp = ADCL;
  adc_tmp += (ADCH <<8);
  return adc_tmp;
}



