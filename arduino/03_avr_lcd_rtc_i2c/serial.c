#include "serial.h"

// Macro to calculate the baud rate prescale value
#define BAUD_PRESCALE(baudrate) ((F_CPU / (baudrate * 16UL)) - 1)

// Initialize Serial communication
void Serial_Init(uint32_t baudrate)
{
    // Set baud rate
    UBRR0H = (uint8_t)(BAUD_PRESCALE(baudrate) >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALE(baudrate));
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmit a single byte of data
void Serial_Transmit(uint8_t data)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Put data into buffer, sends the data
    UDR0 = data;
}

// Receive a single byte of data
uint8_t Serial_Receive(void)
{
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)))
        ;
    // Get and return received data from buffer
    return UDR0;
}

// Print a string via Serial
void Serial_Print(const char *str)
{
    // Loop through each character in the string
    while (*str)
    {
        // Transmit the current character
        Serial_Transmit(*str++);
    }
}

// Print an integer as a string via Serial
void Serial_PrintInt(int number)
{
    char buffer[12]; // Buffer to hold the ASCII representation of the number
    // Convert the integer to a string (ASCII)
    itoa(number, buffer, 10);
    // Print the string using Serial
    Serial_Print(buffer);
}