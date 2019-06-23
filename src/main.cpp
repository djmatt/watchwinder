/* 
This is a test sketch for the Adafruit assembled Motor Shield for Arduino v2
It won't work with v1.x motor shields! Only for the v2's with built in PWM
control

For use with the Adafruit Motor Shield v2 
---->	http://www.adafruit.com/products/1438
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <ESP8266WiFi.h>
#include "WiFiUdp.h"
#include <NTPClient.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Connect a stepper motor with 513 steps per revolution
// to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(513, 2);

//An NTP client, requires Wifi had been connected previously
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//Calculation of time delay
//Gap is in minutes
#define GAP (15)
unsigned long minutes, seconds, t = 0;

void setup()
{
  //Start serial connection
  Serial.begin(9600);
  Serial.println("Starting program");

  //Setup stepper controls
  AFMS.begin();          // create with the default frequency 1.6KHz
  myMotor->setSpeed(25); // revs per minutes

  Serial.println("Motor config'd");

  //Wait for wifi to connect
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected");

  //Connect to the NTP server
  timeClient.begin();
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
}

void loop()
{
  //run the winder operation for 1 minutes (roughly)
  for (int i = 0; i < 25; i++)
  {
    myMotor->step(513, FORWARD, DOUBLE);
  }
  myMotor->release();
  Serial.println("Finished rotations");

  //Delay until the next GAP minutes mark
  minutes = GAP - timeClient.getMinutes() % GAP - 1;
  seconds = 60 - timeClient.getSeconds();
  t = (minutes * 60) + seconds;
  t = (t == 0) ? GAP * 60 : t;

  Serial.printf("Delaying %lu minutes - %lu seconds\n", t / 60, t % 60);
  delay(t * 1000UL);
  timeClient.update();
  Serial.println(timeClient.getFormattedTime());
}