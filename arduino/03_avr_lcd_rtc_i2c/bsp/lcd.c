#include "lcd.h"

// Define the LCD control pins
#define LCD_RS_PIN PB0     // Arduino Uno Pin 8
#define LCD_ENABLE_PIN PB1 // Arduino Uno Pin 9

#define LCD_D4_PIN PD4 // Arduino Uno Pin 4
#define LCD_D5_PIN PD5 // Arduino Uno Pin 5
#define LCD_D6_PIN PD6 // Arduino Uno Pin 6
#define LCD_D7_PIN PD7 // Arduino Uno Pin 7

// Define the port
#define LCD_CONTROL_PORT PORTB
#define LCD_CONTROL_DDR DDRB

#define LCD_DATA_PORT PORTD
#define LCD_DATA_DDR DDRD

// Helper macros for controlling the LCD
#define LCD_RS_LOW() (LCD_CONTROL_PORT &= ~(1 << LCD_RS_PIN))
#define LCD_RS_HIGH() (LCD_CONTROL_PORT |= (1 << LCD_RS_PIN))
#define LCD_ENABLE_LOW() (LCD_CONTROL_PORT &= ~(1 << LCD_ENABLE_PIN))
#define LCD_ENABLE_HIGH() (LCD_CONTROL_PORT |= (1 << LCD_ENABLE_PIN))

/**
 * @brief Sends half a byte to the LCD.
 *
 * @param value The half byte value to send.
 */
static void lcdSendHalfByte(unsigned char value)
{
    value <<= 4;
    LCD_ENABLE_HIGH();
    _delay_us(50);
    LCD_DATA_PORT &= 0b00001111;
    LCD_DATA_PORT |= value;
    LCD_ENABLE_LOW();
    _delay_us(50);
}

/**
 * @brief Sends a byte to the LCD in either command or data mode.
 *
 * @param c The byte to send.
 * @param mode The mode for the byte (command=0, data=1).
 */
static void lcdSendByte(unsigned char c, unsigned char mode)
{
    if (mode)
    {
        LCD_RS_HIGH(); // Data mode
    }
    else
    {
        LCD_RS_LOW(); // Command mode
    }

    lcdSendHalfByte(c >> 4); // Send higher nibble
    lcdSendHalfByte(c);      // Send lower nibble
}

/**
 * @brief Initializes the LCD display with predefined settings.
 */
void lcdInit(void)
{
    // Configure LCD pins as output
    LCD_CONTROL_DDR |= (1 << LCD_RS_PIN) | (1 << LCD_ENABLE_PIN);
    LCD_DATA_DDR |= (1 << LCD_D4_PIN) | (1 << LCD_D5_PIN) | (1 << LCD_D6_PIN) | (1 << LCD_D7_PIN);

    // Initialization sequence for the LCD
    _delay_ms(15);
    lcdSendByte(0b00000011, 0);
    _delay_ms(4);
    lcdSendByte(0b00000011, 0);
    _delay_us(100);
    lcdSendByte(0b00000011, 0);
    _delay_ms(1);
    lcdSendByte(0b00000010, 0); // Switch to 4-bit mode
    _delay_ms(1);
    lcdSendByte(0b00101000, 0); // 2 rows, 5x8 point matrix
    _delay_ms(1);
    lcdSendByte(0b00001100, 0); // Display on, cursor off
    _delay_ms(1);
    lcdSendByte(0b00000110, 0); // Cursor increment, no shift
    _delay_ms(1);

    lcdClear();
}

/**
 * @brief Clears the LCD display.
 */
void lcdClear(void)
{
    lcdSendByte(0b00000001, 0);
    _delay_us(1500);
}

/**
 * @brief Sets the cursor to a specified position on the LCD.
 *
 * @param x The x-coordinate (column) for the cursor.
 * @param y The y-coordinate (row) for the cursor.
 */
void lcdSetCursor(uint8_t x, uint8_t y)
{
    char address = (0x40 * y + x) | 0b10000000;
    lcdSendByte(address, 0);
}

/**
 * @brief Displays a string of text on the LCD.
 *
 * @param str Pointer to the string to be displayed.
 */
void lcdPrint(const char *str)
{
    while (*str)
    {
        lcdSendByte(*str++, 1);
    }
}