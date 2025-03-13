# StepCraft CNC to Servo Interface

This project provides an Arduino Nano-based interface between a StepCraft CNC machine and a PWM-controlled servo motor. Three different implementation methods are provided to suit different CNC setups.

## Hardware Requirements

- Arduino Nano
- PWM-compatible servo motor
- Jumper wires
- Breadboard (optional)
- 5V power supply (if servo requires more power than Arduino can provide)

## Implementation Methods

### 1. Binary Method (StepCraftCNCToServo.ino)

Uses multiple digital outputs from the CNC to create a binary number representing the desired servo position.

#### Wiring:
- CNC Output 1 → Arduino Pin 2 (LSB)
- CNC Output 2 → Arduino Pin 3
- CNC Output 3 → Arduino Pin 4
- CNC Output 4 → Arduino Pin 5 (MSB)
- Servo signal → Arduino Pin 9
- Servo power → 5V
- Servo ground → GND

#### How It Works:
The sketch reads up to 4 digital inputs from the CNC machine, treating them as a binary number:
- With 2 bits: 4 possible positions (0-3)
- With 3 bits: 8 possible positions (0-7)
- With 4 bits: 16 possible positions (0-15)

The binary value is mapped to the servo's angle range (0-180 degrees by default).

### 2. Pulse Width Method (StepCraftCNCToServo_PulseWidth.ino)

Uses a single digital output from the CNC that produces pulses of varying width to control the servo position.

#### Wiring:
- CNC Pulse Output → Arduino Pin 2
- Servo signal → Arduino Pin 9
- Servo power → 5V
- Servo ground → GND

#### How It Works:
The sketch measures the width of pulses coming from the CNC machine using interrupts. The pulse width is then mapped to the servo's angle range. This method is similar to how RC receivers control servos.

### 3. Analog Method (StepCraftCNCToServo_Analog.ino)

Uses an analog voltage output from the CNC to control the servo position.

#### Wiring:
- CNC Analog Output → Arduino Pin A0
- Servo signal → Arduino Pin 9
- Servo power → 5V
- Servo ground → GND

#### How It Works:
The sketch reads an analog voltage from the CNC machine using the Arduino's ADC. The analog value (0-1023) is mapped to the servo's angle range. Multiple readings are averaged to reduce noise.

## Choosing the Right Method

- **Binary Method**: Best when you have multiple digital outputs available and need precise, discrete positions.
- **Pulse Width Method**: Best when you have a single digital output that can generate pulses of varying width.
- **Analog Method**: Best when you have an analog output available and need smooth, continuous motion.

## Configuration

Each sketch has configurable parameters that you can adjust:

- Servo pulse width range (`SERVO_MIN_PULSE` and `SERVO_MAX_PULSE`)
- Servo angle range (`SERVO_MIN_ANGLE` and `SERVO_MAX_ANGLE`)
- Method-specific parameters (number of bits, pulse width range, analog thresholds)

## CNC Machine Configuration

Configure your StepCraft CNC machine according to the method you've chosen:

1. **Binary Method**: Set digital outputs to represent the binary position
2. **Pulse Width Method**: Configure one output to generate pulses of varying width
3. **Analog Method**: Configure an analog output to provide a voltage proportional to the desired position

## Troubleshooting

- Check all connections
- Use the Serial Monitor (9600 baud) to debug
- Ensure your CNC is correctly outputting the expected signals
- Verify your servo's pulse width requirements match the configured values

## Advanced Usage

- For more positions with the binary method, you could use shift registers to expand the number of inputs
- For more precision with the analog method, you could use an external ADC
- For bidirectional communication, consider implementing a serial protocol 