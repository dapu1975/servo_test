//*****************************************************************************
// project: servo tester
// author: Daniel Pust
// create: 2022-04-14
//*****************************************************************************
#include <Arduino.h>
#include <Servo.h>

#define BEEPER 3
#define SERVO 5

Servo myservo;
int pos = 90;

#define LBUTTON 15
#define MBUTTON 16
#define RBUTTON 17

#define DATA_PIN 8
#define CLK_PIN 7
#define LATCH_PIN 4

void SendDataToSegment(byte Segment_no, byte hexValue);

const byte C_HEX = 0xC6;
const byte TWO_HEX = 0xA4;
const byte P_HEX = 0x8C;

const char LED_a = 0xFE;
const byte LED_b = 0xFD;
const byte LED_c = 0xFB;
const byte LED_d = 0xF7;
const byte LED_e = 0xEF;
const byte LED_f = 0xDF;
const byte LED_g = 0xBF;
const char LED_dp = 0x7F;

byte select_seg1 = 0xF1;
byte select_seg2 = 0xF2;
byte select_seg3 = 0xF4;
byte select_seg4 = 0xF8;

const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void setup()
{
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, HIGH);

  myservo.attach(SERVO);

  pinMode(LBUTTON, INPUT);
  pinMode(MBUTTON, INPUT);
  pinMode(RBUTTON, INPUT);

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
}

void loop()
{
  if ((digitalRead(LBUTTON) == LOW) & (pos > 0))
  {
    pos--;
  }
  if ((digitalRead(RBUTTON) == LOW) & (pos < 255))
  {
    pos++;
  }
  myservo.write(pos);
  // delay(25);
  SendDataToSegment(select_seg1, LED_e);
  SendDataToSegment(select_seg2, LED_f);
  SendDataToSegment(select_seg3, LED_g);
  SendDataToSegment(select_seg4, LED_dp);
}

void SendDataToSegment(byte Segment_no, byte hexValue)
{
  /* Make Latch pin Low */
  digitalWrite(LATCH_PIN, LOW);
  /* Transfer Segmenent data */
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, hexValue);
  /* Transfer Segmenent Number  */
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, Segment_no);
  /* Make Latch pin High so the data appear on Latch parallel pins */
  digitalWrite(LATCH_PIN, HIGH);
}