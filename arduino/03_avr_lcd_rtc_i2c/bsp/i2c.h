#ifndef I2C_H
#define I2C_H

#include "../app/main.h"

// Function declarations for I2C communication
void i2cInit(void);                                             // Initialize I2C interface
void i2cStart(void);                                            // Generate a START condition for I2C communication
void i2cStop(void);                                             // Generate a STOP condition to end I2C communication
void i2cSendByte(unsigned char byte);                           // Send a single byte via I2C
void i2cSendByteByADDR(unsigned char data, unsigned char addr); // Send a byte to a specific I2C address
unsigned char i2cReadByte(void);                                // Read a byte from I2C (not the last byte)
unsigned char i2cReadLastByte(void);                            // Read the last byte from I2C (send NACK after reception)

#endif
