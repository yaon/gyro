/*
   AUTHORS

   CAILLARD Jim
   CHHIM    Ly William
   LY TANG  Guillaume

   2015

   Main file for the wifi board
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define GP_5               (17) // GPIO17 corresponds to Arduino PIN5
#define GP_6               (24) // GPIO24 corresponds to Arduino PIN6
#define GP_7               (27) // GPIO27 corresponds to Arduino PIN7
#define GPIO_DIRECTION_IN  (1)
#define GPIO_DIRECTION_OUT (0)
#define ERROR              (-1)


int openGPIO(int gpio, int direction )
{
  char buffer[256];
  int fileHandle;
  int fileMode;

  //Export GPIO
  fileHandle = open("/sys/class/gpio/export", O_WRONLY);
  if(ERROR == fileHandle)
  {
    puts("Error: Unable to opening /sys/class/gpio/export");
    return(-1);
  }
  sprintf(buffer, "%d", gpio);
  write(fileHandle, buffer, strlen(buffer));
  close(fileHandle);

  //Direction GPIO
  sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
  fileHandle = open(buffer, O_WRONLY);
  if(ERROR == fileHandle)
  {
    puts("Unable to open file:");
    puts(buffer);
    return(-1);
  }

  if (direction == GPIO_DIRECTION_OUT)
  {
    // Set out direction
    write(fileHandle, "out", 3);
    fileMode = O_WRONLY;
  }
  else
  {
    // Set in direction
    write(fileHandle, "in", 2);
    fileMode = O_RDONLY;
  }
  close(fileHandle);


  //Open GPIO for Read / Write
  sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
  fileHandle = open(buffer, fileMode);
  if(ERROR == fileHandle)
  {
    puts("Unable to open file:");
    puts(buffer);
    return(-1);
  }

  return(fileHandle);  //This file handle will be used in read/write and close operations.
}

int writeGPIO(int fHandle, int val)
{

  if(val ==  0)
  {
    // Set GPIO low status
    write(fHandle, "0", 1);
  }
  else
  {
    // Set GPIO high status
    write(fHandle, "1", 1);
  }

  return(0);
}

int readGPIO(int fileHandle)
{
  int value;

  read(fileHandle, &value, 1);

  if('0' == value)
  {
    // Current GPIO status low
    value = 0;
  }
  else
  {
    // Current GPIO status high
    value = 1;
  }

  return value;
}

int closeGPIO(int gpio, int fileHandle)
{
  char buffer[256];

  close(fileHandle); //This is the file handle of opened GPIO for Read / Write earlier.

  fileHandle = open("/sys/class/gpio/unexport", O_WRONLY);
  if(ERROR == fileHandle)
  {
    puts("Unable to open file:");
    puts(buffer);
    return(-1);
  }
  sprintf(buffer, "%d", gpio);
  write(fileHandle, buffer, strlen(buffer));
  close(fileHandle);

  return(0);
}

int main()
{
  char str[100];
  int listen_fd, comm_fd;

  int fileHandleGPIO_LED;
  int fileHandleGPIO_5;
  int fileHandleGPIO_6;
  int fileHandleGPIO_7;
  int i=0;

  fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_OUT);
  fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_OUT);
  fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_OUT);

  struct sockaddr_in servaddr;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    perror("socket\n");

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(4249);

  if (bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)))
    perror("bind");

  listen(listen_fd, 10);

  if ((comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) == -1)
    perror("accept\n");

  while(1)
  {
    bzero(str, 100);

    int i = read(comm_fd, str, 100);

    if (strlen(str+1) != 0)
      fprintf(stderr, "Echoing back -%d - %s - %d\n", strlen(str+1), str+1, i);

    switch (char c = str[1])
    {
      pid_t pid;
      fprintf(stderr, "%c\n", c);
      case 'u': // Forward
        writeGPIO(fileHandleGPIO_5, 1);
        writeGPIO(fileHandleGPIO_6, 1);
        writeGPIO(fileHandleGPIO_7, 1);
        break;
      case 'd': // Backwards
        writeGPIO(fileHandleGPIO_5, 1);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 1);
        break;
      case 'r': // Turn right
        writeGPIO(fileHandleGPIO_5, 1);
        writeGPIO(fileHandleGPIO_6, 1);
        writeGPIO(fileHandleGPIO_7, 0);
        break;
      case 'l': // Turn left
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 1);
        writeGPIO(fileHandleGPIO_7, 1);
        break;
      case 'k': // Kill asserv program
        if (pid = fork() == 0)
        {
          char *cmd[] = {"/bin/sh", "ssh.sh", "killall gyro", NULL};
          execv("/bin/sh", cmd);
          return 0;
        }
        break;
      case 'm': // Launch asserv program
        if (pid = fork() == 0)
        {
          char *cmd[] = {"/bin/sh", "ssh.sh", "killall gyro ; ./gyro", NULL};
          execv("/bin/sh", cmd);
          return 0;
        }
        break;
        break;
    }
    usleep(300);

    // Do not move
    writeGPIO(fileHandleGPIO_5, 0);
    writeGPIO(fileHandleGPIO_6, 0);
    writeGPIO(fileHandleGPIO_7, 0);

    if ((comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) == -1)
      perror("accept\n");
  }
}
