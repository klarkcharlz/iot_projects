#include "lcd.h"
#include <microDS3231.h>

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

// classes
class RTCDateTime {
public:
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;

  void update(uint16_t y, uint8_t mo, uint8_t d, uint8_t h, uint8_t mi, uint8_t s) {
    year = y;
    month = mo;
    day = d;
    hour = h;
    minute = mi;
    second = s;
  }

  void increment(uint8_t mode) {
    switch (mode) {
      case HOURS_MODE: hour = (hour + 1) % 24; break;
      case MINUTES_MODE: minute = (minute + 1) % 60; break;
      case SECONDS_MODE: second = (second + 1) % 60; break;
      case DAYS_MODE: day = day % 31 + 1; break;
      case MONTHS_MODE: month = month % 12 + 1; break;
      case YEARS_MODE: year++; break;
    }
  }

  void decrement(uint8_t mode) {
    switch (mode) {
      case HOURS_MODE: hour = (hour + 23) % 24; break;
      case MINUTES_MODE: minute = (minute + 59) % 60; break;
      case SECONDS_MODE: second = (second + 59) % 60; break;
      case DAYS_MODE: day = (day == 1) ? 31 : day - 1; break;
      case MONTHS_MODE: month = (month == 1) ? 12 : month - 1; break;
      case YEARS_MODE: year--; break;
    }
  }
};

// variables
uint32_t previousBlinkMillis = 0;
const uint32_t blinkInterval = 350;
bool isBlinkShow = false;

char dateStr[11] = {};
char timeStr[9] = {};

uint32_t lastDisplayUpdate = 0;
const uint32_t displayUpdateInterval = 1000;
uint32_t lastReadButton = 0;
const uint32_t readButtonInterval = 10;
uint32_t currentMillis;

uint8_t lastButtonState = BTN_NONE;
uint8_t currentButtonState;

uint8_t currentSettingsMode = NONE_MODE;

// prototypes
void dateTimeToTimeString(DateTime dt, char* buffer);
void dateTimeToDateString(DateTime dt, char* buffer);
uint8_t detectButton();
void printDatetime();
void changeDatetime(uint8_t mode, uint8_t operation);
void settingsMode();
void readRTC();
void formatNumber(char* buffer, uint8_t number, bool isVisible);
void handleButtonPress(uint8_t button, uint8_t& mode);

// objects
CustomLCD lcd(8, 9, 4, 5, 6, 7);
MicroDS3231 rtc;
RTCDateTime myDateTime;

void setup() {
  // rtc.setTime(COMPILE_TIME);  // use for the first firmware to set the current time
  Serial.begin(9600);
  lcd.init();
  readRTC();
  printDatetime();
}

void loop() {
  currentMillis = millis();

  if (currentMillis - lastReadButton >= readButtonInterval) {
    lastReadButton = currentMillis;
    currentButtonState = detectButton();
    if (currentButtonState == BTN_SELECT && currentButtonState != lastButtonState) {
      lastButtonState = currentButtonState;
      settingsMode();
    } else {
      lastButtonState = currentButtonState;
    }
  }

  if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
    lastDisplayUpdate = currentMillis;
    readRTC();
    printDatetime();
  }
}

void readRTC() {
  myDateTime.update(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
}

void formatNumber(char* buffer, uint8_t number, bool isVisible) {
  if (isVisible) {
    buffer[0] = '0' + number / 10;
    buffer[1] = '0' + number % 10;
  } else {
    buffer[0] = buffer[1] = ' ';
  }
}

void dateTimeToTimeString(RTCDateTime dt, char* buffer) {
  formatNumber(buffer, dt.hour, currentSettingsMode != HOURS_MODE);
  buffer[2] = ':';
  formatNumber(buffer + 3, dt.minute, currentSettingsMode != MINUTES_MODE);
  buffer[5] = ':';
  formatNumber(buffer + 6, dt.second, currentSettingsMode != SECONDS_MODE);
  buffer[8] = '\0';  // terminating null character
}

void dateTimeToDateString(RTCDateTime dt, char* buffer) {
  formatNumber(buffer, dt.day, currentSettingsMode != DAYS_MODE);
  buffer[2] = '.';
  formatNumber(buffer + 3, dt.month, currentSettingsMode != MONTHS_MODE);
  buffer[5] = '.';
  formatNumber(buffer + 6, dt.year / 100, currentSettingsMode != YEARS_MODE);
  formatNumber(buffer + 8, dt.year % 100, currentSettingsMode != YEARS_MODE);
  buffer[10] = '\0';  // terminating null character
}

uint8_t detectButton() {
  uint16_t keyAnalog = analogRead(A0);
  if (keyAnalog < 50) return BTN_RIGHT;
  if (keyAnalog < 195) return BTN_UP;
  if (keyAnalog < 380) return BTN_DOWN;
  if (keyAnalog < 555) return BTN_LEFT;
  if (keyAnalog < 790) return BTN_SELECT;
  return BTN_NONE;
}

void printDatetime() {
  dateTimeToTimeString(myDateTime, timeStr);
  dateTimeToDateString(myDateTime, dateStr);
  lcd.setCursor(0, 0);
  lcd.print(dateStr);
  lcd.setCursor(0, 1);
  lcd.print(timeStr);
}

void changeDatetime(uint8_t mode, uint8_t operation) {
  if (operation == UP) {
    myDateTime.increment(mode);
  } else {
    myDateTime.decrement(mode);
  }
  printDatetime();
}

void handleButtonPress(uint8_t button, uint8_t& mode) {
  switch (button) {
    case BTN_UP: changeDatetime(mode, UP); break;
    case BTN_DOWN: changeDatetime(mode, DOWN); break;
    case BTN_LEFT: mode = (mode - 1 + MODES_COUNT) % MODES_COUNT; break;
    case BTN_RIGHT: mode = (mode + 1) % MODES_COUNT; break;
  }
}

void settingsMode() {
  Serial.println("Enter in setup mode");
  bool setupUp = true;
  uint8_t currentMode = HOURS_MODE;

  while (setupUp) {
    currentMillis = millis();

    if (currentMillis - lastReadButton >= readButtonInterval) {
      lastReadButton = currentMillis;
      uint8_t newButtonState = detectButton();

      if (newButtonState != BTN_NONE && newButtonState != lastButtonState) {
        if (newButtonState == BTN_SELECT) setupUp = false;
        else handleButtonPress(newButtonState, currentMode);

        lastButtonState = newButtonState;
      }
    }

    if (currentMillis - previousBlinkMillis >= blinkInterval) {
      previousBlinkMillis = currentMillis;
      isBlinkShow = !isBlinkShow;
      currentSettingsMode = isBlinkShow ? currentMode : NONE_MODE;
      printDatetime();
    }
  }
  rtc.setTime(myDateTime.second, myDateTime.minute, myDateTime.hour, myDateTime.day, myDateTime.month, myDateTime.year);
  Serial.println("Exit in setup mode");
  currentSettingsMode = NONE_MODE;
  printDatetime();
}