#ifndef Navigate_h
#define Navigate_h
#include <Arduino.h>

class Navigate
{
 private: 
 int destination; //Buttons distance
 public:
 // Constructor
 Navigate();
 // Methods
 void start();
 double getPosition1();
 void setPosition(int destination);
 void setPosition1(int destination);

};

#endif
