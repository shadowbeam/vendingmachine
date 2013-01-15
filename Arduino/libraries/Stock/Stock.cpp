#include <Arduino.h>
#include <Stock.h>

Stock::Stock(int trigPin, int echoPin)
{
	_trigPin = trigPin;
	_echoPin = echoPin;
	pinMode(trigPin, INPUT);
	pinMode(echoPin, OUTPUT);
}

int Stock::getStock()
{
	int cm = _getDistance();
	return 5 - (cm / 2);
}

boolean Stock::isEmpty()
{
	return getStock == 0;
}

int Stock::_getDistance()
{
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  return duration / 2 / 29.39;
}
