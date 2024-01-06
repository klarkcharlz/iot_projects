#include "rtc.h"

/**
 * @brief Convert binary-coded decimal (BCD) to binary.
 *
 * @param data The BCD value to convert.
 * @return uint8_t The converted binary value.
 */
uint8_t rtcConvertFromBinDec(uint8_t data)
{
	char BinDec = ((data / 10) << 4) | (data % 10);
	return BinDec;
}

/**
 * @brief Convert binary to binary-coded decimal (BCD).
 *
 * @param data The binary value to convert.
 * @return uint8_t The converted BCD value.
 */
uint8_t rtcConvertFromDec(uint8_t data)
{
	uint8_t Dec = ((data >> 4) * 10 + (0b00001111 & data));
	return Dec;
}

/**
 * @brief Set the RTC with the provided date and time values.
 *
 * @param sec Seconds value to set.
 * @param min Minutes value to set.
 * @param hour Hours value to set.
 * @param day Day of the week to set.
 * @param date Date value to set.
 * @param month Month value to set.
 * @param year Year value to set.
 */
void setRtc(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
	i2cStart();
	i2cSendByte(TWI_addrRTC_write); // Write to RTC address
	i2cSendByte(0);					// Set register pointer to 0
	// Convert and send each time component in BCD format
	i2cSendByte(rtcConvertFromBinDec(sec));
	i2cSendByte(rtcConvertFromBinDec(min));
	i2cSendByte(rtcConvertFromBinDec(hour));
	i2cSendByte(rtcConvertFromBinDec(day));
	i2cSendByte(rtcConvertFromBinDec(date));
	i2cSendByte(rtcConvertFromBinDec(month));
	i2cSendByte(rtcConvertFromBinDec(year));
	i2cStop(); // Stop I2C communication
}

/**
 * @brief Read the current date and time from the RTC.
 */
void readRtc()
{
	i2cSendByteByADDR(0, TWI_addrRTC_write); // Prepare RTC for reading
	_delay_ms(100);							 // Wait for RTC to be ready
	i2cStart();
	i2cSendByte(TWI_addrRTC_read); // Read from RTC address
	// Read each time component in BCD format
	RTC.sec = i2cReadByte();
	RTC.min = i2cReadByte();
	RTC.hour = i2cReadByte();
	RTC.day = i2cReadByte();
	RTC.date = i2cReadByte();
	RTC.month = i2cReadByte();
	RTC.year = i2cReadLastByte();
	i2cStop(); // Stop I2C communication

	// Convert each time component from BCD to binary
	RTC.sec = rtcConvertFromDec(RTC.sec);
	RTC.min = rtcConvertFromDec(RTC.min);
	RTC.hour = rtcConvertFromDec(RTC.hour);
	RTC.day = rtcConvertFromDec(RTC.day);
	RTC.date = rtcConvertFromDec(RTC.date);
	RTC.year = rtcConvertFromDec(RTC.year) + 2000;
	RTC.month = rtcConvertFromDec(RTC.month);
}