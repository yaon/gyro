/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Main file that launch the asserving program
*/
#include "main.hh"
#include "PID_v1.h"
#include <iostream>

int main( int argc, char ** argv )
{
  printf("\n *** LED and sensor demo *** \n\n");

  interrupt_init();
  init(argc, argv);

  Wire.begin();
  Accelero accelero;
  Motors motor;
  double input, output;
  // the goal value we want to achieve
  double setPoint = 0.f;
  // The Z-N tuning method
  /* double ku = 0.201f; */
  /* double pu = 60.f/108.f; */
  /* double kp = 0.6f * ku; */
  /* double ki = 2.f * kp / pu; */
  /* double kd = kp * pu / 8.f; */

  // Empiric terms we found manually
  double kp = 0.075f;
  double ki = 0.25f;
  double kd = 0.0001f;

  PID pid(&input, &output, &setPoint, kp, ki, kd, DIRECT);
  pid.SetOutputLimits(-128, 127);
  pid.SetSampleTime(1);
  pid.SetMode(AUTOMATIC);

  while (46)
  {
    // read the accelero value
    input = accelero();

    // value is between 0 and 1024 whenever the gyropod is tilted forward
    // and between 4096 - 1024 and 4096 if it is tilted backward
    input = input < 1024 ? - input : 4096 - input;
    std::cerr << "accel value " << input << std::endl;

    pid.Compute();
    std::cerr << "output val " << output << std::endl;

    std::cerr << "[D]: Input val " << input << " output val " << output << std::endl;
    motor((int)output);
  }
}
