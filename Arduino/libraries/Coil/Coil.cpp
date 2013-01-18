#include <Arduino.h>
#include <Coil.h>
#include <EEPROM.h>
#include <Servo.h>


Coil::Coil(int trigPin, int echoPin, int motorPin, int eepromLoc)
{
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT); 
	_trigPin = trigPin;
	_echoPin = echoPin;
	_motorPin = motorPin;
  _eepromLoc = eepromLoc;
}

int Coil::getStock()
{
	return EEPROM.read(_eepromLoc);
}

void Coil::setStock(int quant)
{
	EEPROM.write(_eepromLoc, quant);
}

boolean Coil::isEmpty()
{
	return getStock() == 0;
}

void Coil::vend()
{  
  Servo servo;
  servo.attach(_motorPin);
  servo.write(0);
  delay(1350);
  servo.write(90);
  delete &servo;
}
