#include <PS4Controller.h>

// Wheel Motor Pins
#define MOTOR1_A 32
#define MOTOR1_B 33
#define MOTOR2_A 25
#define MOTOR2_B 26
#define MOTOR3_A 14
#define MOTOR3_B 27
#define MOTOR4_A 12 // Linear Actuator for Shooting
#define MOTOR4_B 13

// Shooter Pins
#define SHOOTER_A 16
#define SHOOTER_B 17

// Conveyor Pins
#define R_CONV_A 4
#define R_CONV_B 5
#define L_CONV_A 18
#define L_CONV_B 19

float Vx = 0.0, Vy = 0.0, w = 0.0;
float v1 = 0.0, v2 = 0.0, v3 = 0.0, v4 = 0.0;
float shooterSpeed = 0.0;
float conveyorLeftSpeed = 0.0;
float conveyorRightSpeed = 0.0;
int mode = 0;
bool lastTriangleState = false;

void setup() {
  Serial.begin(115200);

  // Set all motor pins as OUTPUT
  int motorPins[] = {
    MOTOR1_A, MOTOR1_B, MOTOR2_A, MOTOR2_B,
    MOTOR3_A, MOTOR3_B, MOTOR4_A, MOTOR4_B,
    SHOOTER_A, SHOOTER_B, R_CONV_A, R_CONV_B,
    L_CONV_A, L_CONV_B
  };

  for (int i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  PS4.begin("10:04:10:05:10:06");
  Serial.println("PS4 ready to connect!");
}

void loop() {
  bool currentTriangleState = PS4.Triangle();
  if (PS4.isConnected()) {

    
  // debouncing Detect rising edge: when button is pressed now but wasn't before
    if(currentTriangleState && !lastTriangleState){
      mode = (mode == 0) ? 1 : 0; 
    }

    // Save current state for next loop iteration
    lastTriangleState = currentTriangleState;

    // Map joystick values to motor speeds
    if(mode == 0){
      Vx = map(PS4.LStickX(), -128, 127, -255, 255) / 2;
      Vy = map(PS4.LStickY(), -128, 127, 255, -255) / 2;
      w = map(PS4.RStickX(), -128, 127, -180, 180) / 2;
    } else if (mode == 1) {
      Vx = map(PS4.LStickX(), -128, 127, -255, 255) / 1.5;
      Vy = map(PS4.LStickY(), -128, 127, 255, -255) / 1.5;
      w = map(PS4.RStickX(), -128, 127, -180, 180) / 1.5;
    }
    // Calculate wheel velocities for omni movement
    v1 = Vx + w;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w;

    // Linear actuator (MOTOR4)
    if (PS4.Down()) {
      v4 = 255;
    } else if (PS4.Up()) {
      v4 = -255;
    } else {
      v4 = 0;
    }

    // Shooter control
    if (PS4.R2Value() > 0) {
      shooterSpeed = map(PS4.R2Value(), 0, 255, 0, 125);
    } else if (PS4.R1()) {
      shooterSpeed = 80;
    } else {
      shooterSpeed = 0;
    }

    // Conveyor control
    if (PS4.L2()) {
      // Forward: left slightly slower
      conveyorLeftSpeed = 225;
      conveyorRightSpeed = 255;
    } else if (PS4.L1()) {
      // Backward: right slightly slower
      conveyorLeftSpeed = -225;
      conveyorRightSpeed = -255;
    } else {
      conveyorLeftSpeed = 0;
      conveyorRightSpeed = 0;
    }

    // Debug Info
    Serial.printf("Vx: %.2f\tVy: %.2f\tw: %.2f  \t|| V1: %.2f\tV2: %.2f\tV3: %.2f\tV4: %.2f\tShooter: %.2f\tConv L: %.2f\tConv R: %.2f\tMode: %d\n",
                  Vx, Vy, w, v1, v2, v3, v4, shooterSpeed, conveyorLeftSpeed, conveyorRightSpeed, mode);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();

  } else {
    // Controller disconnected: stop all motors
    Vx = Vy = w = v1 = v2 = v3 = v4 = shooterSpeed = conveyorLeftSpeed = conveyorRightSpeed = 0;
    Serial.println("CONTROLLER DISCONNECTED\tAll motors set to 0");
  }

  // Drive motors
// Apply deadzone of ±10 for base movement
  controlMotor((abs(v1) > 24) ? v1 : 0, MOTOR1_A, MOTOR1_B);
  controlMotor((abs(v2) > 10) ? v2 : 0, MOTOR2_A, MOTOR2_B);
  controlMotor((abs(v3) > 10) ? v3 : 0, MOTOR3_A, MOTOR3_B);

  controlMotor(v4, MOTOR4_A, MOTOR4_B);

  // Shooter
  controlMotor(shooterSpeed, SHOOTER_A, SHOOTER_B);

  // Conveyors
  controlMotor(conveyorRightSpeed, R_CONV_A, R_CONV_B);
  controlMotor(conveyorLeftSpeed, L_CONV_A, L_CONV_B);
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
