#ifndef SERIAL_H
#define SERIAL_H
#include "main.h"

// Function declarations for Serial communication
void Serial_Init(uint32_t baudrate); // Initialize Serial communication with given baud rate
void Serial_Transmit(uint8_t data);  // Transmit a single byte of data
uint8_t Serial_Receive(void);        // Receive a single byte of data
void Serial_Print(const char *str);  // Print a string via Serial
void Serial_PrintInt(int number);    // Print an integer as a string via Serial

#endif
