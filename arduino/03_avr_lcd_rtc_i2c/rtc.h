#ifndef RTC_H
#define RTC_H
#include "main.h"

// I2C addresses for RTC in write and read modes
#define TWI_addrRTC_write 0b11010000
#define TWI_addrRTC_read 0b11010001

// Function declarations for RTC operations
uint8_t RTC_ConvertFromBinDec(uint8_t data);                                                                 // Convert binary-coded decimal to binary
uint8_t RTC_ConvertFromDec(uint8_t data);                                                                    // Convert binary to binary-coded decimal
void setRTC(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint16_t year); // Set RTC with given date and time

// Struct representing the RTC date and time
typedef struct
{
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
} TIME;
TIME RTC; // Global variable for storing RTC data

#endif
