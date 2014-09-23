/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015
*/
#pragma once

// the class used to calculate the speed of the motors
class PID
{
  public:
    PID();
    // return the new motor speed depending on the position of accelerometer
    int operator()(int value);
  private:
};
