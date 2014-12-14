/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   File that tests the accelerometer
*/
#include "accelero.hh"
#include "PID_v1.h"
#include <iostream>

int main( int argc, char ** argv )
{
  std::cout << "Test: Read accelerometer value" << std::endl;
  interrupt_init();
  init(argc, argv);

  Wire.begin();
  Accelero accelero;

  int input = accelero();
  if (input)
  {
    std::cout << "Successfully read a value: " << input << std::endl;
    return 0;
  }
  std::cout << "Error: read 0 value" << std::endl;
  return 1;
}
