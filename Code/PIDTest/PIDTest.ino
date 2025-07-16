#include <QTRSensors.h>

// === QTR Sensor Setup ===
const uint8_t NUM_SENSORS = 8;
QTRSensors qtr;
uint16_t sensorValues[NUM_SENSORS];

// === PID Constants ===
float Kp = 0.2;
float Ki = 0.0;
float Kd = 4.0;

int lastError = 0;
float integral = 0;

// === Speed Settings (for simulation only) ===
const int baseSpeed = 120;
const int maxSpeed  = 255;

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("QTR-8A Sensor Debug Test (No Motor Output)");

  // QTR sensor setup (analog mode)
  qtr.setTypeAnalog();

  // Update with your actual analog pins used for QTR-8A
  qtr.setSensorPins((const uint8_t[]) {
    A0, A1, A2, A3, A4, A5, A6, A7
  }, NUM_SENSORS);
}

void loop() {
  // Read line position (0 to 7000 for 8 sensors)
  int position = qtr.readLineBlack(sensorValues);
  int error = position - 3500; // 3500 is center for 8 sensors

  // PID computation
  integral += error;
  int derivative = error - lastError;
  float correction = Kp * error + Ki * integral + Kd * derivative;

  // Simulated motor outputs
  int rightMotor = baseSpeed - correction;
  int leftMotor  = baseSpeed + correction;

  rightMotor = constrain(rightMotor, -maxSpeed, maxSpeed);
  leftMotor  = constrain(leftMotor, -maxSpeed, maxSpeed);

  // === Debug Print ===
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
  delay(200); // Adjust as needed
}
