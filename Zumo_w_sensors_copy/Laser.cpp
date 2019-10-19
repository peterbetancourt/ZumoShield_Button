#include "Arduino.h"
#include "Laser.h"
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor1;
VL53L1X sensor2;
 Laser::Laser(int pin1,int pin2) 
{
   this->pin1;
   this->pin2;
}
void Laser::setAddy()
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  delay(500);

  pinMode(pin1, INPUT);
  delay(150);
  Serial.println("00");
  sensor1.init(true);

  Serial.println("01");
  delay(100);
  sensor1.setAddress((uint8_t)22);
  Serial.println("02");

  pinMode(pin2, INPUT);
  delay(150);
  sensor2.init(true);
  Serial.println("03");
  delay(100);
  sensor2.setAddress((uint8_t)43);
  Serial.println("04");

  Serial.println("addresses set");
  
  sensor1.setTimeout(500);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor1.startContinuous(50);
}

void Laser::i2cScan() 
{
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 1; i < 120; i++)
  {

    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0)
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);
    }
  }
  Serial.println ("Done.");
  Serial.print ("Found ");
  Serial.print (count, DEC);
  Serial.println (" device(s).");

}
void Laser::start()
{
  sensor1.setTimeout(500);
  if (!sensor1.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1);
  }
  
  // Use long distance mode and allow up to 50000 us (50 ms) for a measurement.
  // You can change these settings to adjust the performance of the sensor, but
  // the minimum timing budget is 20 ms for short distance mode and 33 ms for
  // medium and long distance modes. See the VL53L1X datasheet for more
  // information on range and timing limits.
  sensor1.setDistanceMode(VL53L1X::Medium);
  sensor1.setMeasurementTimingBudget(50000);

  // Start continuous readings at a rate of one measurement every 50 ms (the
  // inter-measurement period). This period should be at least as long as the
  // timing budget.
  sensor1.startContinuous(50);
}

int Laser::getDistance1()
{
  sensor1.read();
  double dist = sensor1.ranging_data.range_mm/25.4;
  Serial.print("range: ");
  Serial.println(dist);
  return dist;
}

int Laser::getDistance2()
{
  sensor2.read();
  double dist = sensor2.ranging_data.range_mm/25.4;
  Serial.print("range: ");
  Serial.println(dist);
  return dist;
}
int Laser::getPosition()
{
  //int x = getDistance1();
  int y = getDistance2();
  Serial.print("The coordinates are: X= ");
  //Serial.println(x);
  Serial.print(" Y= ");
  Serial.println(y);
}
