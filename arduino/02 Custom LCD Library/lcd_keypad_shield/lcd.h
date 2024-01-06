#pragma once  // Prevents the same header file from being included repeatedly

#include <Arduino.h>  // Connecting the main Arduino file to work with Arduino functions and data types

class CustomLCD {
private:
  uint8_t rs, enable, d4, d5, d6, d7;

  void sendchar(unsigned char c);                      // Method to send character to LCD
  void sendbyte(unsigned char c, unsigned char mode);  // Method to send a byte of data or command
  void sendhalfbyte(unsigned char c);                  // Method to send half a byte (since 4-bit mode is used)

public:
  CustomLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);  // Constructor
  void init(void);                                                                        // Method to initialize the display
  void clear(void);                                                                       // Method to clear the screen
  void setCursor(uint8_t x, uint8_t y);                                                   // Method for setting cursor position
  void print(char str[]);                                                                 // Method for printing a string to the screen
};
