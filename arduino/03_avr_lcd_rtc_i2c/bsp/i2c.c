#include "i2c.h"

void i2cInit(void)
{
	TWBR = 0x20; // Set bit rate register value for I2C speed
}

void i2cStart(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition
	while (!(TWCR & (1 << TWINT)))					  // Wait for START condition to be transmitted
		;
}

void i2cStop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send STOP condition
}

void i2cSendByte(unsigned char byte)
{
	TWDR = byte;					   // Load byte into Data Register
	TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission of byte
	while (!(TWCR & (1 << TWINT)))	   // Wait for byte transmission to complete
		;
}

void i2cSendByteByADDR(unsigned char data, unsigned char addr)
{
	i2cStart();		   // Start I2C communication
	i2cSendByte(addr); // Send slave address with write flag
	i2cSendByte(data); // Send data byte
	i2cStop();		   // Stop I2C communication
}

unsigned char i2cReadByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable ACK for reception, start receiving
	while (!(TWCR & (1 << TWINT)))					 // Wait for byte reception to complete
		;
	return TWDR; // Return received byte
}

unsigned char i2cReadLastByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN); // Disable ACK for reception, start receiving last byte
	while (!(TWCR & (1 << TWINT)))	   // Wait for the last byte reception to complete
		;
	return TWDR; // Return the last received byte
}
