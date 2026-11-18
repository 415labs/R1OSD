#define DIV_FACTOR 		0.1362			// Rapport pont diviseur = 0.1362 (R1=6K34, R2=1K)
#define ADC_RESOLUTION	1024			// Resolution ADC=1024 bits
#define VREF			5.000			// Vref=5.000V (regul)
#define ADC_ITERATIONS  20				// Nombre d'iterations lors de conversion A/D



void adc_init(int input);
int adc_read();
void getBatteryInfo();
