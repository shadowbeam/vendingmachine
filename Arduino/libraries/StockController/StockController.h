#ifndef Stock_h
#define Stock_h

#include <Arduino.h>


class StockController
{
  public:
    StockController(int trigPin, int echoPin);
    int getStock();
    boolean isEmpty();
  private:
    int _trigPin;
	int _echoPin;
    int _getDistance();
};

#endif
