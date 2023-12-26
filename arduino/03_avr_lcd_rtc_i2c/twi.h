#ifndef TWI_H
#define TWI_H

#include "main.h"

// Function declarations for I2C communication
void I2C_Init(void);                                             // Initialize I2C interface
void I2C_Start(void);                                            // Generate a START condition for I2C communication
void I2C_Stop(void);                                             // Generate a STOP condition to end I2C communication
void I2C_SendByte(unsigned char byte);                           // Send a single byte via I2C
void I2C_SendByteByADDR(unsigned char data, unsigned char addr); // Send a byte to a specific I2C address
unsigned char I2C_ReadByte(void);                                // Read a byte from I2C (not the last byte)
unsigned char I2C_ReadLastByte(void);                            // Read the last byte from I2C (send NACK after reception)

#endif
