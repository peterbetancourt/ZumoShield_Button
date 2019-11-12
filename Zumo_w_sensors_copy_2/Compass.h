#ifndef Compass_h
#define Compass_h
#include <Arduino.h>

class Compass
{
 private: 
 float startAngle;
 public:
 // Constructor
 Compass();
 // Methods
 void calibrate1();
 void setAngle(int angle);
 void autoCorrectAngle();
 void straight();
 int getAngle();

};

#endif
