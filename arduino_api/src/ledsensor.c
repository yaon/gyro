/*
 * ledsensor.c
 * Source code for lab on IoT Intel Galileo
 * Author : Nicolas Vailliet <nicolas.vailliet@intel.com> 2014
 * License: Creative Commons 3.0
 */ 

#include "ledsensor.h"

#define sensorPin 0 
#define ledPin 8
int sensorvalue = 0;

void setup()
{
	printf("Setting up...");
	pinInit();
	//fastGpioSCInit();
	pinMode(ledPin, OUTPUT);
	printf("Done\n");
}

void loop()
{
	printf("Looping...\n");
	for(;;)
	{
		sensorvalue = analogRead(sensorPin);
		printf("Value: %d\n",sensorvalue);

		if(sensorvalue > 600)
			digitalWrite(ledPin,HIGH);		
		else
			digitalWrite(ledPin,LOW);
	}
	//digitalWrite(echoPin,LOW);
	//fastGpioDigitalWrite(GPIO_FAST_IO2, 0);	
}

int main( int argc, char ** argv )
{
	printf("\n *** LED and sensor demo *** \n\n");

	//Initializing log, interrupt mechanism and Arduino C lib
	//trace_init(VARIANT_TRACE_LEVEL, 0);
	//trace_target_enable(TRACE_TARGET_UART);
	interrupt_init();
	init(argc, argv);
	
	//Setting up I/O
	setup();	

	//Looping
	loop();

	//End main	
}
