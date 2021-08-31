#include <Arduino.h>
#include <Servo.h>

Servo spindle;

const byte interruptPin = 2;
const byte spindlePwmPin = 4;

// Default values are 1000-2000us
// but you can adjust it personally
const unsigned int minDuty = 900;
const unsigned int maxDuty = 2000;
const unsigned int period = 20000;

// Initialize variables that will hold the timstamps
// of the rising and falling edges of the input PWM signal.
// Since those are called from within the ISRs they should be volatile
volatile long lastHigh = 0;
volatile long lastLow = 0;
volatile long highLength = 0;
volatile long lowLength = 0;

volatile float saturation = 0;

unsigned long currentMicros = micros();

void setup() {
  pinMode(interruptPin, INPUT);
  spindle.attach(spindlePwmPin);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gohigh, RISING);
}

void loop() {

  // Edge conditions
  currentMicros = micros();
  if (currentMicros - lastHigh > period) {
    if (digitalRead(interruptPin) == HIGH) {
      saturation = 1;
    } else {
      saturation = 0;
    }
  }

  spindle.writeMicroseconds(minDuty + int(saturation * (maxDuty - minDuty)));
  delay(2); // ms
}

// ISR functions are used to read the PWM signal more accurately.
// Those work reliably up to about 4KHz
void golow() {
  attachInterrupt(digitalPinToInterrupt(interruptPin), gohigh, RISING);
  highLength = micros() - lastHigh;
  lastLow = micros();

  saturation = float(highLength) / float(lowLength + highLength);
}

void gohigh() {
  attachInterrupt(digitalPinToInterrupt(interruptPin), golow, FALLING);
  lowLength = micros() - lastLow;
  lastHigh = micros();
}
