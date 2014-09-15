#pragma once

#include "Wire.h"

// MD25 I2C address
#define MD25 0x58

// the class is used to conveniently use the MD25 motors drive system
class Motors
{
  public:
    Motors();
    // makes the motors move at the value given (between -128 and 127)
    void operator()(int value);
  private:
};
