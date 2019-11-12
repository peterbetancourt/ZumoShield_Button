#include "Arduino.h"
#include "Laser.h"
#include "Navigate.h"
#include "Motors.h"
#define LED_PIN 13

//Initializing Objects
Lasers Laser(12,11);

Motors motor;

Navigate::Navigate() 
{
  
}

void Navigate::start()
{
  Laser.setAddy();
  Laser.i2cScan();
  pinMode(3, OUTPUT);
}

double Navigate::getPosition1()
{
  return Laser.getDistanceR();
}

void Navigate::setPosition(int destination)
{
  double travelDist = Laser.getDistanceR() - destination;
  if(travelDist > 0)
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = 80;
    motor.setSpeeds(speed,speed);
    for(int i=0;i<=9999;i++)
    {
      if(Laser.getDistanceR()  <= destination) 
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        i = 9999;
       // digitalWrite(2, HIGH);   // push button
       // delay(1000);             // wait for a second
       // digitalWrite(2, LOW); 
       // delay(100);
      }
    }
  }
  else
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = -80;
    motor.setSpeeds(speed,speed);
    for(int i=0;i<=9999;i++)
    {
      if(Laser.getDistanceR() > destination) 
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        i = 9999;
       // digitalWrite(2, HIGH);   // push button
        //delay(1000);             // wait for a second
       // digitalWrite(2, LOW); 
       // delay(100);
      }
    }
  }
}

void Navigate::setPosition1(int destination)
{
  double travelDist = Laser.getDistanceR() - destination;
  while(travelDist > 0)
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = 80;
    motor.setSpeeds(speed,speed);
    while(travelDist > 0)
    {
      if(Laser.getDistanceR() < destination)
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        travelDist = 1;
      }
    }
  }
  while(travelDist < 0)
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = -80;
    motor.setSpeeds(speed,speed);
    while(travelDist < 0)
    {
      if(Laser.getDistanceR() == destination)
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        travelDist = 1;
      }
    }
  }
}
