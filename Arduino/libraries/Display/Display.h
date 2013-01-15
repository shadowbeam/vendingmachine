#ifndef Display_h
#define Display_h

#include <Arduino.h>
#include <LiquidCrystal.h>


class Display
{
  public:
    Display(int pin1, int pin2, int pin3, int pin4, int pin5);
    void setString(const char *str);
    void setStringAndBlock(const char *str, const int mins);
  private:
    LiquidCrystal _lcd;
};

#endif
