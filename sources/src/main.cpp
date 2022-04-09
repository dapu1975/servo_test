#include <Arduino.h>
#include <Servo.h>

#define beeper 3
#define servo 5

Servo myservo;

void setup()
{
  // beeper off
  pinMode(beeper, OUTPUT);
  digitalWrite(beeper, HIGH);
  myservo.attach(servo);
}

void loop()
{
  // put your main code here, to run repeatedly:
}