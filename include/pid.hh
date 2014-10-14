/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Header of PID
*/
#pragma once

// the class used to calculate the speed of the motors
class PID
{
  public:
    PID();
    // return the new motor speed depending on the position of accelerometer
    // the value is between -128 and 127
    int operator()(int value);
  private:
    // Coefficient used for the PID
    const float kp;
    const float ki;
    const float kd;

    // error_sum is used to calculate the integrate term
    int error_sum;

    // last_error is used to calculate the derivative term
    int last_error;
};
