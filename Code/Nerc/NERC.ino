#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t NUM_SENSORS = 8;
QTRSensors qtr;
uint16_t sensorValues[NUM_SENSORS];

// === Motor Pins ===
const int RmPWM = 7;
const int RmF = 6;
const int RmB = 5;
const int LmPWM = 2;
const int LmF = 3;
const int LmB = 4;

// === PID Constants ===
float Kp = 0.2;
float Ki = 0.0;
float Kd = 4.0;
int lastError = 0;
float integral = 0;

// === Speed Settings ===
const int baseSpeed = 120;
const int maxSpeed = 255;

// === Junction Vars ===
bool junc = false;
int juncCount = 0;
int threshold = 600; // Adjust this as needed
int Track = 1;

void setup() {
  Serial.begin(9600);

  // Motor setup
  pinMode(RmPWM, OUTPUT);
  pinMode(RmF, OUTPUT);
  pinMode(RmB, OUTPUT);
  pinMode(LmPWM, OUTPUT);
  pinMode(LmF, OUTPUT);
  pinMode(LmB, OUTPUT);

  // QTR setup: 8 analog pins
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    A0, A1, A2, A3, A4, A5, A6, A7
  }, NUM_SENSORS);
}

void loop() {
  int position = qtr.readLineBlack(sensorValues);
  int blackCount = BlackCount();

  // Detect a junction (tune threshold for 8 sensors)
  while (blackCount >= 5) {
    position = qtr.readLineBlack(sensorValues);
    blackCount = BlackCount();
    driveMotors(255, 255);
    junc = true;
  }

  if (junc) {
    juncCount++;
    Serial.print("Junction Detected! Count: ");
    Serial.println(juncCount);
    junc = false;
  }

  // Turn decisions
  if (juncCount == 4 || juncCount == 7 || juncCount == 16 || juncCount == 17) {
    if (Track == 1) {
      LeftTurn();
    } else {
      RightTurn();
    }
  }

  if (juncCount == 11 || juncCount == 12 || juncCount == 21 || juncCount == 22) {
    if (Track == 1) {
      RightTurn();
    } else {
      LeftTurn();
    }
  }

  // Normal line following
  linefollow(position);
}

void driveMotors(int rightPWM, int leftPWM) {
  digitalWrite(RmF, rightPWM > 0);
  digitalWrite(RmB, rightPWM < 0);
  digitalWrite(LmF, leftPWM > 0);
  digitalWrite(LmB, leftPWM < 0);

  analogWrite(RmPWM, abs(rightPWM));
  analogWrite(LmPWM, abs(leftPWM));
}

void linefollow(int position) {
  int error = position - 3500; // 8 sensors × 1000/2 = 3500 center
  integral += error;
  int derivative = error - lastError;
  float correction = Kp * error + Ki * integral + Kd * derivative;

  int rightSpeed = baseSpeed - correction;
  int leftSpeed = baseSpeed + correction;

  rightSpeed = constrain(rightSpeed, -maxSpeed, maxSpeed);
  leftSpeed = constrain(leftSpeed, -maxSpeed, maxSpeed);

  driveMotors(rightSpeed, leftSpeed);
  lastError = error;
}

int BlackCount() {
  int count = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    if (sensorValues[i] > threshold) count++;
  }
  return count;
}

// Generic right turn function (name is kept "RightTurn" for clarity)
void RightTurn() {
  Serial.println(">> Turning right...");
  while (true) {
    qtr.read(sensorValues);
    bool rightOff = sensorValues[6] < threshold && sensorValues[7] < threshold;
    bool midLeftOn = sensorValues[3] > threshold || sensorValues[2] > threshold;
    if (rightOff && midLeftOn) break;
    driveMotors(100, -100);
    delay(10);
  }
  driveMotors(0, 0);
  Serial.println(">> Right turn complete.");
}

// Generic left turn function
void LeftTurn() {
  Serial.println(">> Turning left...");
  while (true) {
    qtr.read(sensorValues);
    bool leftOff = sensorValues[0] < threshold && sensorValues[1] < threshold;
    bool midRightOn = sensorValues[4] > threshold || sensorValues[5] > threshold;
    if (leftOff && midRightOn) break;
    driveMotors(-100, 100);
    delay(10);
  }
  driveMotors(0, 0);
  Serial.println(">> Left turn complete.");
}
