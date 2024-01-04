#ifndef SERIAL_H
#define SERIAL_H
#include "../app/main.h"

// Function declarations for Serial communication
void serialInit(uint32_t baudrate); // Initialize Serial communication with given baud rate
void serialTransmit(uint8_t data);  // Transmit a single byte of data
uint8_t serialReceive(void);        // Receive a single byte of data
void serialPrint(const char *str);  // Print a string via Serial
void serialPrintInt(int number);    // Print an integer as a string via Serial

#endif
