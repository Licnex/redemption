// === Motor Pins (adjust these) ===
const int RmPWM = 7;
const int RmF = 6;
const int RmB = 5;
const int LmPWM = 2;
const int LmF = 3;
const int LmB = 4;

void setup() {
  pinMode(RmPWM, OUTPUT);
  pinMode(RmF, OUTPUT);
  pinMode(RmB, OUTPUT);
  pinMode(LmPWM, OUTPUT);
  pinMode(LmF, OUTPUT);
  pinMode(LmB, OUTPUT);
}

void setMotor(int Lspeed, int Rspeed) {
  // Left motor
  digitalWrite(LmF, Lspeed >= 0);
  digitalWrite(LmB, Lspeed < 0);
  analogWrite(LmPWM, abs(Lspeed));

  // Right motor
  digitalWrite(RmF, Rspeed >= 0);
  digitalWrite(RmB, Rspeed < 0);
  analogWrite(RmPWM, abs(Rspeed));
}

void stopMotors() {
  analogWrite(LmPWM, 0);
  analogWrite(RmPWM, 0);
}

void spinInPlace(int speed, int duration) {
  setMotor(speed, -speed);
  delay(duration);
  stopMotors();
  delay(200);
}

void wiggle(int speed, int times, int delayTime) {
  for (int i = 0; i < times; i++) {
    setMotor(speed, speed / 2);  // Turn right
    delay(delayTime);
    setMotor(speed / 2, speed);  // Turn left
    delay(delayTime);
  }
  stopMotors();
}

void loop() {
  // Dance sequence
  spinInPlace(200, 500);
  wiggle(150, 4, 300);
  spinInPlace(-200, 500); // Spin other way
  delay(1000); // Pause
}
