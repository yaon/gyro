#include <iostream>
#include <thread>
#include "main.hh"
#include "Wire.h"

#define MMA8452_ADDRESS 0x1D  // 0x1D if SA0 is high, 0x1C if low
#define MD25 0x58

//Define a few of the registers that we will be accessing on the MMA8452
#define OUT_X_MSB 0x01
#define XYZ_DATA_CFG  0x0E
#define WHO_AM_I   0x0D
#define CTRL_REG1  0x2A

// Sets full-scale range to +/-2, 4, or 8g. Used to calc real g values.
#define GSCALE 2

byte foo();

void setup()
{
  printf("MMA8452 Basic Example\n");
  Wire.begin();

  initMMA8452(); //Test and intialize the MMA8452
  printf("TAMERE JE LA BAISE\n");
  printf("DEBUG BATTERY VOLT %d\n", foo());
  while (42) {}
}

void loop()
{
  printf("Looping...\n");
  for(;;)
  {
    int accelCount[3];  // Stores the 12-bit signed value
    readAccelData(accelCount);  // Read the x/y/z adc values

    // Now we'll calculate the accleration value into actual g's
    float accelG[3];  // Stores the real accel value in g's
    for (int i = 0 ; i < 3 ; i++)
    {
      // get actual g value, this depends on scale being set
      accelG[i] = (float) accelCount[i] / ((1<<12)/(2*GSCALE));
    }

    // Print out values
    for (int i = 0 ; i < 3 ; i++)
    {
      printf("%.4f\t", accelG[i]);
    }
    printf("\n");

    delay(10);  // Delay here for visibility
  }
}

void readAccelData(int *destination)
{
  byte rawData[6];  // x/y/z accel register data stored here

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
}

// Initialize the MMA8452 registers
// See the many application notes for more info on setting all of these registers:
// http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=MMA8452Q
void initMMA8452()
{
  byte c = readRegister(WHO_AM_I);  // Read WHO_AM_I register
  if (c == 0x2A) // WHO_AM_I should always be 0x2A
  {
    printf("MMA8452Q is online...\n");
  }
  else
  {
    printf("Could not connect to MMA8452Q: 0x%d\n", c);
    while(1) ; // Loop forever if communication doesn't happen
  }

  MMA8452Standby();  // Must be in standby to change registers

  // Set up the full scale range to 2, 4, or 8g.
  byte fsr = GSCALE;
  if(fsr > 8) fsr = 8; //Easy error check
  fsr >>= 2; // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
  writeRegister(XYZ_DATA_CFG, fsr);

  //The default data rate is 800Hz and we don't modify it in this example code

  MMA8452Active();  // Set to active to start reading
}

// Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Standby()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Active()
{
  byte c = readRegister(CTRL_REG1);
  writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// Read bytesToRead sequentially,
// starting at addressToRead into the dest byte array
void readRegisters(byte addressToRead, int bytesToRead, byte * dest)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  //Ask for bytes, once done, bus is released by default
  Wire.requestFrom(MMA8452_ADDRESS, bytesToRead);

  //Hang out until we get the # of bytes we expect
  while(Wire.available() < bytesToRead);

  for(int x = 0 ; x < bytesToRead ; x++)
    dest[x] = Wire.read();
}

// Read a single byte from addressToRead and return it as a byte
byte readRegister(byte addressToRead)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToRead);
  Wire.endTransmission(false); //endTransmission but keep the connection active

  //Ask for 1 byte, once done, bus is released by default
  Wire.requestFrom(MMA8452_ADDRESS, 1);

  while(!Wire.available()) ; //Wait for the data to come back
  return Wire.read(); //Return this one byte
}

byte foo()
{
  Wire.beginTransmission(MD25);
  // Activate mode 3 of the motors
  Wire.write(15);
  Wire.write(3);
  Wire.endTransmission(); //endTransmission but keep the connection active

  for (unsigned int i = 0; i < 10000; i++)
  {
    Wire.beginTransmission(MD25);
    // set speed to max
    Wire.write(0);
    Wire.write(127);
    Wire.endTransmission(); //endTransmission but keep the connection active
  }
  printf("DEBUG STOPEU\n");
  Wire.beginTransmission(MD25);
  // set speed to 0 before ending
  Wire.write(0);
  Wire.write(0);
  Wire.endTransmission(); //endTransmission but keep the connection active
  //Ask for 1 byte, once done, bus is released by default
  /* Wire.requestFrom(MD25|0x1, 1); */

  /* while(!Wire.available()) ; //Wait for the data to come back */
  /* return Wire.read(); //Return this one byte */
  return 23;
}

// Writes a single byte (dataToWrite) into addressToWrite
void writeRegister(byte addressToWrite, byte dataToWrite)
{
  Wire.beginTransmission(MMA8452_ADDRESS);
  Wire.write(addressToWrite);
  Wire.write(dataToWrite);
  Wire.endTransmission(); //Stop transmitting
}

int main( int argc, char ** argv )
{
  printf("\n *** LED and sensor demo *** \n\n");

  interrupt_init();
  init(argc, argv);

  //Setting up I/O
  setup();

  //Looping
  loop();

  //End main
}
