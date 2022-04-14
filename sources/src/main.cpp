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

void SendDataToSegment(byte Segment_no, byte Segments);

const byte LED_a = 0x01;
const byte LED_b = 0x02;
const byte LED_c = 0x04;
const byte LED_d = 0x08;
const byte LED_e = 0x10;
const byte LED_f = 0x20;
const byte LED_g = 0x40;
const byte LED_dp = 0x80;

byte select_seg1 = 0xF1;
byte select_seg2 = 0xF2;
byte select_seg3 = 0xF4;
byte select_seg4 = 0xF8;

byte nums[] = {
    LED_a + LED_b + LED_c + LED_d + LED_e + LED_f,
    LED_b + LED_c,
    LED_a + LED_b + LED_g + LED_e + LED_d,
    LED_a + LED_b + LED_g + LED_c + LED_d,
    LED_f + LED_g + LED_b + LED_c,
    LED_a + LED_f + LED_g + LED_c + LED_d,
    LED_a + LED_c + LED_d + LED_e + LED_f + LED_g,
    LED_a + LED_b + LED_c,
    LED_a + LED_b + LED_c + LED_d + LED_e + LED_f + LED_g,
    LED_a + LED_b + LED_c + LED_d + LED_f + LED_g};

const byte SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

unsigned long ms, old;

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
  ms = millis();
  if (ms >= (old + 25))
  {
    old = ms;
    if ((digitalRead(LBUTTON) == LOW) & (pos > 0))
    {
      pos--;
    }
    if (digitalRead(MBUTTON) == LOW)
    {
      pos = 90;
    }
    if ((digitalRead(RBUTTON) == LOW) & (pos < 180))
    {
      pos++;
    }
    myservo.write(pos);
  }
  byte dummy = pos;
  byte value = 0;
  SendDataToSegment(select_seg1, 0x00);
  value = dummy % 100;
  dummy = value * 100;
  SendDataToSegment(select_seg2, nums[value]);
  value = dummy % 10;
  dummy = value * 10;
  SendDataToSegment(select_seg3, nums[value]);
  value = dummy;
  SendDataToSegment(select_seg4, nums[value]);
}

void SendDataToSegment(byte Segment_no, byte Segments)
{
  /* Make Latch pin Low */
  digitalWrite(LATCH_PIN, LOW);
  /* Transfer Segmenent data */
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, 0xFF - Segments);
  /* Transfer Segmenent Number  */
  shiftOut(DATA_PIN, CLK_PIN, MSBFIRST, Segment_no);
  /* Make Latch pin High so the data appear on Latch parallel pins */
  digitalWrite(LATCH_PIN, HIGH);
}