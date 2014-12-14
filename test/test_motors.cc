/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   File that tests the motors
*/
#include "motors.hh"
#include "PID_v1.h"
#include <iostream>

int main( int argc, char ** argv )
{
  std::cout << "Test: Set the motors at max speed for a while" << std::endl;
  interrupt_init();
  init(argc, argv);

  Wire.begin();
  Motors motor;

  std::cout << "Setting max speed" << std::endl;
  for (unsigned int i = 0; i < 10000; i++)
    motor(128);
  std::cout << "Set it back to 0" << std::endl;
  motor(0);
  return 0;
}
