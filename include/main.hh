#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "variant.h"
#include "wiring_digital.h"
#include "trace.h"

#define MILLISEC *1000

void readAccelData(int *destination);
void initMMA8452();
void MMA8452Standby();
void MMA8452Active();
void readRegisters(byte addressToRead, int bytesToRead, byte * dest);
byte readRegister(byte addressToRead);
void writeRegister(byte addressToWrite, byte dataToWrite);
