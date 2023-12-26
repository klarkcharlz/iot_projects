#ifndef ADC_H
#define ADC_H
#include "main.h"

// Function prototypes for using the Analog-to-Digital Converter (ADC)

void ADC_Init(); // Initialize the ADC. This function sets up the ADC for use in the program. It configures the ADC with the necessary settings for the conversion process.

uint16_t ADC_Read(uint8_t channel); // Read an analog value from a specified ADC channel. The `channel` parameter specifies which ADC channel to read from. The function returns a 10-bit digital representation (0-1023) of the analog input.

#endif
