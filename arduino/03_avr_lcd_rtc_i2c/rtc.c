#include "rtc.h"

// Global variables for RTC time components
unsigned char sec, min, hour, day, date, month, year;
extern TIME; // External declaration of TIME structure

// Convert binary-coded decimal (BCD) to binary
uint8_t RTC_ConvertFromBinDec(uint8_t data)
{
	char BinDec = ((data / 10) << 4) | (data % 10);
	return BinDec;
}

// Convert binary to binary-coded decimal (BCD)
uint8_t RTC_ConvertFromDec(uint8_t data)
{
	uint8_t Dec = ((data >> 4) * 10 + (0b00001111 & data));
	return Dec;
}

// Set the RTC with provided date and time values
void setRTC(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
	I2C_Start();
	I2C_SendByte(TWI_addrRTC_write); // Write to RTC address
	I2C_SendByte(0);				 // Set register pointer to 0
	// Convert and send each time component in BCD format
	I2C_SendByte(RTC_ConvertFromBinDec(sec));
	I2C_SendByte(RTC_ConvertFromBinDec(min));
	I2C_SendByte(RTC_ConvertFromBinDec(hour));
	I2C_SendByte(RTC_ConvertFromBinDec(day));
	I2C_SendByte(RTC_ConvertFromBinDec(date));
	I2C_SendByte(RTC_ConvertFromBinDec(month));
	I2C_SendByte(RTC_ConvertFromBinDec(year));
	I2C_Stop(); // Stop I2C communication
}

// Read current date and time from RTC
void readRTC()
{
	I2C_SendByteByADDR(0, TWI_addrRTC_write); // Prepare RTC for reading
	_delay_ms(100);							  // Wait for RTC to be ready
	I2C_Start();
	I2C_SendByte(TWI_addrRTC_read); // Read from RTC address
	// Read each time component in BCD format
	RTC.sec = I2C_ReadByte();
	RTC.min = I2C_ReadByte();
	RTC.hour = I2C_ReadByte();
	RTC.day = I2C_ReadByte();
	RTC.date = I2C_ReadByte();
	RTC.month = I2C_ReadByte();
	RTC.year = I2C_ReadLastByte();
	I2C_Stop(); // Stop I2C communication

	// Convert each time component from BCD to binary
	RTC.sec = RTC_ConvertFromDec(RTC.sec);
	RTC.min = RTC_ConvertFromDec(RTC.min);
	RTC.hour = RTC_ConvertFromDec(RTC.hour);
	RTC.day = RTC_ConvertFromDec(RTC.day);
	RTC.date = RTC_ConvertFromDec(RTC.date);
	RTC.year = RTC_ConvertFromDec(RTC.year);
	RTC.month = RTC_ConvertFromDec(RTC.month);
}