/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Motor is an interface to send values to the MD25 motor drive system
*/
#include "motors.hh"

Motors::Motors()
{
  // set to mode 3 (see documentation)
  Wire.beginTransmission(MD25);
  Wire.write(15);
  Wire.write(3);
  Wire.endTransmission();

  // set speed to 0
  Wire.beginTransmission(MD25);
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission();

  // set turn to 0
  Wire.beginTransmission(MD25);
  Wire.write(1);
  Wire.write(0);
  Wire.endTransmission();
}

void Motors::operator()(int value)
{
  Wire.beginTransmission(MD25);
  Wire.write(0);
  Wire.write(value);
  Wire.endTransmission();
}
