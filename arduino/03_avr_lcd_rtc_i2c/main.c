#include "main.h"

#define BTN_UP 1
#define BTN_DOWN 2
#define BTN_LEFT 3
#define BTN_RIGHT 4
#define BTN_SELECT 5
#define BTN_NONE 10

#define HOURS_MODE 0
#define MINUTES_MODE 1
#define SECONDS_MODE 2
#define DAYS_MODE 3
#define MONTHS_MODE 4
#define YEARS_MODE 5
#define MODES_COUNT 6

#define NONE_MODE 10

#define UP 0
#define DOWN 1

#define true 1
#define false 0

// variables
char dateStr[11] = {};
char timeStr[9] = {};
uint32_t previousBlinkMillis = 0;
const uint32_t blinkInterval = 350;
uint8_t isBlinkShow = false;
uint32_t lastDisplayUpdate = 0;
const uint32_t displayUpdateInterval = 1000;
uint32_t lastReadButton = 0;
const uint32_t readButtonInterval = 10;
uint32_t currentMillis;
uint8_t lastButtonState = BTN_NONE;
uint8_t currentButtonState;
uint8_t currentSettingsMode = NONE_MODE;

// prototypes
uint8_t detectButton();
void formatNumber(char *buffer, uint8_t number, uint8_t isVisible);
void dateTimeToTimeString(TIME dt, char *buffer);
void dateTimeToDateString(TIME dt, char *buffer);
void printDatetime();

int main(void)
{
  ADC_Init();
  lcd_init();
  I2C_Init();
  Serial_Init(9600);

  lcd_clear();
  setRTC(12, 0, 0, 1, 1, 1, 21);

  lcd_set_cursor(0, 0);
  lcd_print("Hello, World1!");
  lcd_set_cursor(0, 1);

  _delay_ms(3000);

  uint8_t button;
  while (1)
  {
    readRTC();
    lcd_clear();
    printDatetime();
    button = detectButton();
    Serial_Print("Hello, World!\n");
    Serial_PrintInt(button);
    _delay_ms(1000);
  }

  return 0;
}


uint8_t detectButton() {
    uint16_t keyAnalog = ADC_Read(0); // Read from channel PC0 (A0 in Arduino)
    if (keyAnalog < 50) return BTN_RIGHT;
    if (keyAnalog < 195) return BTN_UP;
    if (keyAnalog < 380) return BTN_DOWN;
    if (keyAnalog < 555) return BTN_LEFT;
    if (keyAnalog < 790) return BTN_SELECT;
    return BTN_NONE;
}

void printDatetime()
{
  dateTimeToTimeString(RTC, timeStr);
  dateTimeToDateString(RTC, dateStr);
  lcd_set_cursor(0, 0);
  lcd_print(dateStr);
  lcd_set_cursor(0, 1);
  lcd_print(timeStr);
}

void dateTimeToTimeString(TIME dt, char *buffer)
{
  formatNumber(buffer, dt.hour, currentSettingsMode != HOURS_MODE);
  buffer[2] = ':';
  formatNumber(buffer + 3, dt.min, currentSettingsMode != MINUTES_MODE);
  buffer[5] = ':';
  formatNumber(buffer + 6, dt.sec, currentSettingsMode != SECONDS_MODE);
  buffer[8] = '\0'; // terminating null character
}

void dateTimeToDateString(TIME dt, char *buffer)
{
  formatNumber(buffer, dt.day, currentSettingsMode != DAYS_MODE);
  buffer[2] = '.';
  formatNumber(buffer + 3, dt.month, currentSettingsMode != MONTHS_MODE);
  buffer[5] = '.';
  formatNumber(buffer + 6, dt.year / 100, currentSettingsMode != YEARS_MODE);
  formatNumber(buffer + 8, dt.year % 100, currentSettingsMode != YEARS_MODE);
  buffer[10] = '\0'; // terminating null character
}

void formatNumber(char *buffer, uint8_t number, uint8_t isVisible)
{
  if (isVisible)
  {
    buffer[0] = '0' + number / 10;
    buffer[1] = '0' + number % 10;
  }
  else
  {
    buffer[0] = buffer[1] = ' ';
  }
}
