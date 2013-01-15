#include <Arduino.h>
#include <Display.h>
#include <LiquidCrystal.h>

Display::Display(int pin1, int pin2, int pin3, int pin4, int pin5)
{
	_lcd = lcd(pin1, pin2, pin3, pin4, pin5);
}

void Display::setString(char *str)
{
	_lcd.clear();
	_lcd.setCursor(0,1);
	_lcd.write(s);
}

void Display::setStringAndBlock(char *str, const int mins)
{
	setString(str);
	delay(mins * 1000);
}
