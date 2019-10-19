#include "Arduino.h"
#include "Motors.h"
#include "Laser.h"
#include "Navigate.h"
#include "Compass.h"
#include <Wire.h>
#include <VL53L1X.h>
#include <ZumoShield.h>
#define LED_PIN 13

//Initializing Objects
Laser sensors(10,11);
Navigate nav;
Compass compass1;
void setup()
{
  //Initialize Distance Sensor Wire
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  pinMode(LED_PIN, OUTPUT);
  //Initialize Distance Sensor
  //sensors.setAddy();
  sensors.i2cScan();
  sensors.start();
  compass1.calibrate1();

}

void loop()
{
  int x;
  //Array with digits of PI
  int pi [15] = {3,1,4,1,5,9,2,6,5,3,5,8,9,7,9};
  
  for(int i = 0;pi;i++)
  {
    x = 36 - pi[i]*3;
    Serial.print("Going to Button ");
    Serial.print(pi[i]);
    Serial.print("   Which is at: ");
    Serial.print(x);
    Serial.println(" inches");
    nav.setPosition(x);
    delay(2000);  
  }
}
