#include "Arduino.h"
#include "Laser.h"
#include "Navigate.h"
#include "Motors.h"
#define LED_PIN 13

//Initializing Objects
Laser sensor(8,9);
Motors motor;

Navigate::Navigate() 
{
}

void Navigate::setPosition(int destination)
{
  int travelDist = sensor.getDistance1() - destination;
  if(travelDist > 0)
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = 80;
    motor.setSpeeds(speed,speed);
    for(int i=0;i<=9999;i++)
    {
      if(sensor.getDistance1() < destination) 
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        i = 9999;
      }
      delay(0);
    }
  }
  else if (travelDist < 0)
  {
    digitalWrite(LED_PIN, HIGH);
    int speed = -80;
    motor.setSpeeds(speed,speed);
    for(int i=0;i<=9999;i++)
    {
      if(sensor.getDistance1() == destination) 
      {
        motor.setSpeeds(0,0);
        digitalWrite(LED_PIN, LOW);
        i = 9999;
      }
      delay(0);
    }
  }
}
