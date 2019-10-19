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
 void setPosition(int destination);

};

#endif
