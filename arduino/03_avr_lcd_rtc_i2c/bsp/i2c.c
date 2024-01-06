#include "i2c.h"

#define TIMEOUT_LIMIT 10000

/**
 * @brief Waits for a specific condition on the I2C bus.
 *
 * @param mask The mask to apply to the TWCR register.
 * @param condition The condition to meet for exiting the wait loop.
 * @return uint8_t Status of the wait operation (0 for success, -1 for timeout).
 */
static uint8_t waitForCondition(uint8_t mask, uint8_t condition)
{
	uint16_t timeout = 0;
	while ((TWCR & mask) != condition)
	{
		if (timeout++ > TIMEOUT_LIMIT)
		{
			return -1; // An error is returned when the timeout is exceeded
		}
	}
	return 0; // Success
}

/**
 * @brief Initializes the I2C communication interface.
 */
void i2cInit(void)
{
	TWBR = 0x20; // Set bit rate register value for I2C speed
}

/**
 * @brief Sends the I2C start condition.
 */
void i2cStart(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Send START condition
	// while (!(TWCR & (1 << TWINT)))					  // Wait for START condition to be transmitted
	// 	;
	waitForCondition((1 << TWINT), (1 << TWINT));
}

/**
 * @brief Sends the I2C stop condition.
 */
void i2cStop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Send STOP condition
}

/**
 * @brief Sends a byte of data over I2C.
 *
 * @param byte The byte to be sent.
 */
void i2cSendByte(unsigned char byte)
{
	TWDR = byte;					   // Load byte into Data Register
	TWCR = (1 << TWINT) | (1 << TWEN); // Start transmission of byte
	// while (!(TWCR & (1 << TWINT)))	   // Wait for byte transmission to complete
	// 	;
	waitForCondition((1 << TWINT), (1 << TWINT));
}

/**
 * @brief Sends a byte of data to a specific I2C address.
 *
 * @param data The byte of data to send.
 * @param addr The I2C address to send the data to.
 */
void i2cSendByteByADDR(unsigned char data, unsigned char addr)
{
	i2cStart();		   // Start I2C communication
	i2cSendByte(addr); // Send slave address with write flag
	i2cSendByte(data); // Send data byte
	i2cStop();		   // Stop I2C communication
}

/**
 * @brief Reads a byte of data from I2C.
 *
 * @return unsigned char The received byte.
 */
unsigned char i2cReadByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable ACK for reception, start receiving
	// while (!(TWCR & (1 << TWINT)))					 // Wait for byte reception to complete
	// 	;
	waitForCondition((1 << TWINT), (1 << TWINT));
	return TWDR; // Return received byte
}

/**
 * @brief Reads the last byte of data from I2C.
 *
 * @return unsigned char The last received byte.
 */
unsigned char i2cReadLastByte(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN); // Disable ACK for reception, start receiving last byte
	// while (!(TWCR & (1 << TWINT)))	   // Wait for the last byte reception to complete
	// 	;
	waitForCondition((1 << TWINT), (1 << TWINT));
	return TWDR; // Return the last received byte
}
