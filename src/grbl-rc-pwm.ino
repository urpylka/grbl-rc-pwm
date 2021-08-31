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

boolean flag = false;
unsigned long currentMicros = micros();
unsigned long previousMillis = 0;

void debug() {
  unsigned long currentMillis = millis();
  // Interval > 2sec
  if (currentMillis - previousMillis >= 2000) {
    previousMillis = currentMillis;

    if (flag) Serial.println("in edge");

    Serial.print("currentMicros: ");
    Serial.println(currentMicros);
    Serial.print("lastHigh:  ");
    Serial.println(lastHigh);

    Serial.print("saturation: ");
    Serial.println(saturation);
    Serial.print("ms: ");
    Serial.println(minDuty + int(saturation * (maxDuty - minDuty)));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin, INPUT);
  spindle.attach(spindlePwmPin);
  attachInterrupt(digitalPinToInterrupt(interruptPin), gohigh, RISING);
}

void loop() {
  // Edge conditions
  currentMicros = micros();
  flag = false;
  if (currentMicros - lastHigh > period) {
    if (digitalRead(interruptPin) == HIGH) {
      saturation = 1;
    } else {
      saturation = 0;
    }
    flag = true;
  }

  debug();

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
