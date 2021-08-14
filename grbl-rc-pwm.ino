#include <Servo.h>

Servo spindle;
const byte interruptPin = 2;
const byte spindlePwmPin = 3;
int spindleOut = 0;

// Initialize variables that will hold the timstamps of the rising and falling edges of the input PWM signal.
// Since those are called from within the ISRs they should be volatile
volatile long lastHigh = 0;
volatile long lastLow = 0;
volatile long highLength = 0;
volatile long lowLength = 0;
volatile int saturation = 0;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gohigh, RISING);
  spindle.attach(spindlePwmPin);
}

void loop() {
  spindle.write(saturation);
  delay(10);
}

// ISR functions are used to read the PWM signal more accurately. Those work reliably up to about 4KHz
void golow() {
  attachInterrupt(digitalPinToInterrupt(interruptPin), gohigh, RISING);
  highLength = micros() - lastHigh;
  lastLow = micros();
  saturation = (180 * highLength) / (lowLength + highLength);
}

void gohigh() {
  attachInterrupt(digitalPinToInterrupt(interruptPin), golow, FALLING);
  lowLength = micros() - lastLow;
  lastHigh = micros();
}
