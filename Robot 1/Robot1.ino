#include <PS4Controller.h>

// Wheel Motor Pins
#define MOTOR3_A 27
#define MOTOR3_B 14
#define MOTOR1_A 26
#define MOTOR1_B 25
#define MOTOR2_A 16
#define MOTOR2_B 17
#define MOTOR4_A 19 // Linear Actuator for Shooting
#define MOTOR4_B 15

// Shooter Pins
#define SHOOTER_A 33
#define SHOOTER_B 32

// Conveyor Pins
#define R_CONV_A 4
#define R_CONV_B 5
#define L_CONV_A 13
#define L_CONV_B 12

float Vx = 0.0, Vy = 0.0, w = 0.0;
float v1 = 0.0, v2 = 0.0, v3 = 0.0, v4 = 0.0;
float shooterSpeed = 0.0;
float conveyorSpeed = 0.0;

void setup() {
  Serial.begin(115200);

  // Set all motor pins as OUTPUT
  int motorPins[] = {MOTOR1_A, MOTOR1_B, MOTOR2_A, MOTOR2_B, MOTOR3_A, MOTOR3_B,
                     MOTOR4_A, MOTOR4_B, SHOOTER_A, SHOOTER_B, R_CONV_A, R_CONV_B, L_CONV_A, L_CONV_B};
  for (int i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  PS4.begin("20:04:20:05:20:06");
  Serial.println("PS4 ready to connect!");
}

void loop() {
  if (PS4.isConnected()) {
    // Map joystick values to motor speeds
    Vx = map(PS4.LStickX(), -128, 127, -255, 255)/4;
    Vy = map(PS4.LStickY(), -128, 127, -255, 255)/4;
    w = map(PS4.RStickX(), -128, 127, -180, 180)/5;

    // Calculate wheel velocities for omnidirectional movement
    v1 = Vx + w ;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w ;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w ;

    // Linear actuator
    if (PS4.Up()) {
      v4 = 255;
    } else if (PS4.Down()) {
      v4 = -255;
    } else {
      v4 = 0;
    }

    // Shooter control
    if (PS4.R2Value() > 0) {
      shooterSpeed = map(PS4.R2Value(), 0, 255, 0, 255);
    } else if (PS4.R1()) {
      shooterSpeed = 170;
    } else {
      shooterSpeed = 0;
    }

    // Conveyor control
    if (PS4.L1()) {
      conveyorSpeed = 255;  // forward
    } else if (PS4.L2()) {
      conveyorSpeed = -255; // backward
    } else {
      conveyorSpeed = 0;
    }

    // Serial Monitor Debug
    Serial.printf("Vx: %.2f\tVy: %.2f\tw: %.2f  \t|| V1: %.2f\tV2: %.2f\tV3: %.2f\tV4: %.2f\tShooter: %.2f\tConv: %.2f\t", Vx, Vy, w, v1, v2, v3, v4, shooterSpeed, conveyorSpeed);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();

  } else {
    Vx = Vy = w = v1 = v2 = v3 = v4 = shooterSpeed = conveyorSpeed = 0;
    Serial.printf("CONTROLLER DISCONNECTED\tAll motors set to 0\n");
  }

  // Drive Motors
  controlMotor(v1, MOTOR1_A, MOTOR1_B);
  controlMotor(v2, MOTOR2_A, MOTOR2_B);
  controlMotor(v3, MOTOR3_A, MOTOR3_B);
  controlMotor(v4, MOTOR4_A, MOTOR4_B);

  // Shooter Motor
  controlMotor(shooterSpeed, SHOOTER_A, SHOOTER_B);

  // Conveyors
  controlMotor(conveyorSpeed, R_CONV_A, R_CONV_B);
  controlMotor(conveyorSpeed, L_CONV_A, L_CONV_B);
}

void controlMotor(float v_target, int in1, int in2) {
  int pwmVal = (int) fabs(v_target);
  if (pwmVal > 255) pwmVal = 255;

  if (v_target > 0) {
    analogWrite(in1, pwmVal);
    analogWrite(in2, LOW);
  } else if (v_target < 0) {
    analogWrite(in1, LOW);
    analogWrite(in2, pwmVal);
  } else {
    analogWrite(in1, LOW);
    analogWrite(in2, LOW);
  }
}
