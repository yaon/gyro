/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   PID is the asserving algorithm used to set the speed of the motors
*/
#include "pid.hh"
#include <iostream>

PID::PID()
  : kp(1), ki(1), kd(1), last_error(0), error_sum(0)
{}

int PID::operator()(int value)
{
  // values of the pid
  int p_val = 0;
  int i_val = 0;
  int d_val = 0;

  // value is between 0 and 1024 whenever the gyropod is tilted forward
  // and between 4096 - 1024 and 4096 if it is tilted backward
  int error = value < 1024 ? value : value - 4096;

  // proportional term
  p_val = error * kp;

  // integrate term
  error_sum += error;
  i_val = error_sum * ki;

  // derivative term
  d_val = last_error * kd;

  int pid = p_val + i_val + d_val;

  // update the last error for the next iteration
  last_error = error;

  // control the max value of the pid
  if (pid > 1023)
    pid = 1023;
  else if (pid < -1024)
    pid = -1024;

  // set the value between -128 and 127 for the motor
  pid = (pid * 128) / 1024;

  // debug check
  if (pid > 127)
  {
    std::cerr << "[D]: Error on the pid value (> 127) : " << pid << std::endl;
    pid = 127;
  }
  else if (pid < -128)
  {
    std::cerr << "[D]: Error on the pid value (< -128) : " << pid << std::endl;
    pid = -128;
  }

  return pid;
}
