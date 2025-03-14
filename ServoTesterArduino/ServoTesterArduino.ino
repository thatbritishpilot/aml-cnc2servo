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
#include <math.h>

// Pin Definitions
// Output pins to simulate CNC outputs
#define OUTPUT_PIN1 2  // LSB
#define OUTPUT_PIN2 3
#define OUTPUT_PIN3 4
#define OUTPUT_PIN4 5  // MSB

// Servo control pin
#define SERVO_PIN 9   // PWM capable pin on Arduino Nano

// Servo parameters
#define SERVO_MIN_PULSE 1000  // Minimum pulse width in microseconds
#define SERVO_MAX_PULSE 2000  // Maximum pulse width in microseconds
#define SERVO_MIN_PWM 0       // Minimum PWM value
#define SERVO_MAX_PWM 255     // Maximum PWM value
#define MOVEMENT_DURATION 1000 // Time to move between positions in milliseconds
#define MOVEMENT_STEPS 50     // Number of steps for smooth movement

// Vibration parameters
#define VIBRATION_DURATION 5000  // Duration of vibration in milliseconds
#define VIBRATION_AMPLITUDE 20   // Amplitude of vibration in PWM units
#define VIBRATION_DELAY 20      // Delay between movements (20ms = 50Hz)

// Number of bits to use for position encoding
#define NUM_POSITION_BITS 4   // Using 4 bits for 16 positions

Servo myServo;
int currentPosition = 0;
String inputBuffer = "";
bool inputComplete = false;
volatile bool interruptVibration = false;  // Flag to interrupt vibration

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
  myServo.write(map(SERVO_MIN_PWM, 0, 255, 0, 180));
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
    
    // Set the servo position and vibrate
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
  // Interrupt any ongoing vibration
  interruptVibration = true;
  
  // Calculate maximum possible position value based on number of bits
  int maxPosition = (1 << NUM_POSITION_BITS) - 1;
  
  // Map position to PWM range
  int targetPWM = map(position, 0, maxPosition, SERVO_MIN_PWM, SERVO_MAX_PWM);
  
  // Get current servo position
  int currentPWM = map(myServo.read(), 0, 180, 0, 255);
  
  // Calculate PWM difference
  int pwmDiff = targetPWM - currentPWM;
  
  // Move smoothly to new position
  unsigned long startTime = millis();
  unsigned long lastStepTime = startTime;
  int stepDelay = MOVEMENT_DURATION / MOVEMENT_STEPS;
  int step = 1;
  
  // Non-blocking movement loop
  while (step <= MOVEMENT_STEPS) {
    unsigned long currentTime = millis();
    
    if (currentTime - lastStepTime >= stepDelay) {
      // Calculate intermediate position using linear interpolation
      float progress = (float)step / MOVEMENT_STEPS;
      int newPWM = currentPWM + (pwmDiff * progress);
      
      // Convert PWM to degrees for the servo
      int servoAngle = map(newPWM, 0, 255, 0, 180);
      
      // Move servo
      myServo.write(servoAngle);
      
      lastStepTime = currentTime;
      step++;
    }
    
    // Check for serial input during movement
    if (Serial.available() > 0) {
      return;  // Exit if new command received
    }
  }
  
  // Ensure we reach exactly the target position
  myServo.write(map(targetPWM, 0, 255, 0, 180));
  
  // Reset interrupt flag and start vibration
  interruptVibration = false;
  vibrateServo(targetPWM);
}

void vibrateServo(int basePWM) {
  unsigned long startTime = millis();
  unsigned long lastMove = 0;
  int step = 0;
  const int numSteps = 8;  // Number of positions in the vibration cycle
  
  // Pre-calculate vibration positions in PWM values
  int positions[8];
  for (int i = 0; i < numSteps; i++) {
    int pwmOffset = (i < numSteps/2 ? VIBRATION_AMPLITUDE : -VIBRATION_AMPLITUDE);
    positions[i] = constrain(basePWM + pwmOffset, SERVO_MIN_PWM, SERVO_MAX_PWM);
  }
  
  while (millis() - startTime < VIBRATION_DURATION) {
    // Check for interrupt flag or new serial data
    if (interruptVibration || Serial.available() > 0) {
      return;  // Exit immediately if interrupted
    }
    
    unsigned long currentTime = millis();
    
    // Only update position at fixed intervals
    if (currentTime - lastMove >= VIBRATION_DELAY) {
      // Convert PWM to degrees for the servo
      myServo.write(map(positions[step], 0, 255, 0, 180));
      step = (step + 1) % numSteps;
      lastMove = currentTime;
    }
  }
  
  // Only return to base position if we weren't interrupted
  if (!interruptVibration) {
    myServo.write(map(basePWM, 0, 255, 0, 180));
  }
} 