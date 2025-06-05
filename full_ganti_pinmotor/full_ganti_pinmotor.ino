#include <PS4Controller.h>

#define MOTOR3_A 25 // Wheel 3
#define MOTOR3_B 26
#define MOTOR1_A 12 // Wheel 2
#define MOTOR1_B 13
#define MOTOR2_A 17 // Wheel 1
#define MOTOR2_B 16
#define MOTOR4_A 6 // Linear Actuator for Shooting
#define MOTOR4_B 7
#define MOTOR5_A 27 // Shooter
#define MOTOR5_B 14 
#define MOTOR6_A 8 // Conveyor kanan
#define MOTOR6_B 15
#define MOTOR7_A 4 // conveyor kiri
#define MOTOR7_B 5
#define MOTOR8_A 33 // Naik kana conve bawah
#define MOTOR8_B 32 

float Vx = 0.0;
float Vy = 0.0;
float w = 0.0;
float v1 = 0.0;
float v2 = 0.0;
float v3 = 0.0;
float v4 = 0.0;
float v5 = 0.0;
float v6 = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Initialize motor pins
  pinMode(MOTOR1_A, OUTPUT);
  pinMode(MOTOR1_B, OUTPUT);
  pinMode(MOTOR2_A, OUTPUT);
  pinMode(MOTOR2_B, OUTPUT);
  pinMode(MOTOR3_A, OUTPUT);
  pinMode(MOTOR3_B, OUTPUT);
  pinMode(MOTOR4_A, OUTPUT);
  pinMode(MOTOR4_B, OUTPUT);
  pinMode(MOTOR5_A, OUTPUT);
  pinMode(MOTOR5_B, OUTPUT);
  pinMode(MOTOR6_A, OUTPUT);
  pinMode(MOTOR6_B, OUTPUT);
  pinMode(MOTOR7_A, OUTPUT);
  pinMode(MOTOR7_B, OUTPUT);
  pinMode(MOTOR8_A, OUTPUT);
  pinMode(MOTOR8_B, OUTPUT);

  Serial.println("Starting PS4 controller setup...");
  
  // Initialize PS4 controller with MAC address
  // Make sure this MAC matches your PS4 controller's MAC
  PS4.begin("d0:2a:aa:06:22:c2");
  
  Serial.println("PS4 controller initialized!");
  Serial.println("Please connect your PS4 controller...");
  Serial.println("Press and hold PS + Share buttons simultaneously for 3 seconds");
}

void loop() {
  if (PS4.isConnected()) {
    // Map joystick values to motor speeds
    Vx = map(PS4.LStickX(), -128, 127, -255, 255);
    Vy = map(PS4.LStickY(), -128, 127, -255, 255);
    w = map(PS4.RStickX(), -128, 127, -180, 180);

    // Calculate wheel velocities for omnidirectional movement
    v1 = Vx + w / 2;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w / 2;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w / 2;
    
    // Shooter and conveyor controls
    v5 = map(PS4.R2(), 0, 255, 0, 255);  // Right trigger for shooter
    v6 = map(PS4.L2(), 0, 255, 0, 255);  // Left trigger for conveyor

    // Linear actuator control
    if (PS4.Up()) {
      v4 = 255;
    } else if (PS4.Down()) {
      v4 = -255;
    } else {
      v4 = 0;
    }

    // Debug output
    Serial.printf("Vx: %.2f   \tVy: %.2f   \tw: %.2f    \t||\t  V1: %.2f\tV2: %.2f\tV3: %.2f\tV4: %.2f\t", Vx, Vy, w, v1, v2, v3, v4);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();

    // Control all motors
    controlMotor(v1, MOTOR1_A, MOTOR1_B);
    controlMotor(v2, MOTOR2_A, MOTOR2_B);
    controlMotor(v3, MOTOR3_A, MOTOR3_B);
    controlMotor(v4, MOTOR4_A, MOTOR4_B);
    controlMotor(v5, MOTOR5_A, MOTOR5_B);
    controlMotor(v6, MOTOR6_A, MOTOR6_B);   
    
    delay(20); // Small delay for stability
  } else {
    // Connection status
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
      Serial.println("PS4 controller not connected. Waiting...");
      lastPrint = millis();
    }
    delay(100);
  }
}

void controlMotor(float v_target, int in1, int in2) {
  int pwmVal = (int) fabs(v_target);
  if (pwmVal > 255) pwmVal = 255;

  int dir = (v_target >= 0) ? 1 : -1;

  if (dir == 1) {
    analogWrite(in1, pwmVal);
    analogWrite(in2, 0);  // Use 0 instead of LOW for clarity
  } else {
    analogWrite(in1, 0);
    analogWrite(in2, pwmVal);
  }
}