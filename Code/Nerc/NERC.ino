#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t NUM_SENSORS = 13;
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

  // QTR setup: using 13 analog pins in custom order
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    A3, A2, A1, A0, A17, A16, A15, A14, A13, A12, A11, A10, A9
  }, NUM_SENSORS);
}

void loop() {
  int position = qtr.readLineBlack(sensorValues);
  int blackCount = BlackCount();

  // Detect a junction (5+ black sensors)
  while (blackCount >= 8) {
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
    if (Track == 1){ 
    Left90();
    }
    else {
    Right90();
    }
  }

  if (juncCount == 11 || juncCount == 12 || juncCount == 21 || juncCount == 22) {
    if (Track == 1){
    Right90();
    }
    else {
    Left90();
    }
  }

  // Line following
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
  int error = position - 6500; // 13 sensors × 1000/2 = 6500 center
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

void Right90() {
  Serial.println(">> Turning right 90°...");
  while (true) {
    qtr.read(sensorValues);
    bool rightOff = sensorValues[10] < threshold && sensorValues[11] < threshold && sensorValues[12] < threshold;
    bool midLeftOn = sensorValues[5] > threshold || sensorValues[6] > threshold;
    if (rightOff && midLeftOn) break;
    driveMotors(100, -100);
    delay(10);
  }
  driveMotors(0, 0);
  Serial.println(">> Right 90 complete.");
} // this works for any degree turn not just 90, i just named it that cause i will be using it in 90 degree turns

void Left90() {
  Serial.println(">> Turning left 90°...");
  while (true) {
    qtr.read(sensorValues);
    bool leftOff = sensorValues[0] < threshold && sensorValues[1] < threshold && sensorValues[2] < threshold;
    bool midRightOn = sensorValues[6] > threshold || sensorValues[7] > threshold;
    if (leftOff && midRightOn) break;
    driveMotors(-100, 100);
    delay(10);
  }
  driveMotors(0, 0);
  Serial.println(">> Left 90 complete.");
}
// this works for any degree turn not just 90 i just named it that cause i will be using it in 90 degree turns