#include "motors.hh"

Motors::Motors()
{
  Wire.beginTransmission(MD25);
  Wire.write(15);
  Wire.write(3);
  Wire.endTransmission();
}

void Motors::operator()(int value)
{
  Wire.beginTransmission(MD25);
  Wire.write(0);
  Wire.write(value);
  Wire.endTransmission();
}
