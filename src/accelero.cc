#include "accelero.hh"

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
// void initMMA8452()
Accelero::Accelero()
{
  uint8_t c = readRegister(WHO_AM_I);  // Read WHO_AM_I register
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {
    printf("MMA8452Q is online...\n");
  }
  else
  {
    printf("Could not connect to MMA8452Q: 0x%d\n", c);
    while(1); // Loop forever if communication doesn't happen
  }

  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  uint8_t fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active();  // Set to active to start reading
}

int Accelero::operator()()
{
  int destination[3];  // Stores the 12-bit signed value
  uint8_t rawData[6];  // x/y/z accel register data stored here

  // Read the six raw data registers into data array
  readRegisters(OUT_X_MSB, 6, rawData);

  // Loop to calculate 12-bit ADC and g value for each axis
  for(int i = 0; i < 3 ; i++)
  {
    //Combine the two 8 bit registers into one 12-bit number
    int gCount = (rawData[i*2] << 8) | rawData[(i*2)+1];
    //The registers are left align, here we right align the 12-bit integer
    gCount >>= 4;

    // If the number is negative, we have to make it so manually
    // (no 12-bit data type)
    if (rawData[i*2] > 0x7F)
    {
      gCount = ~gCount + 1;
      gCount *= -1;  // Transform into negative 2's complement #
    }

    destination[i] = gCount; //Record this gCount into the 3 int array
  }

  // Return only X axis
  return destination[0];
}

void Accelero::writeRegister(uint8_t addressToWrite, uint8_t dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}

void Accelero::readRegisters(uint8_t addressToRead, int uint8_tsToRead, uint8_t * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  //Ask for uint8_ts, once done, bus is released by default
  Wire.requestFrom(MMA8452_ADDRESS, uint8_tsToRead);

  //Hang out until we get the # of uint8_ts we expect
  while(Wire.available() < uint8_tsToRead);

  for(int x = 0 ; x < uint8_tsToRead ; x++)
    dest[x] = Wire.read();
}

uint8_t Accelero::readRegister(uint8_t addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  //Ask for 1 uint8_t, once done, bus is released by default
  Wire.requestFrom(MMA8452_ADDRESS, 1);

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one uint8_t
}

void Accelero::MMA8452Standby()
{
  uint8_t c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

void Accelero::MMA8452Active()
{
  uint8_t c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}
