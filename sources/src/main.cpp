//*****************************************************************************
// project: servo tester
// author: Daniel Pust
// create: 2022-04-14
// working: 2022-04-18
// version: 1.0
//*****************************************************************************
#include <Arduino.h>
#include <Servo.h>

#define BEEPER 3
#define SERVO 5

#define LBUTTON 15
#define MBUTTON 16
#define RBUTTON 17

#define DATA_PIN 8
#define CLK_PIN 7
#define LATCH_PIN 4

#define MODE_POS 0x00
#define MODE_TIME 0x01

#define LED_a 0x01
#define LED_b 0x02
#define LED_c 0x04
#define LED_d 0x08
#define LED_e 0x10
#define LED_f 0x20
#define LED_g 0x40
#define LED_dp 0x80

#define select_seg1 0xF1
#define select_seg2 0xF2
#define select_seg3 0xF4
#define select_seg4 0xF8

#define P_MIN 0
#define P_MIDDLE 90
#define P_MAX 180

#define T_MIN 0
#define T_MIDDLE 1250
#define T_MAX 2500

Servo myservo;

void SendNumberToDisplay(unsigned int value);
void SendDataToSegment(byte Segment_no, byte Segments);

unsigned int mb_time = 0;   // counts in 25 ms steps duration of pressing
boolean mb_oldstate = HIGH; // button state in last loop
boolean mb_lock;            // button deactive -> wait for release

byte mode = MODE_POS;
unsigned int min = P_MIN;
unsigned int middle = P_MIDDLE;
unsigned int max = P_MAX;
unsigned int pos = P_MIDDLE;

unsigned long ms, old;

byte nums_seg[] = {
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

void setup()
{
  pinMode(BEEPER, OUTPUT);
  digitalWrite(BEEPER, HIGH);

  myservo.attach(SERVO);

  pinMode(LBUTTON, INPUT_PULLUP);
  pinMode(MBUTTON, INPUT_PULLUP);
  pinMode(RBUTTON, INPUT_PULLUP);

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

    if (digitalRead(LBUTTON) == LOW)
    {
      if ((mode == MODE_POS) && (pos > min))
        pos--;
      if ((mode == MODE_TIME) && (pos >= min + 5))
        pos -= 5;
    }

    if (digitalRead(RBUTTON) == LOW)
    {
      if ((mode == MODE_POS) && (pos < max))
        pos++;
      if ((mode == MODE_TIME) && (pos <= max - 5))
        pos += 5;
    }

    if (!mb_lock && (digitalRead(MBUTTON) == LOW))
    {
      mb_time++;
      // long press > 1000 ms (20*25)
      if (mb_time > 20)
      {
        // switch mode
        if (mode == MODE_POS)
        {
          mode = MODE_TIME;
          min = T_MIN;
          middle = T_MIDDLE;
          max = T_MAX;
        }
        else
        {
          mode = MODE_POS;
          min = P_MIN;
          middle = P_MIDDLE;
          max = P_MAX;
        }
        pos = middle;
        mb_lock = true;
        // confirmation beep
        digitalWrite(BEEPER, LOW);
        delay(25);
        digitalWrite(BEEPER, HIGH);
      }
    }
    boolean state = digitalRead(MBUTTON);
    if ((state == HIGH) && (mb_oldstate == LOW))
    {
      mb_time = 0;
      if (mb_lock == true)
        mb_lock = false;
      else
      {
        if ((pos != min) && (pos != middle) && (pos != max))
          pos = middle;
        else if (pos == min)
          pos = middle;
        else if (pos == middle)
          pos = max;
        else if (pos == max)
          pos = min;
      }
    }
    mb_oldstate = state;

    if (mode == MODE_POS)
      myservo.write(pos);
    else if (mode == MODE_TIME)
      myservo.writeMicroseconds(pos);

  } // this block runs every 25ms
  SendNumberToDisplay(pos);
}

void SendNumberToDisplay(unsigned int value)
{
  byte add_val = 0x00;
  if (mode == MODE_POS)
  {
    add_val = LED_dp;
  }
  else
  {
    add_val = 0x00;
  }
  SendDataToSegment(select_seg1, nums_seg[value / 1000] + add_val);
  value -= (value / 1000) * 1000;
  SendDataToSegment(select_seg2, nums_seg[value / 100]);
  value -= (value / 100) * 100;
  SendDataToSegment(select_seg3, nums_seg[value / 10]);
  value -= (value / 10) * 10;
  if (mode == MODE_TIME)
  {
    add_val = LED_dp;
  }
  else
  {
    add_val = 0x00;
  }
  SendDataToSegment(select_seg4, nums_seg[value] + add_val);
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