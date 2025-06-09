// ROBOT 2 PNEUMATIC
#include <PS4Controller.h>

#define MOTOR3_A 12 // Wheel 3
#define MOTOR3_B 13
#define MOTOR1_A 32 // Wheel 2
#define MOTOR1_B 33
#define MOTOR2_A 25 // Wheel 1
#define MOTOR2_B 26
#define MOTOR4_A 14// Shooter
#define MOTOR4_B 27
#define pneumatic 2


float Vx = 0.0;
float Vy = 0.0;
float w = 0.0;
float v1 = 0.0;
float v2 = 0.0;
float v3 = 0.0;
float v4 = 0.0;
bool status;

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
  pinMode(pneumatic, OUTPUT);

  Serial.println("Starting PS4 controller setup...");
  
  // Initialize PS4 controller with MAC address
  // Make sure this MAC matches your PS4 controller's MAC
  digitalWrite(pneumatic, LOW);
  PS4.begin("20:04:20:05:20:06");
  
  Serial.println("PS4 controller initialized!");
  Serial.println("Please connect your PS4 controller...");
  Serial.println("Press and hold PS + Share buttons simultaneously for 3 seconds");
}

void loop() {
  if (PS4.isConnected()) {
    // Map joystick values to motor speeds
    Vx = map(PS4.LStickX(), -128, 127, -255, 255)/3;
    Vy = map(PS4.LStickY(), -128, 127, -255, 255)/3;
    w = map(PS4.RStickX(), -128, 127, -180, 180)/5;

    // Calculate wheel velocities for omnidirectional movement
    v1 = Vx + w ;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w ;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w ;
    
    // Shooter and conveyor controls
    if (PS4.R2()) {
      v4 = 150; // If R1 is pressed, set shooter power to 127
    } else if (PS4.R1()) {
      v4 = 100; // If R1 is pressed, set shooter power to 127  
      // v4 = PS4.R1Value(); // If R2 is pressed, use analog trigger value
    } else if (PS4.Triangle()){
      v4 = 200; // If R1 is pressed, set shooter power to 127  
    } else {
      v4 = 0; // Otherwise, turn off
    }

    // Linear actuator control
    if (PS4.L1()) {
      digitalWrite(pneumatic, HIGH); // L1 pressed → Activate pneumatic
    } else if (PS4.L2()) {
      digitalWrite(pneumatic, LOW);  // L2 pressed → Deactivate pneumatic
    }

    // Debug output
    Serial.printf("Vx: %.2f   \tVy: %.2f   \tw: %.2f    \t||\t  V1: %.2f\tV2: %.2f\tV3: %.2f\tV4: %.2f\tpneumatic: %.2f\t", Vx, Vy, w, v1, v2, v3, v4);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();

    // Control all motors
    controlMotor(v1, MOTOR1_A, MOTOR1_B);
    controlMotor(v2, MOTOR2_A, MOTOR2_B);
    controlMotor(v3, MOTOR3_A, MOTOR3_B);
    controlMotor(v4, MOTOR4_A, MOTOR4_B); 
    
    // delay(20); // Small delay for stability
  } else {
    // Connection status
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
      Serial.println("PS4 controller not connected. Waiting...");
      lastPrint = millis();
    }
    // delay(100);
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
