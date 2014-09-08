#pragma once

#include "Wire.h"

// Accelerometer I2C address 0x1D if SA0 is high, 0x1C if low
#define MMA8452_ADDRESS 0x1D

// Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB       0x01
#define XYZ_DATA_CFG    0x0E
#define WHO_AM_I        0x0D
#define CTRL_REG1       0x2A

// Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
#define GSCALE          2

class Accelero
{
  public:
    Accelero();
    int operator()();
  private:
    void readRegisters(byte addressToRead, int bytesToRead, byte * dest);
    uint8_t readRegister(byte addressToRead);
    void MMA8452Standby();
    void writeRegister(byte addressToWrite, byte dataToWrite);
    void MMA8452Active();
};


