#include <Arduino.h>
#include <StockController.h>

StockController::StockController(int trigPin, int echoPin)
{
	_trigPin = trigPin;
	_echoPin = echoPin;
	pinMode(trigPin, INPUT);
	pinMode(echoPin, OUTPUT);
}

int StockController::getStock()
{
	int cm = _getDistance();
	return 5 - (cm / 2);
}

boolean StockController::isEmpty()
{
	return getStock() == 0;
}

int StockController::_getDistance()
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
