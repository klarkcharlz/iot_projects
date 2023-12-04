#include "lcd.h"

// Конструктор класса
CustomLCD::CustomLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
  : rs(rs), enable(enable), d4(d4), d5(d5), d6(d6), d7(d7) {}

// Инициализация LCD
void CustomLCD::init() {
  // Установка режима вывода для пинов
  pinMode(rs, OUTPUT);
  pinMode(enable, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);

  // Начальная инициализация дисплея команды и задержки из даташита
  delay(15);
  sendbyte(0b00000011, LOW);
  delay(4);
  sendbyte(0b00000011, LOW);
  delayMicroseconds(100);
  sendbyte(0b00000011, LOW);
  delay(1);
  sendbyte(0b00000010, LOW);
  delay(1);

  // Конфигурация LCD
  sendbyte(0b00101000, LOW);  // 4бит-режим (DL=0) и 2 линии (N=1)
  delay(1);
  sendbyte(0b00001100, LOW);  // включаем изображение на дисплее (D=1), курсоры никакие не включаем (C=0, B=0)
  delay(1);
  sendbyte(0b00000110, LOW);  // курсор (хоть он у нас и невидимый) будет двигаться влево
  delay(1);
  clear();
}

// Очистка дисплея
void CustomLCD::clear() {
  sendbyte(0b00000001, LOW);  // Команда очистки дисплея
  delayMicroseconds(1500);    // Ожидание завершения команды
}

// Установка позиции курсора
void CustomLCD::setCursor(uint8_t x, uint8_t y) {
  char adress;
  adress = (0x40 * y + x) | 0b10000000;  // Вычисление адреса в памяти LCD
  sendbyte(adress, LOW);                 // Команда установки позиции курсора
}

// Вывод текста на дисплей
void CustomLCD::print(char str[]) {
  wchar_t n;
  for (n = 0; str[n] != '\0'; n++)  // Перебор всех символов в строке
    sendchar(str[n]);               // Отправка каждого символа
}

// Отправка символа
void CustomLCD::sendchar(unsigned char c) {
  sendbyte(c, 1);  // Отправка символа с установленным битом RS (данные)
}

// Отправка байта данных или команды
void CustomLCD::sendbyte(unsigned char c, unsigned char mode) {
  digitalWrite(rs, mode);  // Установка режима RS (HIGH для данных, LOW для команд)
  unsigned char hc = 0;
  hc = c >> 4;       // Получение старшей тетрады байта
  sendhalfbyte(hc);  // Отправка старшей тетрады
  sendhalfbyte(c);   // Отправка младшей тетрады
}

void CustomLCD::sendhalfbyte(unsigned char value) {
  value <<= 4;                 // Сдвиг значений в старшие разряды для соответствия пинам D4-D7
  digitalWrite(enable, HIGH);  //включаем линию Е
  delayMicroseconds(50);
  digitalWrite(d4, value & 0x10);
  digitalWrite(d5, value & 0x20);
  digitalWrite(d6, value & 0x40);
  digitalWrite(d7, value & 0x80);
  digitalWrite(enable, LOW);  //выключаем линию Е
  delayMicroseconds(50);
}
