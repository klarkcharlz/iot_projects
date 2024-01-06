#include "main.h"

typedef enum
{
  BTN_UP = 1,
  BTN_DOWN,
  BTN_LEFT,
  BTN_RIGHT,
  BTN_SELECT,
  BTN_NONE = 10
} ButtonType;

typedef enum
{
  HOURS_MODE = 0,
  MINUTES_MODE,
  SECONDS_MODE,
  DAYS_MODE,
  MONTHS_MODE,
  YEARS_MODE,
  MODES_COUNT,
  NONE_MODE = 10
} TimeMode;

typedef enum
{
  UP = 0,
  DOWN
} Direction;

// variables
uint8_t date_str[11] = {};
uint8_t time_str[9] = {};
ButtonType last_button_state = BTN_NONE;
ButtonType current_button_state = BTN_NONE;
TimeMode current_settings_mode = NONE_MODE;
uint8_t flag_need_print_date_time = 0;
uint8_t flag_need_read_button = 0;
uint8_t flag_is_blink_show = 0;
uint8_t flag_blink_mode = 0;

// prototypes
ButtonType detectButton();
void formatNumber(uint8_t *buffer, uint8_t number, TimeMode current_mode, TimeMode target_mode);
void dateTimeToTimeString(TIME dt, uint8_t *buffer);
void dateTimeToDateString(TIME dt, uint8_t *buffer);
void printDateTime();
void settingsMode();
uint8_t handleButtonPress(ButtonType button, TimeMode mode);
void changeDateTime(TimeMode mode, Direction operation);
void decrement(TIME *dt, uint8_t mode);
void increment(TIME *dt, uint8_t mode);

/**
 * @brief Initialize Timer0 in CTC mode with a prescaler of 64 and a compare value for 10 ms.
 */
void initTimer0()
{
  TCCR0A = (1 << WGM01);              // Set CTC mode
  OCR0A = 249;                        // Compare value for 10 ms with prescaler 64 and F_CPU 16 MHz
  TCCR0B = (1 << CS01) | (1 << CS00); // Prescaler at 64
  TIMSK0 = (1 << OCIE0A);             // Enable compare match interrupt for Timer0
}

/**
 * @brief Initialize Timer1 in CTC mode with a prescaler of 1024 and a compare value for 1 second.
 */
void initTimer1()
{
  TCCR1A = 0;                                        // Set CTC mode for Timer1
  OCR1A = 15624;                                     // Compare value for 1 s with prescaler 1024 and F_CPU 16 MHz
  TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // Prescaler at 1024 and CTC mode
  TIMSK1 = (1 << OCIE1A);                            // Enable compare match interrupt for Timer1
}

/**
 * @brief Set Timer1 for a blink interval of 350ms.
 */
void setTimerFor350msBlink()
{
  TCNT1 = 0;
  OCR1A = 5461;
}

/**
 * @brief Set Timer1 for an interval of 1 second.
 */
void setTimerFor1SecondInterval()
{
  TCNT1 = 0;
  OCR1A = 15624;
}

/**
 * @brief Timer0 Compare Match A interrupt service routine to flag button reading necessity.
 */
ISR(TIMER0_COMPA_vect)
{
  // Code that runs every 10ms
  flag_need_read_button = 1;
}

/**
 * @brief Timer1 Compare Match A interrupt service routine to toggle blink flag or flag date-time printing.
 */
ISR(TIMER1_COMPA_vect)
{
  // Code that runs every second
  if (flag_blink_mode)
  {
    flag_is_blink_show = !flag_is_blink_show;
  }
  else
  {
    flag_need_print_date_time = 1;
  }
}

/**
 * @brief Main function to initialize peripherals, enter an infinite loop to process button presses and update display.
 * @return int Exit status.
 */
int main(void)
{
  adcInit();
  lcdInit();
  i2cInit();
  serialInit(9600);

  lcdClear();
  // setRtc(12, 0, 0, 1, 1, 1, 21);

  lcdSetCursor(0, 0);
  lcdPrint("Hello, World1!");
  serialPrint("Hello, World!");

  _delay_ms(3000);

  initTimer0();
  initTimer1();
  sei();
  wdt_enable(WDTO_2S);

  while (1)
  {

    if (flag_need_print_date_time)
    {
      readRtc();
      lcdClear();
      printDateTime();
      flag_need_print_date_time = 0;
    }

    if (flag_need_read_button)
    {
      current_button_state = detectButton();
      if (current_button_state == BTN_SELECT && current_button_state != last_button_state)
      {
        last_button_state = current_button_state;
        flag_need_read_button = 0;
        settingsMode();
      }
      else
      {
        flag_need_read_button = 0;
        last_button_state = current_button_state;
      }
    }

    wdt_reset();
  }

  return 0;
}

/**
 * @brief Detects the currently pressed button.
 *
 * @return ButtonType The type of the button that is pressed.
 */
ButtonType detectButton()
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

/**
 * @brief Print the date and time on the LCD display.
 */
void printDateTime()
{
  dateTimeToTimeString(RTC, time_str);
  dateTimeToDateString(RTC, date_str);
  lcdSetCursor(0, 0);
  lcdPrint(date_str);
  lcdSetCursor(0, 1);
  lcdPrint(time_str);
}

/**
 * @brief Convert the given TIME struct to a time string and format it into a buffer.
 * @param dt The TIME struct containing time information.
 * @param buffer Buffer to store the formatted time string.
 */
void dateTimeToTimeString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.hour, current_settings_mode, HOURS_MODE);
  buffer[2] = ':';
  formatNumber(buffer + 3, dt.min, current_settings_mode, MINUTES_MODE);
  buffer[5] = ':';
  formatNumber(buffer + 6, dt.sec, current_settings_mode, SECONDS_MODE);
  buffer[8] = '\0'; // terminating null character
}

/**
 * @brief Convert the given TIME struct to a date string and format it into a buffer.
 * @param dt The TIME struct containing date information.
 * @param buffer Buffer to store the formatted date string.
 */
void dateTimeToDateString(TIME dt, uint8_t *buffer)
{
  formatNumber(buffer, dt.day, current_settings_mode, DAYS_MODE);
  buffer[2] = '.';
  formatNumber(buffer + 3, dt.month, current_settings_mode, MONTHS_MODE);
  buffer[5] = '.';
  formatNumber(buffer + 6, dt.year / 100, current_settings_mode, YEARS_MODE);
  formatNumber(buffer + 8, dt.year % 100, current_settings_mode, YEARS_MODE);
  buffer[10] = '\0'; // terminating null character
}

/**
 * @brief Formats a number into a buffer depending on visibility.
 *
 * @param buffer Pointer to buffer where formatted number will be stored.
 * @param number The number to format.
 * @param current_mode The current mode of operation.
 * @param target_mode The target mode for formatting comparison.
 */
void formatNumber(uint8_t *buffer, uint8_t number, TimeMode current_mode, TimeMode target_mode)
{
  uint8_t is_visible = current_mode != target_mode;
  if (is_visible)
  {
    buffer[0] = '0' + number / 10;
    buffer[1] = '0' + number % 10;
  }
  else
  {
    buffer[0] = buffer[1] = ' ';
  }
}

/**
 * @brief Increment the specified field of the TIME struct.
 * @param dt Pointer to the TIME struct to be incremented.
 * @param mode Field of the TIME struct to increment.
 */
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

/**
 * @brief Decrement the specified field of the TIME struct.
 * @param dt Pointer to the TIME struct to be decremented.
 * @param mode Field of the TIME struct to decrement.
 */
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

/**
 * @brief Change the date or time based on the given mode and operation.
 * @param mode Mode to define which part of the date/time to change.
 * @param operation Operation (increment or decrement) to perform.
 */
void changeDateTime(TimeMode mode, Direction operation)
{
  operation == UP ? increment(&RTC, mode) : decrement(&RTC, mode);
  printDateTime();
}

/**
 * @brief Handle button press events and change the current mode or date/time accordingly.
 * @param button Button that was pressed.
 * @param mode Current mode of the application.
 * @return uint8_t The updated mode after handling the button press.
 */
uint8_t handleButtonPress(ButtonType button, TimeMode mode)
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

/**
 * @brief Mode to adjust settings of the clock, handling button presses and updating the display.
 */
void settingsMode()
{
  flag_blink_mode = 1;
  flag_need_print_date_time = 0;
  uint8_t current_mode = HOURS_MODE;
  setTimerFor350msBlink();
  uint8_t last_blink_mode = !flag_is_blink_show;
  uint8_t flag_setup_up = 1;
  serialPrint("Enter in setup mode");

  while (flag_setup_up)
  {
    if (flag_need_read_button)
    {
      current_button_state = detectButton();
      if (current_button_state != BTN_NONE && current_button_state != last_button_state)
      {
        if (current_button_state == BTN_SELECT)
          flag_setup_up = 0;
        else
          current_mode = handleButtonPress(current_button_state, current_mode);
      }
      flag_need_read_button = 0;
      last_button_state = current_button_state;
    }

    if (last_blink_mode != flag_is_blink_show)
    {
      current_settings_mode = flag_is_blink_show ? current_mode : NONE_MODE;
      printDateTime();
      last_blink_mode = flag_is_blink_show;
    }

    wdt_reset();
  }

  current_settings_mode = NONE_MODE;
  setRtc(RTC.sec, RTC.min, RTC.hour, RTC.day, RTC.date, RTC.month, RTC.year % 100);
  flag_blink_mode = 0;
  printDateTime();
  setTimerFor1SecondInterval();
  serialPrint("Exit in setup mode");
}
