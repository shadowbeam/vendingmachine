#include <Arduino.h>
#include <Coil.h>
#include <Servo.h>


Coil::Coil(int trigPin, int echoPin, int motorPin)
{
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT); 
	_trigPin = trigPin;
	_echoPin = echoPin;
	_motorPin = motorPin;
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

void Coil::vend()
{  
  Servo servo;
  servo.attach(_motorPin);
  servo.write(0);
  delay(1350);
  servo.write(90);
  delete &servo;
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
