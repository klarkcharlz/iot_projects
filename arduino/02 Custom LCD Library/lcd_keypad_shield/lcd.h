#pragma once  // Предотвращает повторное включение одного и того же заголовочного файла

#include <Arduino.h>  // Подключение основного файла Arduino для работы с функциями и типами данных Arduino

class CustomLCD {
private:
  uint8_t rs, enable, d4, d5, d6, d7;

  void sendchar(unsigned char c);                      // Метод для отправки символа на LCD
  void sendbyte(unsigned char c, unsigned char mode);  // Метод для отправки байта данных или команды
  void sendhalfbyte(unsigned char c);                  // Метод для отправки половины байта (так как используется 4-битный режим)

public:
  CustomLCD(uint8_t rs, uint8_t enable, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);  // Конструктор
  void init(void);                                                                        // Метод для инициализации дисплея
  void clear(void);                                                                       // Метод для очистки экрана
  void setCursor(uint8_t x, uint8_t y);                                                      // Метод для установки позиции курсора
  void print(char str[]);                                                                 // Метод для печати строки на экран
};
