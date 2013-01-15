#ifndef Stock_h
#define Stock_h

#include <Arduino.h>


class Stock
{
  public:
    Stock(int trigPin, int echoPin);
    int getStock();
    boolean isEmpty();
  private:
    int _trigPin;
	int _echoPinl;
    int _getDistance();
};

#endif
