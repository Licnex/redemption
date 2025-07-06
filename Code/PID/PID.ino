#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t numSensors = 8;
const uint8_t emitterControlPin = A8; // Connect to LEDON or set to QTR_NO_EMITTER_PIN
QTRSensorsAnalog qtr(
  (uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7},
  numSensors,
  2500,
  emitterControlPin
);
unsigned int sensorValues[8];

// === Motor Driver Pins ===
const int motorLeftIn1 = 8;
const int motorLeftIn2 = 9;
const int motorLeftPWM = 10;

const int motorRightIn1 = 6;
const int motorRightIn2 = 7;
const int motorRightPWM = 5;

// === PID Constants ===
float Kp = 0.2;
float Ki = 0.0;
float Kd = 5.0;

int baseSpeed = 120;
int maxMotorSpeed = 255;

int lastError = 0;
int integral = 0;

void setup() {
  pinMode(motorLeftIn1, OUTPUT);
  pinMode(motorLeftIn2, OUTPUT);
  pinMode(motorLeftPWM, OUTPUT);

  pinMode(motorRightIn1, OUTPUT);
  pinMode(motorRightIn2, OUTPUT);
  pinMode(motorRightPWM, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Read line position without calibration
  int position = qtr.readLine(sensorValues, QTR_EMITTERS_ON, false); // no calibration
  int error = position - 3500; // center for 8 sensors = 7 * 1000 / 2 = 3500

  integral += error;
  int derivative = error - lastError;
  int correction = Kp * error + Ki * integral + Kd * derivative;
  lastError = error;

  int leftSpeed = baseSpeed + correction;
  int rightSpeed = baseSpeed - correction;

  leftSpeed = constrain(leftSpeed, 0, maxMotorSpeed);
  rightSpeed = constrain(rightSpeed, 0, maxMotorSpeed);

  setMotorSpeeds(leftSpeed, rightSpeed);
}

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
  if (leftSpeed >= 0) {
    digitalWrite(motorLeftIn1, HIGH);
    digitalWrite(motorLeftIn2, LOW);
  } else {
    digitalWrite(motorLeftIn1, LOW);
    digitalWrite(motorLeftIn2, HIGH);
    leftSpeed = -leftSpeed;
  }
  analogWrite(motorLeftPWM, constrain(leftSpeed, 0, 255));

  if (rightSpeed >= 0) {
    digitalWrite(motorRightIn1, HIGH);
    digitalWrite(motorRightIn2, LOW);
  } else {
    digitalWrite(motorRightIn1, LOW);
    digitalWrite(motorRightIn2, HIGH);
    rightSpeed = -rightSpeed;
  }
  analogWrite(motorRightPWM, constrain(rightSpeed, 0, 255));
}
