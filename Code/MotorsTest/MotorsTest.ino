// Motor Pin Definitions
const int RmPWM = 7; // Right motor PWM
const int RmF   = 6; // Right motor Forward
const int RmB   = 5; // Right motor Backward
const int LmPWM = 2; // Left motor PWM
const int LmF   = 3; // Left motor Forward
const int LmB   = 4; // Left motor Backward

void setup() {
  pinMode(RmPWM, OUTPUT);
  pinMode(RmF, OUTPUT);
  pinMode(RmB, OUTPUT);
  pinMode(LmPWM, OUTPUT);
  pinMode(LmF, OUTPUT);
  pinMode(LmB, OUTPUT);
}

void loop() {
  motorTest(100);   // Run test at PWM 100
  motorTest(255);   // Then at PWM 255
}

void motorTest(int pwmVal) {
  // Forward
  driveMotors(pwmVal, pwmVal);
  delay(1000);

  // Backward
  driveMotors(-pwmVal, -pwmVal);
  delay(1000);

  // Turn Right (left motor forward, right motor backward)
  driveMotors(-pwmVal, pwmVal);
  delay(1000);

  // Turn Left (right motor forward, left motor backward)
  driveMotors(pwmVal, -pwmVal);
  delay(1000);

  // Stop
  driveMotors(0, 0);
  delay(5000);
}

void driveMotors(int rightPWM, int leftPWM) {
  // Right Motor Direction
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

  // Left Motor Direction
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

  // Set PWM Speed (absolute value)
  analogWrite(RmPWM, abs(rightPWM));
  analogWrite(LmPWM, abs(leftPWM));
}
