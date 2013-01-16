#include <Arduino.h>
#include <Coil.h>


Coil::Coil(int trigPin, int echoPin, int motorPin, int price)
{
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT); 
	_trigPin = trigPin;
	_echoPin = echoPin;
	_motorPin = motorPin;
  _price = price;
}

int Coil::getPrice()
{
	return _price;
}

int Coil::getStock()
{
	int cm = _getDistance();
	return 5 - (cm / 2);
}

boolean Coil::isEmpty()
{
	return getStock() == 0;
}

boolean Coil::vend()
{
	if (!isEmpty()) {
		// TODO MOTOR ACTION;
		return true;
	}
	return false;
}

int Coil::_getDistance()
{
	long duration;
	digitalWrite(_trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(_trigPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(_trigPin, LOW);
	duration = pulseIn(_echoPin, HIGH);
	return duration / 2 / 29.39;
}
