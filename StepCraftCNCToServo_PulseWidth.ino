/*
 * StepCraftCNCToServo_PulseWidth
 * 
 * This sketch interfaces between a StepCraft CNC machine and a PWM-controlled servo
 * using an Arduino Nano. It reads pulse width from a single CNC output and
 * converts it to appropriate PWM signals for the servo.
 * 
 * Created by Claude, 2023
 */

#include <Servo.h>

// Pin Definitions
// Input pin from StepCraft CNC
#define INPUT_PIN 2  // Pin to read pulse width from CNC

// Servo control pin
#define SERVO_PIN 9  // PWM capable pin on Arduino Nano

// Servo parameters
#define SERVO_MIN_PULSE 1000  // Minimum pulse width in microseconds (adjust for your servo)
#define SERVO_MAX_PULSE 2000  // Maximum pulse width in microseconds (adjust for your servo)
#define SERVO_MIN_ANGLE 0     // Minimum angle (degrees)
#define SERVO_MAX_ANGLE 180   // Maximum angle (degrees)

// Pulse width measurement parameters
#define MIN_PULSE_WIDTH 500   // Minimum expected pulse width from CNC (microseconds)
#define MAX_PULSE_WIDTH 2500  // Maximum expected pulse width from CNC (microseconds)
#define PULSE_TIMEOUT 3000    // Timeout for pulse measurement (microseconds)

Servo myServo;
int lastServoAngle = -1;  // Track last position to avoid unnecessary updates
volatile unsigned long pulseStart = 0;
volatile unsigned long pulseWidth = 0;
volatile bool newPulseAvailable = false;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("StepCraft CNC to Servo Interface - Pulse Width Method");
  
  // Initialize input pin
  pinMode(INPUT_PIN, INPUT);
  
  // Attach interrupt to measure pulse width
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), pulseChange, CHANGE);
  
  // Initialize servo
  myServo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  
  // Move servo to initial position
  myServo.write(SERVO_MIN_ANGLE);
  delay(500);
}

void loop() {
  // Check if new pulse width measurement is available
  if (newPulseAvailable) {
    // Map pulse width to servo angle
    int servoAngle = mapPulseToAngle();
    
    // Update servo if position has changed
    if (servoAngle != lastServoAngle) {
      myServo.write(servoAngle);
      lastServoAngle = servoAngle;
      
      // Debug output
      Serial.print("Pulse Width: ");
      Serial.print(pulseWidth);
      Serial.print(" us -> Angle: ");
      Serial.println(servoAngle);
    }
    
    newPulseAvailable = false;
  }
  
  // Small delay to avoid excessive updates
  delay(10);
}

// Interrupt handler for pulse width measurement
void pulseChange() {
  // Current time in microseconds
  unsigned long currentTime = micros();
  
  // If rising edge, record start time
  if (digitalRead(INPUT_PIN) == HIGH) {
    pulseStart = currentTime;
  } 
  // If falling edge, calculate pulse width
  else if (pulseStart > 0) {
    pulseWidth = currentTime - pulseStart;
    
    // Only accept pulses within expected range
    if (pulseWidth >= MIN_PULSE_WIDTH && pulseWidth <= MAX_PULSE_WIDTH) {
      newPulseAvailable = true;
    }
  }
}

// Map pulse width to servo angle
int mapPulseToAngle() {
  // Constrain pulse width to expected range
  unsigned long constrainedPulse = constrain(pulseWidth, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  
  // Map pulse width to servo angle range
  return map(constrainedPulse, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
} 