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

#define INIT_SETPOINT 30.f

/**
  These are the pins used to communicate with the other board in order to
  receive the command input

   pin#  5 6 7
   up    1 1 1
   down  1 0 1
   left  1 1 0
   right 0 1 1
**/
static const int pin1 = 5;
static const int pin2 = 6;
static const int pin3 = 7;

// Read the GPIO and execute the command
static void read_gpio(double& setPoint, Motors& motors)
{
  static bool connected = false;
  // The board is considered as connected when the other board will have
  // successfully written 0 in all pins 10 times in a row
  static int cpt = 0;

  // Reading gpio is disabled in order to take into account one command at a
  // time for a short duration
  static bool enable_read = true;
  static int disable_read_cpt = 0;
  if (!enable_read)
  {
    if (++disable_read_cpt == 100)
      enable_read = true;
    return;
  }

  int val1 = digitalRead(pin1);
  int val2 = digitalRead(pin2);
  int val3 = digitalRead(pin3);

  if (connected)
  {
    std::cerr
      << "Read value in pin" << pin1 << ": " << val1 << "\t"
      << "Read value in pin" << pin2 << ": " << val2 << "\t"
      << "Read value in pin" << pin3 << ": " << val3 << "\t"
      << std::endl;

    // Moving up/down doesn't work yet, but the idea is to move the setPoint in
    // order to destabilize the robot so it moves while trying to gain balance
    if (val1 == 1 && val2 == 1 && val3 == 1)
    {
      std::cerr << "UP COMMAND RECEIVED" << std::endl;
      //setPoint = INIT_SETPOINT + 100;
    }
    else if (val1 == 1 && val2 == 0 && val3 == 1)
    {
      std::cerr << "UP COMMAND RECEIVED" << std::endl;
      //setPoint = INIT_SETPOINT - 100;
    }
    else if (val1 == 1 && val2 == 1 && val3 == 0)
    {
      std::cerr << "LEFT COMMAND RECEIVED" << std::endl;
      motors.turn(1);
    }
    else if (val1 == 0 && val2 == 1 && val3 == 1)
    {
      std::cerr << "RIGHT COMMAND RECEIVED" << std::endl;
      motors.turn(-1);
    }
    else
    {
      motors.turn(0);
      setPoint = INIT_SETPOINT;
      return;
    }
    enable_read = false;
    disable_read_cpt = 0;
  }
  else
  {
    if (val1 == 0 && val2 == 0 && val3 == 0)
    {
      if (++cpt == 10)
      {
        connected = true;
        std::cerr << "Connection success" << std::endl;
      }
    }
    else
    {
      std::cerr << "Waiting for connection" << std::endl;
      connected = false;
      cpt = 0;
    }
  }
}

int main(int argc, char ** argv)
{
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  interrupt_init();
  init(argc, argv);

  Wire.begin();
  Accelero accelero;
  Motors motor;
  double input, output;
  // the goal value we want to achieve
  double setPoint = INIT_SETPOINT;
  // The Z-N tuning method
  /* double ku = 0.201f; */
  /* double pu = 60.f/108.f; */
  /* double kp = 0.6f * ku; */
  /* double ki = 2.f * kp / pu; */
  /* double kd = kp * pu / 8.f; */

  // Empiric terms we found manually
  double kp = 0.17f;//075f;
  double ki = 0.0f;//25f;
  double kd = 0.f;//0001f;

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

    read_gpio(setPoint, motor);

    pid.Compute();

    std::cerr << "[D]: Input val " << input << " output val " << output << std::endl;
    motor((int)output);
  }
}
