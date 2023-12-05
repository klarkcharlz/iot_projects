#include <LiquidCrystal.h>
#include "lcd.h"
#include <microDS3231.h>

// LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
CustomLCD lcd(8, 9, 4, 5, 6, 7);
MicroDS3231 rtc;

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

#define UP 0
#define DOWN 1

int blinkIndices[][3] = {
  { 1, 0, 2 },  // Часы (линия, позиция и длина)
  { 1, 3, 2 },  // Минуты
  { 1, 6, 2 },  // Секунды
  { 0, 0, 2 },  // День
  { 0, 3, 2 },  // Месяц
  { 0, 6, 4 }   // Год
};
unsigned long previousBlinkMillis = 0;
const long blinkInterval = 350;
bool isBlinkShow = true;


struct RTCDateTime {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;

  void update(int y, int mo, int d, int h, int mi, int s) {
    year = y;
    month = mo;
    day = d;
    hour = h;
    minute = mi;
    second = s;
  }

  void increment(int mode) {
    switch (mode) {
      case HOURS_MODE: hour = (hour + 1) % 24; break;
      case MINUTES_MODE: minute = (minute + 1) % 60; break;
      case SECONDS_MODE: second = (second + 1) % 60; break;
      case DAYS_MODE: day = day % 31 + 1; break;
      case MONTHS_MODE: month = month % 12 + 1; break;
      case YEARS_MODE: year++; break;
    }
  }

  void decrement(int mode) {
    switch (mode) {
      case HOURS_MODE: hour = (hour + 23) % 24; break;
      case MINUTES_MODE: minute = (minute + 59) % 60; break;
      case SECONDS_MODE: second = (second + 59) % 60; break;
      case DAYS_MODE: day = (day == 1) ? 31 : day - 1; break;
      case MONTHS_MODE: month = (month == 1) ? 12 : month - 1; break;
      case YEARS_MODE: year--; break;
    }
  }

} myDateTime;

char dateStr[11];
char timeStr[9];
int lastButtonState = BTN_NONE;
int currentButtonState;

void dateTimeToTimeString(DateTime dt, char* buffer);
void dateTimeToDateString(DateTime dt, char* buffer);
int detectButton();
void printDatetime();
void changeDatetime(int mode, int operation);
void settingsMode();
void readRTC();

void setup() {
  Serial.begin(9600);
  // lcd.begin(16, 2);
  lcd.init();
  readRTC();
  printDatetime();
}


unsigned long lastDisplayUpdate = 0;
const long displayUpdateInterval = 1000;

void loop() {
  currentButtonState = detectButton();

  if (currentButtonState == BTN_SELECT && currentButtonState != lastButtonState) {
    lastButtonState = currentButtonState;
    settingsMode();
  } else {
    lastButtonState = currentButtonState;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
    lastDisplayUpdate = currentMillis;
    readRTC();
    printDatetime();
  }

  delay(10);
}

void readRTC() {
  myDateTime.update(rtc.getYear(), rtc.getMonth(), rtc.getDate(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
}

void dateTimeToTimeString(RTCDateTime dt, char* buffer) {
  snprintf(buffer, 9, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
}

void dateTimeToDateString(RTCDateTime dt, char* buffer) {
  snprintf(buffer, 11, "%02d.%02d.%04d", dt.day, dt.month, dt.year);
}

int detectButton() {
  int keyAnalog = analogRead(A0);
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
  delay(10);
}

void changeDatetime(int mode, int operation) {
  if (operation == UP) {
    myDateTime.increment(mode);
  } else {
    myDateTime.decrement(mode);
  }
  printDatetime();
}

void settingsMode() {
  Serial.println("Enter in setup mode");
  bool setupUp = true;
  int currentMode = HOURS_MODE;
  unsigned long currentMillis;

  while (setupUp) {
    currentButtonState = detectButton();

    switch (currentButtonState) {
      case BTN_UP:
        Serial.println("BTN_UP");
        if (currentButtonState != lastButtonState) {
          lastButtonState = currentButtonState;
          changeDatetime(currentMode, UP);
        }
        break;
      case BTN_DOWN:
        Serial.println("BTN_DOWN");
        if (currentButtonState != lastButtonState) {
          lastButtonState = currentButtonState;
          changeDatetime(currentMode, DOWN);
        }
        break;
      case BTN_LEFT:
        Serial.println("BTN_LEFT");
        if (currentButtonState != lastButtonState) {
          lastButtonState = currentButtonState;
          currentMode = (currentMode - 1 + MODES_COUNT) % MODES_COUNT;
        }
        break;
      case BTN_RIGHT:
        Serial.println("BTN_RIGHT");
        if (currentButtonState != lastButtonState) {
          lastButtonState = currentButtonState;
          currentMode = (currentMode + 1) % MODES_COUNT;
        }
        break;
      case BTN_SELECT:
        Serial.println("BTN_SELECT");
        if (currentButtonState != lastButtonState) {
          lastButtonState = currentButtonState;
          setupUp = false;
        }
        break;
      default:
        lastButtonState = BTN_NONE;
        break;
    }

    currentMillis = millis();

    if (currentMillis - previousBlinkMillis >= blinkInterval) {
      previousBlinkMillis = currentMillis;
      isBlinkShow = !isBlinkShow;
      for (int i = 0; i < MODES_COUNT; i++) {
        if (i == currentMode) {
          for (int j = 0; j < blinkIndices[i][2]; j++) {
            if (isBlinkShow) {
              printDatetime();
            } else {
              lcd.setCursor(blinkIndices[i][1] + j, blinkIndices[i][0]);
              // lcd.write(' ');
              lcd.print(' ');
            }
          }
        }
      }
    }

    delay(10);
  }
  rtc.setTime(myDateTime.second, myDateTime.minute, myDateTime.hour, myDateTime.day, myDateTime.month, myDateTime.year);
  Serial.println("Exit in setup mode");
  printDatetime();
}