#include "lcd.h"

// Class constructor
CustomLCD::CustomLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  : rs(rs), enable(enable), d4(d4), d5(d5), d6(d6), d7(d7) {}

// Initialize LCD
void CustomLCD::init() {
  // Set the output mode for pins
  pinMode(rs, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);

  // Initialize the command display and delays from the datasheet
  // 0b00000011 This is the command to set the display to 8-bit mode.
  // It is sent three times in a row with different delays between sends,
  // which is standard initialization procedure for most LCD displays
  delay(15);
  sendbyte(0b00000011, LOW);
  delay(4);
  sendbyte(0b00000011, LOW);
  delayMicroseconds(100);
  sendbyte(0b00000011, LOW);
  delay(1);
  sendbyte(0b00000010, LOW);  // Switch to 4-bit mode
  delay(1);

  // Конфигурация LCD
  sendbyte(0b00101000, LOW);  // 2 rows (N = 1) and a 5x8 point matrix (F = 0)
  delay(1);
  sendbyte(0b00001100, LOW);  // display is on (D = 1), cursor is off (C = 0), and flashing cursor is also off (B = 0)
  delay(1);
  sendbyte(0b00000110, LOW);  // the cursor will move to the right (I/D = 1), and will not shift the display contents (S = 0) after each character is written
  delay(1);
  clear();
}

// Clear display
void CustomLCD::clear() {
  sendbyte(0b00000001, LOW);  // Command to clear the display
  delayMicroseconds(1500);    // Wait for the command to complete
}

// Set the cursor position
void CustomLCD::setCursor(uint8_t x, uint8_t y) {
  char adress;
  adress = (0x40 * y + x) | 0b10000000;  // Calculate address in LCD memory
  sendbyte(adress, LOW);                 // Command to set the cursor position
}

// Display text
void CustomLCD::print(char str[]) {
  wchar_t n;
  for (n = 0; str[n] != '\0'; n++)  // Loop through all characters in a string
    sendchar(str[n]);               // Send every character
}

// Send a symbol
void CustomLCD::sendchar(unsigned char c) {
  sendbyte(c, 1);  // Send a character with the RS (data) bit set
}

// Send a byte of data or command
void CustomLCD::sendbyte(unsigned char c, unsigned char mode) {
  digitalWrite(rs, mode);  // Set RS mode (HIGH for data, LOW for commands)
  unsigned char hc = 0;
  hc = c >> 4;       // Get the most significant tetrad of the byte
  sendhalfbyte(hc);  // Send the highest tetrad
  sendhalfbyte(c);   // Send the low tetrad
}

void CustomLCD::sendhalfbyte(unsigned char value) {
  value <<= 4;  // Shift values to high order to match pins D4-D7
  digitalWrite(d4, value & 0x10);
  digitalWrite(d5, value & 0x20);
  digitalWrite(d6, value & 0x40);
  digitalWrite(d7, value & 0x80);

  digitalWrite(enable, HIGH);  // turn on line E
  delayMicroseconds(50);
  digitalWrite(enable, LOW);  // turn off line E
  delayMicroseconds(50);
}
