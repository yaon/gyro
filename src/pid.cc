/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015
*/
#include "pid.hh"

PID::PID() {}

int PID::operator()(int value)
{
  if (value > 3996)
    return -30;
  else if (value < 100)
    return 30;
  else if (value > 3796)
    return -40;
  else if (value < 300)
    return 40;
  else if (value < 1024)
    return 50;
  else if (value > 2096)
    return -50;
  else
    return 0;
}
