#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t NUM_SENSORS = 8;
QTRSensorsAnalog qtr((const uint8_t[]){A0, A1, A2, A3, A4, A5, A6, A7}, NUM_SENSORS);

uint16_t sensorValues[NUM_SENSORS];

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
  delay(500);
  Serial.println("QTR Sensor Debug Test (No Motor Output)");
}

void loop() {
  // Read position (0 to 7000)
  int position = qtr.readLineBlack(sensorValues);
  int error = position - 3500;

  // PID computation
  integral += error;
  int derivative = error - lastError;
  float correction = Kp * error + Ki * integral + Kd * derivative;

  // Calculate motor speeds
  int rightMotor = baseSpeed - correction;
  int leftMotor  = baseSpeed + correction;

  // Clamp to max range
  rightMotor = constrain(rightMotor, -maxSpeed, maxSpeed);
  leftMotor  = constrain(leftMotor, -maxSpeed, maxSpeed);

  // === Print Output ===
  Serial.print("Raw: ");
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    Serial.print(sensorValues[i]);
    Serial.print("\t");
  }

  Serial.print("| Pos: ");
  Serial.print(position);
  Serial.print(" | Err: ");
  Serial.print(error);
  Serial.print(" | L: ");
  Serial.print(leftMotor);
  Serial.print(" | R: ");
  Serial.println(rightMotor);

  lastError = error;
  delay(200); // readable output
}
