/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Header of Motors
*/
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

    // make the motors turn at the given value (between -128 and 127)
    void turn(int value);
  private:
};
