/*
 * StepCraftCNCToServo
 * 
 * This sketch interfaces between a StepCraft CNC machine and a PWM-controlled servo
 * using an Arduino Nano. It reads digital outputs from the CNC and converts them
 * to appropriate PWM signals for the servo.
 * 
 * Created by Claude, 2023
 * Modified to limit servo motion to 0-90 degrees
 */

#include <Servo.h>

// Pin Definitions
// Digital input pins from StepCraft CNC
#define INPUT_PIN1 2  // LSB
#define INPUT_PIN2 3
#define INPUT_PIN3 4
#define INPUT_PIN4 5  // MSB (if used)

// Servo control pin
#define SERVO_PIN 9   // PWM capable pin on Arduino Nano

// Servo parameters
#define SERVO_MIN_PULSE 1000  // Minimum pulse width in microseconds (adjust for your servo)
#define SERVO_MAX_PULSE 2000  // Maximum pulse width in microseconds (adjust for your servo)
#define SERVO_MIN_ANGLE 0     // Minimum angle (degrees)
#define SERVO_MAX_ANGLE 90    // Maximum angle (degrees) - CHANGED FROM 180 TO 90

// Number of bits to use for position encoding (2-4 bits supported)
#define NUM_POSITION_BITS 4   // Change to match your setup

Servo myServo;
int lastServoPosition = -1;  // Track last position to avoid unnecessary updates

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("StepCraft CNC to Servo Interface (0-90 degree range)");
  
  // Initialize input pins
  pinMode(INPUT_PIN1, INPUT_PULLUP);
  pinMode(INPUT_PIN2, INPUT_PULLUP);
  pinMode(INPUT_PIN3, INPUT_PULLUP);
  pinMode(INPUT_PIN4, INPUT_PULLUP);
  
  // Initialize servo
  myServo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  
  // Move servo to initial position
  myServo.write(SERVO_MIN_ANGLE);
  delay(500);
}

void loop() {
  // Read binary input from CNC
  int position = readBinaryPosition();
  
  // Map binary position to servo angle
  int servoAngle = mapPositionToAngle(position);
  
  // Update servo if position has changed
  if (servoAngle != lastServoPosition) {
    myServo.write(servoAngle);
    lastServoPosition = servoAngle;
    
    // Debug output
    Serial.print("Position: ");
    Serial.print(position);
    Serial.print(" -> Angle: ");
    Serial.println(servoAngle);
  }
  
  // Small delay to avoid excessive updates
  delay(10);
}

// Read binary position from input pins
int readBinaryPosition() {
  int position = 0;
  
  // Read each bit and build the position value
  position |= digitalRead(INPUT_PIN1) == LOW ? 1 : 0;
  
  if (NUM_POSITION_BITS >= 2)
    position |= (digitalRead(INPUT_PIN2) == LOW ? 1 : 0) << 1;
    
  if (NUM_POSITION_BITS >= 3)
    position |= (digitalRead(INPUT_PIN3) == LOW ? 1 : 0) << 2;
    
  if (NUM_POSITION_BITS >= 4)
    position |= (digitalRead(INPUT_PIN4) == LOW ? 1 : 0) << 3;
  
  return position;
}

// Map binary position to servo angle
int mapPositionToAngle(int position) {
  // Calculate maximum possible position value based on number of bits
  int maxPosition = (1 << NUM_POSITION_BITS) - 1;
  
  // Map position to servo angle range
  return map(position, 0, maxPosition, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
} 