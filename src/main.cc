#include "main.hh"

int main( int argc, char ** argv )
{
  printf("\n *** LED and sensor demo *** \n\n");

  interrupt_init();
  init(argc, argv);

  Wire.begin();
  Accelero accelero;
  Motors motor;
  PID pid;

  while (46)
  {
    motor(pid(accelero()));
  }
}
