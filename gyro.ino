#include <AXELCD.h>

#include <MD23.h>
#include <Wire.h>

#include <math.h>


////////////////////////////////////////////
#define NUMREADINGS 5 //Gyro noise filter //
int readings[NUMREADINGS];                // the readings from the analog input (gyro)
int index = 0;                            // the index of the current reading
int total = 0;                            // the running total
int average = 0;                          // the average
int inputPin =0;                          // Gyro Analog input
////////////////////////////////////////////

float dt = .1; // .06;     // ( 1024.0 * 256.0 ) / 16000000.0; (Kalman) // what does this means ?? hand tuned
int mydt = 20; // in ms.

static float PP[2][2] = { // (Kalman)
  { 1, 0 } ,              // (Kalman)
  { 0, 1 } ,              // (Kalman)
};                        // (Kalman)


/*
 * Our two states, the angle and the gyro bias.  As a byproduct of computing
 * the angle, we also havef an unbiased angular rate available.   These are
 * read-only to the user of the module.
 */
float angle = 0.0; //(Kalman)
float q_bias = 0.0; //(Kalman)
float rate = 0.0; //(Kalman)
float q_m = 0.0; //(Kalman)

int ax_m = 0;
int ay_m = 0;
int cnt = 0; //Counter
unsigned long lastread = 0;
unsigned long startmillis = 0;

/*
 * R represents the measurement covariance noise.  In this case,
 * it is a 1x1 matrix that says that we expect 0.3 rad jitter
 * from the accelerometer.
 */
float R_angle = .3; //.3 deafault, but is adjusted externally (Kalman)


/*
 * Q is a 2x2 matrix that represents the process covariance noise.
 * In this case, it indicates how much we trust the acceleromter
 * relative to the gyros.
 */
static const float Q_angle = 0.001; //(Kalman)
static const float Q_gyro = 0.003; //(Kalman)
int ledPin = 13;

MD23 md23;
AXELCD lcd;

byte displayState = 0; // 0 clean,
float oldAngle = 0.0; //

float P = 0.0;
float I = 0.0;
float D = 0.0;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(2, INPUT);     // declare pushbutton as input

  delay(500);
  Wire.begin(); // init i2c
  md23.setMode(0);  // set the mode (0 is default, so not really useful :P)
  md23.disableSpeedRegulation();
  lcd.clearDisplay(); // this always need 30 millis delay.
  delay(30);
  lcd.write("Chiappe!");
  delay(10);
  lcd.moveLine2(0);
  delay(10);
  lcd.write("Battery:"); // every other command needs 10 millis delay
  delay(10);

  byte b = md23.getBatteryVolts();
  lcd.write(b, DEC);
  delay(10);
  lcd.moveLine2(0);
  delay(10);


  Serial.begin(9600);
  for (int i = 0; i < NUMREADINGS; i++)
    readings[i] = 0; // initialize all the readings to 0 (gyro average filter)

  startmillis = millis();

}

float sqr(float a)
{
  return a*a;
}

float sgn (float a)
{
  if (a > 0)
    return +1;
  else if (a < 0)
    return -1;
  else
    return 0;
}

long readMAX127(byte chan)
{
  byte control = 0x80 + (chan << 4);
  byte addr = 0x28;

  Wire.beginTransmission(addr);
  Wire.send(control); // send value
  Wire.endTransmission();

  Wire.requestFrom(addr, (byte)2);

  byte b1 = Wire.receive();
  byte b2 = Wire.receive() >> 4;
  long data = (b1 << 4) | (b2 & 0x0F);

  return data;
}


void loop()
{
  int delta = millis()-lastread;
  if( delta >= mydt) { // sample every dt ms -> 1000/dt hz.
    lastread = millis();

    total -= readings[index];               // subtract the last gyro reading
    readings[index] = analogRead(2);        // read from the gyro sensor
    total += readings[index];               // add the reading to the total
    index = (index + 1);                    // advance to the next index

    if (index >= NUMREADINGS)               // if we're at the end of the array...
      index = 0;                            // ...wrap around to the beginning

    average = (total / NUMREADINGS)-500;    // calculate the average of gyro input

    dt = ((float)delta) / 1000.0;

    q_m= ((float)average)*(1500.0/1024.0)*PI/180 ;  // HAC remove 1.5 mult

    /* Unbias our gyro */
    const float q = q_m - q_bias; //(Kalman)

    const float Pdot[2 * 2] = {
      Q_angle - PP[0][1] - PP[1][0], /* 0,0 */ //(Kalman)
      - PP[1][1], /* 0,1 */
      - PP[1][1], /* 1,0 */
      Q_gyro /* 1,1 */
    };

    /* Store our unbias gyro estimate */
    rate = q; //(Kalman)

    /*
     * Update our angle estimate
     * angle += angle_dot * dt
     *       += (gyro - gyro_bias) * dt
     *       += q * dt
     */
    angle += q * dt; //(Kalman)

    /* Update the covariance matrix */
    PP[0][0] += Pdot[0] * dt; //(Kalman)
    PP[0][1] += Pdot[1] * dt; //(Kalman)
    PP[1][0] += Pdot[2] * dt; //(Kalman)
    PP[1][1] += Pdot[3] * dt; //(Kalman)


    // read here!
    ax_m = analogRead(0)-510;
    ay_m = analogRead(1)-510;

    // non aggiusto R
    // R_angle= (joy_y_axis+1)*0.0098039; //external adjust jitter of accelerometer with nunchuck joystick

    const float angle_m = atan2( ay_m, ax_m ); //(Kalman)
    const float angle_err = angle_m - angle; //(Kalman)
    const float C_0 = 1; //(Kalman)
    const float PCt_0 = C_0 * PP[0][0];  //(Kalman)
    const float PCt_1 = C_0 * PP[1][0]; //(Kalman)
    const float E =R_angle+ C_0 * PCt_0; //(Kalman)
    const float K_0 = PCt_0 / E; //(Kalman)
    const float K_1 = PCt_1 / E; //(Kalman)
    const float t_0 = PCt_0; /* C_0 * P[0][0] + C_1 * P[1][0] (Kalman) */

    const float t_1 = C_0 * PP[0][1]; /* + C_1 * P[1][1]  = 0 (Kalman) */


    PP[0][0] -= K_0 * t_0; //(Kalman)
    PP[0][1] -= K_0 * t_1; //(Kalman)
    PP[1][0] -= K_1 * t_0; //(Kalman)
    PP[1][1] -= K_1 * t_1; //(Kalman)
    angle += K_0 * angle_err; //(Kalman)
    q_bias += K_1 * angle_err; //(Kalman)

    float calibration = -4.5; // read from a pot or something
    float myangle=(angle*57.2957795130823)-90.0 + calibration;

    if ((millis()-startmillis) > 6000 )
    {
      digitalWrite(ledPin, HIGH);

      //    float integralAverage = (integral / NUMINTEGRALS);
      //    integral = integral - integralAverage;

      int val = digitalRead(2);  // read input value
      if (val == HIGH) {
        I = 0.0;
      }


      float rate_deg = rate * 57.2957795130823;
      float pot1 = ((float)readMAX127(1));
      float pot2 = ((float)readMAX127(2));
      float pot3 = ((float)readMAX127(3));

      // myangle is up at 0
      // motors is stopped at 128

      P = (myangle * (pot1 / 100.0)); // pot/10
      D = (myangle-oldAngle) * (pot2 / 10.0); // Kd = 50 (800 at pot)
      I = I + myangle * (pot3/100.0); // I * 1.0 + myangle * pot;  // 123 looks good

      float pid = P+I+D;
      float motors = 128.0 - (pid+(sgn(pid)*sqr(pid/18)));
      // cap the value
      if (motors>250.0) motors = 250.0;
      if (motors<5.0) motors = 5.0;

      byte motorsInput = (byte)motors;
      //Serial.println(myangle, DEC);
      Serial.println(motorsInput, DEC);
      if ((myangle > 80) || (myangle < -80))
      {
        md23.setMotor1Speed(128);
        md23.setMotor2Speed(128);
        I=0; // avoid accumulation of integration when stopped
      }
      else
      {
        md23.setMotor1Speed(motorsInput);
        md23.setMotor2Speed(motorsInput);
      }


      oldAngle = myangle;



      // update the display
      displayState ++;
      if (displayState==7) displayState = 0;
      switch (displayState)
      {
        case 0:
          lcd.moveLine2(0);
          break;
        case 1:
          lcd.write(pot1, DEC);
          break;
        case 2:
          lcd.write(" ");
          break;
        case 3:
          lcd.write(pot2, DEC);
          break;
        case 4:
          lcd.write(" ");
          break;
        case 5:
          lcd.write(pot3, DEC);
          break;
        case 6:
          lcd.write("    ");
          break;
      }

    }
    else
    {
      digitalWrite(ledPin, LOW);
    }

    //    Serial.print(" ");

    //    Serial.println(int(angle_m*57.295779513082)+180, DEC); //Prints the accelometer
    //    Serial.println(ay_m, DEC); //Prints the accelometer

    //    Serial.print(" ");
    //Prints degrees Acceleromter+Gyros+KalmanFilters
    //    Serial.print(" ");
  }
}
