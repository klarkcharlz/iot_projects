#include "adc.h"

void ADC_Init() {
    // Select the reference voltage (for example, AVcc with an external capacitor on the AREF pin)
    ADMUX |= (1 << REFS0);
    // Channel selection (PC0, which corresponds to A0 in Arduino)
    ADMUX &= ~(1 << MUX0);
    // Set the frequency prescaler for the ADC (for example, 128 for 16 MHz)
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Enable ADC
    ADCSRA |= (1 << ADEN);
}

uint16_t ADC_Read(uint8_t channel) {
    // Select ADC channel while saving reference voltage settings
    ADMUX = (ADMUX & 0xF8) | channel;
    // Start single conversion
    ADCSRA |= (1 << ADSC);
    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));
    // Return result
    return ADC;
}
