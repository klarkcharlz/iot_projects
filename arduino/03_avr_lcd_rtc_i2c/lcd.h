#ifndef LCD_H
#define LCD_H
#include "main.h"

// Function prototypes for interfacing with an LCD

void lcd_init(void); // Initialize the LCD display. This function should be called at the start of the program to prepare the LCD for use.

void lcd_clear(void); // Clear the LCD display. This function will clear any text or characters currently displayed on the LCD.

void lcd_set_cursor(uint8_t x, uint8_t y); // Set the cursor position on the LCD display. The `x` and `y` parameters specify the column and row respectively where the cursor should be placed. This is where the next characters will be displayed.

void lcd_print(const char *str); // Print a string to the LCD. The characters in the string will be displayed on the LCD starting at the current cursor position.

#endif
