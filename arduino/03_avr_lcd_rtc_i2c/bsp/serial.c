#include "serial.h"

// Macro to calculate the baud rate prescale value
#define BAUD_PRESCALE(baudrate) ((F_CPU / (baudrate * 16UL)) - 1)

// Initialize Serial communication
void serialInit(uint32_t baudrate)
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
void serialTransmit(uint8_t data)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    // Put data into buffer, sends the data
    UDR0 = data;
}

// Receive a single byte of data
uint8_t serialReceive(void)
{
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)))
        ;
    // Get and return received data from buffer
    return UDR0;
}

// Print a string via Serial
void serialPrint(const char *str)
{
    // Loop through each character in the string
    while (*str)
    {
        // Transmit the current character
        serialTransmit(*str++);
    }
    serialTransmit('\r'); // Carriage return
    serialTransmit('\n'); // Line translation
}

static void convertNumToStr(int number, char *buffer) {
    int i = 0;
    int isNegative = 0;
    if (number == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }
    if (number < 0) {
        isNegative = 1;
        number = -number;
    }
    while (number != 0) {
        int rem = number % 10;
        buffer[i++] = rem + '0';
        number = number / 10;
    }
    if (isNegative) {
        buffer[i++] = '-';
    }
    buffer[i] = '\0';
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }
}

// Print an integer as a string via Serial
void serialPrintInt(int number)
{
    char buffer[12]; // Buffer to hold the ASCII representation of the number
    // Convert the integer to a string (ASCII)
    convertNumToStr(number, buffer);
    // Print the string using Serial
    serialPrint(buffer);
}
