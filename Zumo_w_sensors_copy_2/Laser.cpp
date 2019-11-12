#include "Laser.h"
#include <Wire.h>
#include <VL53L1X.h>

VL53L1X sensor3;
VL53L1X sensor4;


Lasers::Lasers(int L_pin, int R_pin) {

  _pinL = L_pin;
  _pinR = R_pin;
}

void Lasers::setAddy() {

  pinMode(_pinL, OUTPUT);
  pinMode(_pinR, OUTPUT);

  digitalWrite(_pinL, LOW);
  digitalWrite(_pinR, LOW);

  delay(500);

  pinMode(_pinL, INPUT);
  delay(150);
  sensor3.init(true);
  Serial.println("05");
  delay(100);
  sensor3.setAddress((uint8_t)42);
  Serial.println("06");

  pinMode(_pinR, INPUT);
  delay(150);
  sensor4.init(true);
  Serial.println("07");
  delay(100);
  sensor4.setAddress((uint8_t)41);
  Serial.println("08");


  sensor3.setTimeout(500);
  sensor4.setTimeout(500);


  sensor3.setDistanceMode(VL53L1X::Long);
  sensor3.setMeasurementTimingBudget(50000);
  sensor4.setDistanceMode(VL53L1X::Long);
  sensor4.setMeasurementTimingBudget(50000);


  sensor3.startContinuous(50);
  sensor4.startContinuous(50);

  Serial.println("addresses set");
}

void Lasers::i2cScan() {
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


double Lasers::sensL() {
  double sensL = sensor3.readRangeContinuousMillimeters();
  return sensL/25.4;
}

double Lasers::sensR() {
  double sensR = sensor4.readRangeContinuousMillimeters();
  Serial.println(sensR/25.4);
  return sensR/25.4;
}

double Lasers::getDistanceR()
{
  sensor4.read();
  double dist = sensor4.ranging_data.range_mm/25.4;
  Serial.print("range: ");
  Serial.println(dist);
  return dist;
}
