/*
 * ServoTesterArduino
 * 
 * This sketch allows testing of the StepCraft CNC to Servo interface
 * via USB serial connection from a computer.
 * 
 * It simulates the binary inputs that would come from the CNC machine
 * and controls the servo accordingly.
 */

#include <Servo.h>

// Pin Definitions
// Output pins to simulate CNC outputs
#define OUTPUT_PIN1 2  // LSB
#define OUTPUT_PIN2 3
#define OUTPUT_PIN3 4
#define OUTPUT_PIN4 5  // MSB (if used)

// Servo control pin
#define SERVO_PIN 9   // PWM capable pin on Arduino Nano

// Servo parameters
#define SERVO_MIN_PULSE 1000  // Minimum pulse width in microseconds (adjust for your servo)
#define SERVO_MAX_PULSE 2000  // Maximum pulse width in microseconds (adjust for your servo)
#define SERVO_MIN_ANGLE 0     // Minimum angle (degrees)
#define SERVO_MAX_ANGLE 90    // Maximum angle (degrees)

// Number of bits to use for position encoding
#define NUM_POSITION_BITS 4   // Using 4 bits for 16 positions

Servo myServo;
int currentPosition = 0;
String inputBuffer = "";
bool inputComplete = false;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Servo Tester Ready");
  
  // Initialize output pins (to simulate CNC outputs)
  pinMode(OUTPUT_PIN1, OUTPUT);
  pinMode(OUTPUT_PIN2, OUTPUT);
  pinMode(OUTPUT_PIN3, OUTPUT);
  pinMode(OUTPUT_PIN4, OUTPUT);
  
  // Initialize all pins to LOW (binary 0000)
  digitalWrite(OUTPUT_PIN1, LOW);
  digitalWrite(OUTPUT_PIN2, LOW);
  digitalWrite(OUTPUT_PIN3, LOW);
  digitalWrite(OUTPUT_PIN4, LOW);
  
  // Initialize servo
  myServo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  
  // Move servo to initial position
  myServo.write(SERVO_MIN_ANGLE);
  delay(500);
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    
    // Add character to input buffer
    if (inChar != '\n') {
      inputBuffer += inChar;
    } else {
      inputComplete = true;
    }
  }
  
  // Process complete commands
  if (inputComplete) {
    processCommand(inputBuffer);
    inputBuffer = "";
    inputComplete = false;
  }
}

void processCommand(String command) {
  // Command format: "B0000" where 0000 is a 4-bit binary number
  if (command.length() >= 5 && command.charAt(0) == 'B') {
    String binaryStr = command.substring(1);
    
    // Convert binary string to integer
    int position = 0;
    for (int i = 0; i < binaryStr.length() && i < 4; i++) {
      if (binaryStr.charAt(i) == '1') {
        position |= (1 << (3 - i));
      }
    }
    
    // Set the binary outputs
    setBinaryOutputs(position);
    
    // Set the servo position
    setServoPosition(position);
    
    // Send confirmation
    Serial.print("OK: Position ");
    Serial.print(position);
    Serial.print(" (Binary: ");
    for (int i = 0; i < 4; i++) {
      Serial.print((position >> (3 - i)) & 1);
    }
    Serial.println(")");
  } else {
    Serial.println("ERROR: Invalid command format. Use B0000 (4-bit binary)");
  }
}

void setBinaryOutputs(int position) {
  // Set each output pin according to the binary representation
  digitalWrite(OUTPUT_PIN1, (position & 0x01) ? HIGH : LOW);       // LSB
  digitalWrite(OUTPUT_PIN2, (position & 0x02) ? HIGH : LOW);
  digitalWrite(OUTPUT_PIN3, (position & 0x04) ? HIGH : LOW);
  digitalWrite(OUTPUT_PIN4, (position & 0x08) ? HIGH : LOW);       // MSB
  
  currentPosition = position;
}

void setServoPosition(int position) {
  // Calculate maximum possible position value based on number of bits
  int maxPosition = (1 << NUM_POSITION_BITS) - 1;
  
  // Map position to servo angle range
  int servoAngle = map(position, 0, maxPosition, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
  
  // Set servo position
  myServo.write(servoAngle);
} 