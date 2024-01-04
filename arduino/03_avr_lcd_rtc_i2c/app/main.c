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

// variables
uint8_t date_str[11] = {};
uint8_t time_str[9] = {};
uint8_t last_button_state = BTN_NONE;
uint8_t current_button_state = BTN_NONE;
uint8_t current_settings_mode = NONE_MODE;
uint8_t need_print_date_time = 0;
uint8_t need_read_button = 0;
uint8_t is_blink_show = 0;
uint8_t blink_mode = 0;

// prototypes
uint8_t detectButton();
void formatNumber(uint8_t *buffer, uint8_t number, uint8_t isVisible);
void dateTimeToTimeString(TIME dt, uint8_t *buffer);
void dateTimeToDateString(TIME dt, uint8_t *buffer);
void printDateTime();
void settingsMode();
uint8_t handleButtonPress(uint8_t button, uint8_t mode);
void changeDateTime(uint8_t mode, uint8_t operation);
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
  need_read_button = 1;
}

ISR(TIMER1_COMPA_vect)
{
  // Code that runs every second
  if (blink_mode)
  {
    is_blink_show = !is_blink_show;
  }
  else
  {
    need_print_date_time = 1;
  }
}

int main(void)
{
  adcInit();
  lcdInit();
  i2cInit();
  serialInit(9600);

  lcdClear();
  setRtc(12, 0, 0, 1, 1, 1, 21);

  lcdSetCursor(0, 0);
  lcdPrint("Hello, World1!");
  serialPrint("Hello, World!");

  _delay_ms(3000);

  initTimer0();
  initTimer1();
  sei();

  while (1)
  {

    if (need_print_date_time)
    {
      readRtc();
      lcdClear();
      printDateTime();
      need_print_date_time = 0;
    }

    if (need_read_button)
    {
      current_button_state = detectButton();
      if (current_button_state == BTN_SELECT && current_button_state != last_button_state)
      {
        last_button_state = current_button_state;
        need_read_button = 0;
        settingsMode();
      }
      else
      {
        need_read_button = 0;
        last_button_state = current_button_state;
      }
    }
  }

  return 0;
}

uint8_t detectButton()
{
  uint16_t keyAnalog = adcRead(0); // Read from channel PC0 (A0 in Arduino)
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

void printDateTime()
{
  dateTimeToTimeString(RTC, time_str);
  dateTimeToDateString(RTC, date_str);
  lcdSetCursor(0, 0);
  lcdPrint(date_str);
  lcdSetCursor(0, 1);
  lcdPrint(time_str);
}

void dateTimeToTimeString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.hour, current_settings_mode != HOURS_MODE);
  buffer[2] = ':';
  formatNumber(buffer + 3, dt.min, current_settings_mode != MINUTES_MODE);
  buffer[5] = ':';
  formatNumber(buffer + 6, dt.sec, current_settings_mode != SECONDS_MODE);
  buffer[8] = '\0'; // terminating null character
}

void dateTimeToDateString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.day, current_settings_mode != DAYS_MODE);
  buffer[2] = '.';
  formatNumber(buffer + 3, dt.month, current_settings_mode != MONTHS_MODE);
  buffer[5] = '.';
  formatNumber(buffer + 6, dt.year / 100, current_settings_mode != YEARS_MODE);
  formatNumber(buffer + 8, dt.year % 100, current_settings_mode != YEARS_MODE);
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

void changeDateTime(uint8_t mode, uint8_t operation)
{
  operation == UP ? increment(&RTC, mode) : decrement(&RTC, mode);
  printDateTime();
}

uint8_t handleButtonPress(uint8_t button, uint8_t mode)
{
  switch (button)
  {
  case BTN_UP:
    changeDateTime(mode, UP);
    break;
  case BTN_DOWN:
    changeDateTime(mode, DOWN);
    break;
  case BTN_LEFT:
    mode = (mode - 1 + MODES_COUNT) % MODES_COUNT;
    break;
  case BTN_RIGHT:
    mode = (mode + 1) % MODES_COUNT;
    break;
  }

  serialPrintInt(mode);

  return mode;
}

void settingsMode()
{
  blink_mode = 1;
  need_print_date_time = 0;
  uint8_t currentMode = HOURS_MODE;
  TCNT1 = 0;
  OCR1A = 5461; // for blink 350ms
  uint8_t lastBlinkMode = !is_blink_show;
  uint8_t setupUp = 1;
  serialPrint("Enter in setup mode");

  while (setupUp)
  {
    if (need_read_button)
    {
      current_button_state = detectButton();
      if (current_button_state != BTN_NONE && current_button_state != last_button_state)
      {
        if (current_button_state == BTN_SELECT)
          setupUp = 0;
        else
          currentMode = handleButtonPress(current_button_state, currentMode);
      }
      need_read_button = 0;
      last_button_state = current_button_state;
    }

    if (lastBlinkMode != is_blink_show)
    {
      current_settings_mode = is_blink_show ? currentMode : NONE_MODE;
      printDateTime();
      lastBlinkMode = is_blink_show;
    }
  }

  current_settings_mode = NONE_MODE;
  setRtc(RTC.sec, RTC.min, RTC.hour, RTC.day, RTC.date, RTC.month, RTC.year % 100);
  blink_mode = 0;
  printDateTime();
  TCNT1 = 0;
  OCR1A = 15624;
  serialPrint("Exit in setup mode");
}
