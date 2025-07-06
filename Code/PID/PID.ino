#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t NUM_SENSORS = 8;
QTRSensorsAnalog qtr((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, NUM_SENSORS);

uint16_t sensorValues[NUM_SENSORS];

// === Motor Pins ===
const int RmPWM = 7;
const int RmF   = 6;
const int RmB   = 5;
const int LmPWM = 2;
const int LmF   = 3;
const int LmB   = 4;

// === PID Constants ===
float Kp = 0.2;
float Ki = 0.0;
float Kd = 4.0;

int lastError = 0;
float integral = 0;

// === Speed Settings ===
const int baseSpeed = 120;
const int maxSpeed  = 255;

void setup() {
  Serial.begin(9600);

  // Motor setup
  pinMode(RmPWM, OUTPUT);
  pinMode(RmF, OUTPUT);
  pinMode(RmB, OUTPUT);
  pinMode(LmPWM, OUTPUT);
  pinMode(LmF, OUTPUT);
  pinMode(LmB, OUTPUT);

  // Sensor emitters on (only required if you tied the LEDON pin to a GPIO)
  // qtr.emittersOn();
}

void loop() {
  int position = qtr.readLineBlack(sensorValues); // returns 0–7000
  int error = position - 3500;

  integral += error;
  int derivative = error - lastError;
  float correction = Kp * error + Ki * integral + Kd * derivative;

  int rightSpeed = baseSpeed - correction;
  int leftSpeed  = baseSpeed + correction;

  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);
  leftSpeed  = constrain(leftSpeed, -maxSpeed, maxSpeed);

  driveMotors(rightSpeed, leftSpeed);
  lastError = error;

  delay(10);
}

void driveMotors(int rightPWM, int leftPWM) {
  // Right motor
  if (rightPWM > 0) {
    digitalWrite(RmF, HIGH);
    digitalWrite(RmB, LOW);
  } else if (rightPWM < 0) {
    digitalWrite(RmF, LOW);
    digitalWrite(RmB, HIGH);
  } else {
    digitalWrite(RmF, LOW);
    digitalWrite(RmB, LOW);
  }

  // Left motor
  if (leftPWM > 0) {
    digitalWrite(LmF, HIGH);
    digitalWrite(LmB, LOW);
  } else if (leftPWM < 0) {
    digitalWrite(LmF, LOW);
    digitalWrite(LmB, HIGH);
  } else {
    digitalWrite(LmF, LOW);
    digitalWrite(LmB, LOW);
  }

  analogWrite(RmPWM, abs(rightPWM));
  analogWrite(LmPWM, abs(leftPWM));
}
