#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>

// Definitions
#define TRIG 13
#define ECHO 2
#define CLOSE 7
#define FAR 8

float distance;
int ledState = LOW;
const long interval = 1000;
unsigned long previousMillis = 0;
volatile int echoCloseFlag;
volatile int echoClosePulseFlag;
volatile int echoFarFlag;
volatile int echoFarPulseFlag;

// Interrupt to Determine LED activation
void ISR_dist()
{
  if ((distance < 5) & (distance >= 4))
  {
    echoCloseFlag = 1;
  }
  else if (distance > 7)
  {
    echoFarFlag = 1;
  }
  else if (distance < 4)
  {
    echoClosePulseFlag = 1;
  }
  else if ((distance >= 5) & (distance <= 7))
  {
    echoFarPulseFlag = 1;
  }
}

void setup()
{
  // Setting Pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(CLOSE, OUTPUT);
  pinMode(FAR, OUTPUT);

  // Interrupt pin tied to Echo
  attachInterrupt(digitalPinToInterrupt(ECHO), ISR_dist, HIGH);

  // Serial communication
  Serial.begin(9600);
}

void loop()
{
  // Turn on TRIG
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Calculate distance from object
  distance = pulseIn(ECHO, HIGH) * 0.034 / 2;

  // LED Interrupt
  if (echoCloseFlag)
  {
    echoClosePulseFlag = 0;
    digitalWrite(CLOSE, HIGH);
    digitalWrite(FAR, LOW);
    echoCloseFlag = 0;
  }
  else if (echoFarFlag)
  {
    digitalWrite(FAR, HIGH);
    digitalWrite(CLOSE, LOW);
    echoFarFlag = 0;
  }
  else if (echoClosePulseFlag)
  {
    echoCloseFlag = 0;
    digitalWrite(FAR, LOW);
    ledBlink();
    digitalWrite(CLOSE, ledState);
  }
  else if (echoFarPulseFlag)
  {
    echoCloseFlag = 0;
    echoFarFlag = 0;
    digitalWrite(CLOSE, LOW);
    ledBlink();
    digitalWrite(FAR, ledState);
  }

  // Print distance via SERIAL
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print("\n");
}

/* LED blink without delay
Using delay alongside interrupts is ineffective, due to the constraint of the delay interfereing with the interrupt. For instance, when triggering the echoFarPulseFlag, if the object is instantly moved close, the new interrupt does not trigger due to the delay
freezing the program. As soon as the delay ends, then the new interrupt occurs. Instead of using delay, the blink function will be made using time analysis.
*/

void ledBlink()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }
  }
}