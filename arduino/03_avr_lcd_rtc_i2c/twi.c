#include "twi.h"

void I2C_Init(void)
{
	TWBR = 0x20; // Set bit rate register value for I2C speed
}

void I2C_Start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition
	while (!(TWCR & (1 << TWINT)))					  // Wait for START condition to be transmitted
		;
}

void I2C_Stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send STOP condition
}

void I2C_SendByte(unsigned char byte)
{
	TWDR = byte;					   // Load byte into Data Register
	TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission of byte
	while (!(TWCR & (1 << TWINT)))	   // Wait for byte transmission to complete
		;
}

void I2C_SendByteByADDR(unsigned char data, unsigned char addr)
{
	I2C_Start();		// Start I2C communication
	I2C_SendByte(addr); // Send slave address with write flag
	I2C_SendByte(data); // Send data byte
	I2C_Stop();			// Stop I2C communication
}

unsigned char I2C_ReadByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable ACK for reception, start receiving
	while (!(TWCR & (1 << TWINT)))					 // Wait for byte reception to complete
		;
	return TWDR; // Return received byte
}

unsigned char I2C_ReadLastByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN); // Disable ACK for reception, start receiving last byte
	while (!(TWCR & (1 << TWINT)))	   // Wait for the last byte reception to complete
		;
	return TWDR; // Return the last received byte
}