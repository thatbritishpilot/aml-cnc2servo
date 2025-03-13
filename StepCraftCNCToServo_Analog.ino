/*
 * StepCraftCNCToServo_Analog
 * 
 * This sketch interfaces between a StepCraft CNC machine and a PWM-controlled servo
 * using an Arduino Nano. It reads an analog voltage from the CNC and
 * converts it to appropriate PWM signals for the servo.
 * 
 * Created by Claude, 2023
 */

#include <Servo.h>

// Pin Definitions
// Analog input pin from StepCraft CNC
#define ANALOG_INPUT_PIN A0  // Analog pin to read voltage from CNC

// Servo control pin
#define SERVO_PIN 9  // PWM capable pin on Arduino Nano

// Servo parameters
#define SERVO_MIN_PULSE 1000  // Minimum pulse width in microseconds (adjust for your servo)
#define SERVO_MAX_PULSE 2000  // Maximum pulse width in microseconds (adjust for your servo)
#define SERVO_MIN_ANGLE 0     // Minimum angle (degrees)
#define SERVO_MAX_ANGLE 180   // Maximum angle (degrees)

// Analog reading parameters
#define ANALOG_SAMPLES 10     // Number of samples to average for noise reduction
#define ANALOG_MIN_VALUE 0    // Minimum expected analog value (0-1023)
#define ANALOG_MAX_VALUE 1023 // Maximum expected analog value (0-1023)
#define ANALOG_THRESHOLD 5    // Minimum change required to update servo

Servo myServo;
int lastServoAngle = -1;  // Track last position to avoid unnecessary updates
int lastAnalogReading = -1;  // Track last analog reading

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("StepCraft CNC to Servo Interface - Analog Method");
  
  // Initialize analog input pin (not strictly necessary for analog inputs)
  pinMode(ANALOG_INPUT_PIN, INPUT);
  
  // Initialize servo
  myServo.attach(SERVO_PIN, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  
  // Move servo to initial position
  myServo.write(SERVO_MIN_ANGLE);
  delay(500);
}

void loop() {
  // Read analog input with averaging for noise reduction
  int analogReading = readAveragedAnalog();
  
  // Only process if the reading has changed significantly
  if (abs(analogReading - lastAnalogReading) > ANALOG_THRESHOLD) {
    // Map analog reading to servo angle
    int servoAngle = mapAnalogToAngle(analogReading);
    
    // Update servo if position has changed
    if (servoAngle != lastServoAngle) {
      myServo.write(servoAngle);
      lastServoAngle = servoAngle;
      
      // Debug output
      Serial.print("Analog Reading: ");
      Serial.print(analogReading);
      Serial.print(" -> Angle: ");
      Serial.println(servoAngle);
    }
    
    lastAnalogReading = analogReading;
  }
  
  // Small delay to avoid excessive updates
  delay(20);
}

// Read analog input with averaging for noise reduction
int readAveragedAnalog() {
  long sum = 0;
  
  // Take multiple samples and average them
  for (int i = 0; i < ANALOG_SAMPLES; i++) {
    sum += analogRead(ANALOG_INPUT_PIN);
    delayMicroseconds(100);  // Short delay between readings
  }
  
  // Return the average
  return sum / ANALOG_SAMPLES;
}

// Map analog reading to servo angle
int mapAnalogToAngle(int analogValue) {
  // Constrain analog value to expected range
  int constrainedValue = constrain(analogValue, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
  
  // Map analog value to servo angle range
  return map(constrainedValue, ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
} 