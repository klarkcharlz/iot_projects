#ifndef LCD_H
#define LCD_H
#include "../app/main.h"

// Function prototypes for interfacing with an LCD

void lcdInit(void); // Initialize the LCD display. This function should be called at the start of the program to prepare the LCD for use.

void lcdClear(void); // Clear the LCD display. This function will clear any text or characters currently displayed on the LCD.

void lcdSetCursor(uint8_t x, uint8_t y); // Set the cursor position on the LCD display. The `x` and `y` parameters specify the column and row respectively where the cursor should be placed. This is where the next characters will be displayed.

void lcdPrint(const char *str); // Print a string to the LCD. The characters in the string will be displayed on the LCD starting at the current cursor position.

#endif
