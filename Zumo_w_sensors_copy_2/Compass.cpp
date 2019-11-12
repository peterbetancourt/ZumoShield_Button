#include "Arduino.h"
#include "Compass.h"
#include <ZumoShield.h>

#define SPEED           200 // Maximum motor speed when going straight; variable speed when turning
#define TURN_BASE_SPEED 100 // Base speed when turning (added to variable speed)


#define CALIBRATION_SAMPLES 70  // Number of compass readings to take when calibrating
#define CRB_REG_M_2_5GAUSS 0x60 // CRB_REG_M value for magnetometer +/-2.5 gauss full scale
#define CRA_REG_M_220HZ    0x1C // CRA_REG_M value for magnetometer 220 Hz update rate

// Allowed deviation (in degrees) relative to target angle that must be achieved before driving straight
#define DEVIATION_THRESHOLD 3

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;
Compass compass0;

Compass::Compass()
{
}

void Compass::calibrate1()
{
  // The highest possible magnetic value to read in any direction is 2047
  // The lowest possible magnetic value to read in any direction is -2047
  LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
  unsigned char index;
  
  // Initiate LSM303
  compass.init();

  // Enables accelerometer and magnetometer
  compass.enableDefault();

  compass.writeReg(LSM303::CRB_REG_M, CRB_REG_M_2_5GAUSS); // +/- 2.5 gauss sensitivity to hopefully avoid overflow problems
  compass.writeReg(LSM303::CRA_REG_M, CRA_REG_M_220HZ);    // 220 Hz compass update rate

  button.waitForButton();

  Serial.println("starting calibration");
  delay(300);
  // To calibrate the magnetometer, the Zumo spins to find the max/min
  // magnetic vectors. This information is used to correct for offsets
  // in the magnetometer data.
  motors.setLeftSpeed(SPEED);
  motors.setRightSpeed(-SPEED);

  for(index = 0; index < CALIBRATION_SAMPLES; index ++)
  {
    // Take a reading of the magnetic vector and store it in compass.m
    compass.read();

    running_min.x = min(running_min.x, compass.m.x);
    running_min.y = min(running_min.y, compass.m.y);

    running_max.x = max(running_max.x, compass.m.x);
    running_max.y = max(running_max.y, compass.m.y);

    Serial.println(index);

    delay(50);
  }

  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);

  Serial.print("max.x   ");
  Serial.print(running_max.x);
  Serial.println();
  Serial.print("max.y   ");
  Serial.print(running_max.y);
  Serial.println();
  Serial.print("min.x   ");
  Serial.print(running_min.x);
  Serial.println();
  Serial.print("min.y   ");
  Serial.print(running_min.y);
  Serial.println();

  // Set calibrated values to compass.m_max and compass.m_min
  compass.m_max.x = running_max.x;
  compass.m_max.y = running_max.y;
  compass.m_min.x = running_min.x;
  compass.m_min.y = running_min.y;
  
  Serial.println("Push Button to set the starting angle");
  button.waitForButton();
  startAngle = compass0.getAngle();
  Serial.print("Starting angle has been set! Angle: ");
  Serial.println(startAngle);
  Serial.println("Push Button to start the robot");
  button.waitForButton();
}
 
 // Converts x and y components of a vector to a heading in degrees.
 // This function is used instead of LSM303::heading() because we don't
 // want the acceleration of the Zumo to factor spuriously into the
 // tilt compensation that LSM303::heading() performs. This calculation
 // assumes that the Zumo is always level.
 template <typename T> float heading(LSM303::vector<T> v)
 {
   float x_scaled =  2.0*(float)(v.x - compass.m_min.x) / ( compass.m_max.x - compass.m_min.x) - 1.0;
   float y_scaled =  2.0*(float)(v.y - compass.m_min.y) / (compass.m_max.y - compass.m_min.y) - 1.0;

   float angle = atan2(y_scaled, x_scaled)*180 / M_PI;
   if (angle < 0)
     angle += 360;
   return angle;
 }

 // Yields the angle difference in degrees between two headings
 float relativeHeading(float heading_from, float heading_to)
 {
   float relative_heading = heading_to - heading_from;

   // constrain to -180 to 180 degree range
   if (relative_heading > 180)
     relative_heading -= 360;
   if (relative_heading < -180)
     relative_heading += 360;

   return relative_heading;
 }

 // Average 10 vectors to get a better measurement and help smooth out
 // the motors' magnetic interference.
 float averageHeading()
 {
   LSM303::vector<int32_t> avg = {0, 0, 0};

   for(int i = 0; i < 10; i ++)
   {
     compass.read();
     avg.x += compass.m.x;
     avg.y += compass.m.y;
   }
   avg.x /= 10.0;
   avg.y /= 10.0;

   // avg is the average measure of the magnetic vector.
   return heading(avg);
 }
int Compass::getAngle()
{
  
  float heading, relative_heading;
  static float target_heading = averageHeading();

  // Heading is given in degrees away from the magnetic vector, increasing clockwise
  heading = averageHeading();

  // This gives us the relative heading with respect to the target angle
  relative_heading = relativeHeading(heading, target_heading);

  // Target heading = target_heading
  // Acutal heading = heading
  // Difference = relative_heading


  Serial.print("Angle: ");
  Serial.println(heading);
  return heading;
  
}

void Compass::setAngle(int angle)
{
  float heading; 
  float relative_heading;
  int speed;
  static float target_heading = averageHeading();
  heading = averageHeading();
  target_heading = fmod(averageHeading() + angle, 360);

  relative_heading = relativeHeading(heading, target_heading);
  for(int i=0;i<=9999;i++)
  {
    heading = averageHeading();
    relative_heading = relativeHeading(heading, target_heading);
    if(abs(relative_heading) < DEVIATION_THRESHOLD)
    {
        motors.setSpeeds(0, 0);
        Serial.print("RelativeHeading: ");
        Serial.print(relative_heading);
        Serial.print("   Heading: ");
        Serial.print(heading);
        Serial.print("   TargetHeading: ");
        Serial.println(target_heading);
        i = 9999;
    }
    else
    {
      speed = SPEED*relative_heading/180;

      if (speed < 0)
        speed -= TURN_BASE_SPEED;
      else
        speed += TURN_BASE_SPEED;

      motors.setSpeeds(speed, -speed);
      
      Serial.println("Turning");
      Serial.print("Target heading: ");
      Serial.print(target_heading);
      Serial.print("    Actual heading: ");
      Serial.print(heading);
      Serial.print("    Difference: ");
      Serial.print(relative_heading);
    }
  }
}
void Compass::autoCorrectAngle()
{
  float heading; 
  float relative_heading;
  int speed;
  static float target_heading = averageHeading();
  heading = averageHeading();
  target_heading = fmod(startAngle, 360);

  relative_heading = relativeHeading(heading, target_heading);
  for(int i=0;i<=9999;i++)
  {
    heading = averageHeading();
    relative_heading = relativeHeading(heading, target_heading);
    if(abs(relative_heading) < DEVIATION_THRESHOLD)
    {
        motors.setSpeeds(0, 0);
        Serial.print("RelativeHeading: ");
        Serial.print(relative_heading);
        Serial.print("   Heading: ");
        Serial.print(heading);
        Serial.print("   TargetHeading: ");
        Serial.println(target_heading);
        i = 9999;
    }
    else
    {
      speed = SPEED*relative_heading/180;

      if (speed < 0)
        speed -= TURN_BASE_SPEED;
      else
        speed += TURN_BASE_SPEED;

      motors.setSpeeds(speed, -speed);
      
      Serial.println("Turning");
      Serial.print("Target heading: ");
      Serial.print(target_heading);
      Serial.print("    Actual heading: ");
      Serial.print(heading);
      Serial.print("    Difference: ");
      Serial.print(relative_heading);
    }
  }
}

void Compass::straight()
{
  float heading; 
  float relative_heading;
  int speed;
  static float target_heading = averageHeading();
  heading = averageHeading();
  target_heading = fmod(startAngle, 360);

  relative_heading = relativeHeading(heading, target_heading);
  for(int i=0;i<=9999;i++)
  {
    heading = averageHeading();
    relative_heading = relativeHeading(heading, target_heading);
    if(abs(relative_heading) < DEVIATION_THRESHOLD)
    {
       motors.setSpeeds(0, 0);
        i = 9999;
    }
    else
    {
      speed = SPEED*relative_heading/180;

      if (speed < 0)
      {
        speed -= TURN_BASE_SPEED;
        motors.setSpeeds(80, 150);
      }
      else
      {
        speed += TURN_BASE_SPEED;
        motors.setSpeeds(150,80);
      }
      
      Serial.println("Turning");
      Serial.print("Target heading: ");
      Serial.print(target_heading);
      Serial.print("    Actual heading: ");
      Serial.print(heading);
      Serial.print("    Difference: ");
      Serial.print(relative_heading);
    }
  }
}
