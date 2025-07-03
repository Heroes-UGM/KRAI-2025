// ROBOT 2 PNEUMATIC
#include <PS4Controller.h>

#define MOTOR1_A 27 // Wheel 3
#define MOTOR1_B 14
#define MOTOR3_A 32 // Wheel 2
#define MOTOR3_B 33
#define MOTOR2_A 25 // Wheel 1
#define MOTOR2_B 26
#define MOTOR4_A 12 // Shooter
#define MOTOR4_B 13
#define pneumatic_A 4
#define pneumatic_B 5

float Vx = 0.0;
float Vy = 0.0;
float w = 0.0;
float v1 = 0.0;
float v2 = 0.0;
float v3 = 0.0;
float v4 = 0.0;
float rawVx = 0.0;
float rawVy = 0.0;
bool status;
int mode = 0;
bool lastRectangleState = false;

bool inverse = false;           // Inverse mode toggle
bool prevCircleState = false;   // To detect button press transition

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
  pinMode(pneumatic_A, OUTPUT);
  pinMode(pneumatic_B, OUTPUT);


  Serial.println("Starting PS4 controller setup...");
  digitalWrite(pneumatic_A, LOW);
  digitalWrite(pneumatic_B, LOW);
  // PS4.begin("20:04:20:05:20:06"); punya robot 2 asli
  PS4.begin("10:04:10:05:10:06"); // dari robot 1
  Serial.println("PS4 controller initialized!");
  Serial.println("Please connect your PS4 controller...");
  // Serial.println("Press and hold PS + Share buttons simultaneously for 3 seconds");
}

void loop() {
  bool currentRectangleState = PS4.Square();
  if (PS4.isConnected()) {

    // Handle inverse mode toggle
    inverseMode();

      // debouncing Detect rising edge: when button is pressed now but wasn't before
    if(currentRectangleState && !lastRectangleState){
      mode = (mode == 0) ? 1 : 0; 
    }

    // Save current state for next loop iteration
    lastRectangleState = currentRectangleState;

    // Map joystick values to motor speeds
    if(mode == 0){
      // Map joystick values
      rawVx = map(PS4.LStickX(), -128, 127, -255, 255) / 3.0;
      rawVy = map(PS4.LStickY(), -128, 127, -255, 255) / 3.0;
      w = map(PS4.RStickX(), -128, 127, -180, 180) / 5.0;
    } else if (mode == 1) {
      rawVx = map(PS4.LStickX(), -128, 127, -255, 255) / 2.0;
      rawVy = map(PS4.LStickY(), -128, 127, -255, 255) / 2.0;
      w = map(PS4.RStickX(), -128, 127, -180, 180) / 3.0;
    }

    // Apply inverse if active
    Vx = inverse ? -rawVx : rawVx;
    Vy = inverse ? -rawVy : rawVy;

    // Calculate wheel velocities
    v1 = Vx + w;
    v2 = -Vx * cos(60 * PI / 180) - Vy * cos(30 * PI / 180) + w;
    v3 = -Vx * cos(60 * PI / 180) + Vy * cos(30 * PI / 180) + w;

    // Shooter logic
    if (PS4.R2()) {
      v4 = 150;
    } else if (PS4.R1()) {
      v4 = 120;
    } else if (PS4.Triangle()) {
      v4 = 200;
    } else {
      v4 = 0;
    }

    // Pneumatic_A control
    if (PS4.L1()) {
      digitalWrite(pneumatic_A, HIGH);
    } else if (PS4.L2()) {
      digitalWrite(pneumatic_A, LOW);
    }

    // Pneumatic_B control
    if (PS4.Up()) {
      digitalWrite(pneumatic_B, HIGH);
    } else if (PS4.Down()) {
      digitalWrite(pneumatic_B, LOW);
    }
    

    // Debug info
    Serial.printf("Vx: %.2f\tVy: %.2f\tw: %.2f   \t||\tV1: %.2f\tV2: %.2f\tV3: %.2f\tV4: %.2f\tInverse: %s\tMode: %d\n", 
                   Vx, Vy, w, v1, v2, v3, v4, inverse ? "ON" : "OFF", mode);
    Serial.printf("Battery: %d", PS4.Battery());
    if (PS4.Charging()) Serial.print(" (Charging)");
    Serial.println();

    // Motor control
    controlMotor(v1, MOTOR1_A, MOTOR1_B);
    controlMotor(v2, MOTOR2_A, MOTOR2_B);
    controlMotor(v3, MOTOR3_A, MOTOR3_B);
    controlMotor(v4, MOTOR4_A, MOTOR4_B);
  } else {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 2000) {
      Serial.println("PS4 controller not connected. Waiting...");
      lastPrint = millis();
    }
  }
}

void controlMotor(float v_target, int in1, int in2) {
  int pwmVal = (int)fabs(v_target);
  if (pwmVal > 255) pwmVal = 255;

  int dir = (v_target >= 0) ? 1 : -1;

  if (dir == 1) {
    analogWrite(in1, pwmVal);
    analogWrite(in2, 0);
  } else {
    analogWrite(in1, 0);
    analogWrite(in2, pwmVal);
  }
}

void inverseMode() {
  bool currentCircleState = PS4.Cross();

  // Toggle inverse mode only on press (not hold)
  if (currentCircleState && !prevCircleState) {
    inverse = !inverse;
    Serial.print("Inverse mode toggled: ");
    Serial.println(inverse ? "ON" : "OFF");
  }

  prevCircleState = currentCircleState;
}