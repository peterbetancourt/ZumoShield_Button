#ifndef Laser_h
#define Laser_h


class Laser
{
 private: 
 int pin1;
 int pin2;

 public:
 // Constructor
 Laser(int pin1,int pin2);
 // Methods
 void setAddy();
 void i2cScan();
 void start();
 int getDistance1();
 int getDistance2();
 int getPosition();

};

#endif
