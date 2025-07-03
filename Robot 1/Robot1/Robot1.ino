#include <PS4Controller.h>

// Movement motors
#define MOTOR3_A 33 //32
#define MOTOR3_B 32 //33
#define MOTOR2_A 26 //25
#define MOTOR2_B 25 //26
#define MOTOR1_A 14
#define MOTOR1_B 27

// Lifter (Linear Actuator)
#define LIFTER_A 5 //4
#define LIFTER_B 17 //16

// Conveyor (shared)
#define CONVEYOR_A 13 //17
#define CONVEYOR_B 12 //5

// Shooter
#define SHOOTER_A 4 //12
#define SHOOTER_B 16 //13

// // Lifter (Linear Actuator)
// #define LIFTER_A 12
// #define LIFTER_B 13

// // Conveyor (shared)
// #define CONVEYOR_A 4
// #define CONVEYOR_B 16

// // Shooter
// #define SHOOTER_A 17
// #define SHOOTER_B 5

float Vx = 0.0, Vy = 0.0, w = 0.0;
float v1 = 0.0, v2 = 0.0, v3 = 0.0, lifter = 0.0;
float shooterSpeed = 0.0;
float conveyorSpeed = 0.0;
int mode = 0;
bool lastTriangleState = false;
bool conveyorUpActive = false;
bool conveyorDownActive = false;
unsigned long conveyorTimer = 0;
bool lastL1State = false;
bool lastL2State = false;


void setup() {
  Serial.begin(115200);

  int motorPins[] = {
    MOTOR1_A, MOTOR1_B, MOTOR2_A, MOTOR2_B,
    MOTOR3_A, MOTOR3_B, LIFTER_A, LIFTER_B,
    SHOOTER_A, SHOOTER_B, CONVEYOR_A, CONVEYOR_B
  };

  for (int i = 0; i < sizeof(motorPins) / sizeof(motorPins[0]); i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  PS4.begin("10:04:10:05:10:06");
  Serial.println("PS4 ready to connect!");
}

void loop() {
  bool currentTriangleState = PS4.Triangle();
  bool currentL1State = PS4.L1();
  bool currentL2State = PS4.L2();
  unsigned long currentTime = millis();

  if (PS4.isConnected()) {
    if (currentTriangleState && !lastTriangleState) {
      mode = (mode == 0) ? 1 : 0;
    }
    lastTriangleState = currentTriangleState;

    // Movement control
    if (mode == 0) {
      Vx = map(PS4.LStickX(), -128, 127, -255, 255) / 1.0;
      Vy = map(PS4.LStickY(), -128, 127, 255, -255) / 1.0;
      w  = map(PS4.RStickX(), -128, 127, -180, 180) / 1.0;
    } else {
      Vx = map(PS4.LStickX(), -128, 127, -255, 255) / 0.5;
      Vy = map(PS4.LStickY(), -128, 127, 255, -255) / 0.5;
      w  = map(PS4.RStickX(), -128, 127, -180, 180) / 0.5;
    }

    v1 = Vx + w;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w;

    // Lifter
    if (PS4.Down()) {
      lifter = 255;
    } else if (PS4.Up()) {
      lifter = -255;
    } else {
      lifter = 0;
    }

    // Shooter
    if (PS4.R2Value() > 0) {
      shooterSpeed = map(PS4.R2Value(), 0, 225, 0, 180);
    } else if (PS4.R1()) {
      shooterSpeed = 80;
      // shooterSpeed = 80;
    } else {
      shooterSpeed = 0;
    }

        // Conveyor control
    if (PS4.L2()) {
      // Forward: left slightly slower
      conveyorSpeed = 255;
    } else if (PS4.L1()) {
      // Backward: right slightly slower
      conveyorSpeed = -255;
    } else {
      conveyorSpeed = 0;
    }
    // // Start conveyor UP sequence (L1 press)
    // if (currentL1State && !lastL1State && !conveyorUpActive && !conveyorDownActive) {
    //   conveyorUpActive = true;
    //   conveyorTimer = currentTime;
    // }

    // // Start conveyor DOWN sequence (L2 press)
    // if (currentL2State && !lastL2State && !conveyorDownActive && !conveyorUpActive) {
    //   conveyorDownActive = true;
    //   conveyorTimer = currentTime;
    // }

    // // Conveyor UP logic
    // if (conveyorUpActive) {
    //   if (currentTime - conveyorTimer < 420) {
    //     conveyorSpeed = 255;
    // } else {
    //   conveyorSpeed = 0;
    //   conveyorUpActive = false;
    // }
    // }

    // // Conveyor DOWN logic
    // else if (conveyorDownActive) {
    //   if (currentTime - conveyorTimer < 310) {
    //     conveyorSpeed = -255;
    //   } else {
    //     conveyorSpeed = 0;
    //     conveyorDownActive = false;
    //   }
    // }

    // // If no active timed movement
    // else {
    //   conveyorSpeed = 0;
    // }

    // // Save button states for rising edge detection 
    // lastL1State = currentL1State;
    // lastL2State = currentL2State;

    Serial.printf("Vx: %.2f Vy: %.2f w: %.2f | V1: %.2f V2: %.2f V3: %.2f | Lifter: %.2f | Shooter: %.2f | Conveyor: %.2f | Mode: %d\n",
      Vx, Vy, w, v1, v2, v3, lifter, shooterSpeed, conveyorSpeed, mode);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();
  } else {
    v1 = v2 = v3 = lifter = shooterSpeed = conveyorSpeed = 0;
    Serial.println("Controller disconnected");
  }

  // Apply motors
  controlMotor((abs(v1) > 24) ? v1 : 0, MOTOR1_A, MOTOR1_B);
  controlMotor((abs(v2) > 10) ? v2 : 0, MOTOR2_A, MOTOR2_B);
  controlMotor((abs(v3) > 10) ? v3 : 0, MOTOR3_A, MOTOR3_B);

  controlMotor(lifter, LIFTER_A, LIFTER_B);
  controlMotor(shooterSpeed, SHOOTER_A, SHOOTER_B);
  controlMotor(conveyorSpeed, CONVEYOR_A, CONVEYOR_B);
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
