#pragma once

#include "Wire.h"

// MD25 I2C address
#define MD25 0x58

class Motors
{
  public:
    Motors();
    void operator()(int value);
  private:
};
