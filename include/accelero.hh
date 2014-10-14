/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Header of Accelerometer
*/
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

// Accelero is an interface to conveniently manipulate the MMA852 accelerometer
class Accelero
{
  public:
    Accelero();
    // retrieves the data of the accelerometer and returns the x-axis data
    int operator()();
  private:
    // Read uint8_tsToRead sequentially,
    // starting at addressToRead into the dest uint8_t array
    void readRegisters(byte addressToRead, int bytesToRead, byte * dest);
    // Read a single uint8_t from addressToRead and return it as a uint8_t
    uint8_t readRegister(byte addressToRead);
    // Sets the MMA8452 to standby mode. It must be in standby to change most register settings
    void MMA8452Standby();
    // Writes a single uint8_t (dataToWrite) into addressToWrite
    void writeRegister(byte addressToWrite, byte dataToWrite);
    // Sets the MMA8452 to active mode. Needs to be in this mode to output data
    void MMA8452Active();
};


