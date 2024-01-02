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
uint8_t dateStr[11] = {};
uint8_t timeStr[9] = {};
uint8_t lastButtonState = BTN_NONE;
uint8_t currentButtonState = BTN_NONE;
uint8_t currentSettingsMode = NONE_MODE;
volatile uint8_t needPrintDateTime = false;
volatile uint8_t needReadButton = false;
volatile uint8_t isBlinkShow = false;
volatile uint8_t blinkMode = false;

// prototypes
uint8_t detectButton();
void formatNumber(uint8_t *buffer, uint8_t number, uint8_t isVisible);
void dateTimeToTimeString(TIME dt, uint8_t *buffer);
void dateTimeToDateString(TIME dt, uint8_t *buffer);
void printDatetime();
void settingsMode();
uint8_t handleButtonPress(uint8_t button, uint8_t mode);
void changeDatetime(uint8_t mode, uint8_t operation);
void decrement(TIME *dt, uint8_t mode);
void increment(TIME *dt, uint8_t mode);

void initTimer0()
{
  TCCR0A = (1 << WGM01);              // Set CTC mode
  OCR0A = 249;                        // Compare value for 10 ms with prescaler 64 and F_CPU 16 MHz
  TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler at 64
  TIMSK0 = (1 << OCIE0A);             // Enable compare match interrupt for Timer0
}

void initTimer1()
{
  TCCR1A = 0;                                        // Set CTC mode for Timer1
  OCR1A = 15624;                                     // Compare value for 1 s with prescaler 1024 and F_CPU 16 MHz
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Prescaler at 1024 and CTC mode
  TIMSK1 = (1 << OCIE1A);                            // Enable compare match interrupt for Timer1
}

ISR(TIMER0_COMPA_vect)
{
  // Code that runs every 10ms
  needReadButton = true;
}

ISR(TIMER1_COMPA_vect)
{
  // Code that runs every second
  if (blinkMode)
  {
    isBlinkShow = !isBlinkShow;
  }
  else
  {
    needPrintDateTime = true;
  }
}

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
  Serial_Print("Hello, World!");

  _delay_ms(3000);

  initTimer0();
  initTimer1();
  sei();

  while (1)
  {

    if (needPrintDateTime)
    {
      readRTC();
      lcd_clear();
      printDatetime();
      needPrintDateTime = false;
    }

    if (needReadButton)
    {
      currentButtonState = detectButton();
      if (currentButtonState == BTN_SELECT && currentButtonState != lastButtonState)
      {
        lastButtonState = currentButtonState;
        needReadButton = false;
        settingsMode();
      }
      else
      {
        needReadButton = false;
        lastButtonState = currentButtonState;
      }
    }
  }

  return 0;
}

uint8_t detectButton()
{
  uint16_t keyAnalog = ADC_Read(0); // Read from channel PC0 (A0 in Arduino)
  if (keyAnalog < 50)
    return BTN_RIGHT;
  if (keyAnalog < 195)
    return BTN_UP;
  if (keyAnalog < 380)
    return BTN_DOWN;
  if (keyAnalog < 555)
    return BTN_LEFT;
  if (keyAnalog < 790)
    return BTN_SELECT;
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

void dateTimeToTimeString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.hour, currentSettingsMode != HOURS_MODE);
  buffer[2] = ':';
  formatNumber(buffer + 3, dt.min, currentSettingsMode != MINUTES_MODE);
  buffer[5] = ':';
  formatNumber(buffer + 6, dt.sec, currentSettingsMode != SECONDS_MODE);
  buffer[8] = '\0'; // terminating null character
}

void dateTimeToDateString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.day, currentSettingsMode != DAYS_MODE);
  buffer[2] = '.';
  formatNumber(buffer + 3, dt.month, currentSettingsMode != MONTHS_MODE);
  buffer[5] = '.';
  formatNumber(buffer + 6, dt.year / 100, currentSettingsMode != YEARS_MODE);
  formatNumber(buffer + 8, dt.year % 100, currentSettingsMode != YEARS_MODE);
  buffer[10] = '\0'; // terminating null character
}

void formatNumber(uint8_t *buffer, uint8_t number, uint8_t isVisible)
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

void increment(TIME *dt, uint8_t mode)
{
  switch (mode)
  {
  case HOURS_MODE:
    dt->hour = (dt->hour + 1) % 24;
    break;
  case MINUTES_MODE:
    dt->min = (dt->min + 1) % 60;
    break;
  case SECONDS_MODE:
    dt->sec = (dt->sec + 1) % 60;
    break;
  case DAYS_MODE:
    dt->day = dt->day % 31 + 1;
    break;
  case MONTHS_MODE:
    dt->month = dt->month % 12 + 1;
    break;
  case YEARS_MODE:
    dt->year++;
    break;
  }
}

void decrement(TIME *dt, uint8_t mode)
{
  switch (mode)
  {
  case HOURS_MODE:
    dt->hour = (dt->hour + 23) % 24;
    break;
  case MINUTES_MODE:
    dt->min = (dt->min + 59) % 60;
    break;
  case SECONDS_MODE:
    dt->sec = (dt->sec + 59) % 60;
    break;
  case DAYS_MODE:
    dt->day = (dt->day == 1) ? 31 : dt->day - 1;
    break;
  case MONTHS_MODE:
    dt->month = (dt->month == 1) ? 12 : dt->month - 1;
    break;
  case YEARS_MODE:
    dt->year--;
    break;
  }
}

void changeDatetime(uint8_t mode, uint8_t operation)
{
  if (operation == UP)
  {
    increment(&RTC, mode);
  }
  else
  {
    decrement(&RTC, mode);
  }
  printDatetime();
}

uint8_t handleButtonPress(uint8_t button, uint8_t mode)
{
  switch (button)
  {
  case BTN_UP:
    changeDatetime(mode, UP);
    break;
  case BTN_DOWN:
    changeDatetime(mode, DOWN);
    break;
  case BTN_LEFT:
    mode = (mode - 1 + MODES_COUNT) % MODES_COUNT;
    break;
  case BTN_RIGHT:
    mode = (mode + 1) % MODES_COUNT;
    break;
  }

  Serial_PrintInt(mode);

  return mode;
}

void settingsMode()
{
  blinkMode = true;
  needPrintDateTime = false;
  uint8_t currentMode = HOURS_MODE;
  TCNT1 = 0;
  OCR1A = 5461; // for blink 350ms
  uint8_t lastBlinkMode = !isBlinkShow;
  uint8_t setupUp = true;
  Serial_Print("Enter in setup mode");

  while (setupUp)
  {
    if (needReadButton)
    {
      currentButtonState = detectButton();
      if (currentButtonState != BTN_NONE && currentButtonState != lastButtonState)
      {
        if (currentButtonState == BTN_SELECT)
          setupUp = false;
        else
          currentMode = handleButtonPress(currentButtonState, currentMode);
      }
      needReadButton = false;
      lastButtonState = currentButtonState;
    }

    if (lastBlinkMode != isBlinkShow)
    {
      currentSettingsMode = isBlinkShow ? currentMode : NONE_MODE;
      printDatetime();
      lastBlinkMode = isBlinkShow;
    }
  }

  currentSettingsMode = NONE_MODE;
  setRTC(RTC.sec, RTC.min, RTC.hour, RTC.day, RTC.date, RTC.month, RTC.year % 100);
  blinkMode = false;
  printDatetime();
  TCNT1 = 0;
  OCR1A = 15624;
  Serial_Print("Exit in setup mode");
}
