#ifndef Coil_h
#define Coil_h

#include <Arduino.h>


class Coil
{
  public:
	Coil(int trigPin, int echoPin, int motorPin, int price);
    int getPrice();
    int getStock();
    boolean isEmpty();
    boolean vend();
  private:
    int _trigPin;
    int _echoPin;
    int _motorPin;
    int _getDistance();
    int _price;    
};

#endif
