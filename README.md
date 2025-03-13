# StepCraft CNC to Servo Interface

This project provides an Arduino Nano-based interface between a StepCraft CNC machine and a PWM-controlled servo motor using the binary method.

## Hardware Requirements

- Arduino Nano
- PWM-compatible servo motor
- Jumper wires
- Breadboard (optional)
- 5V power supply (if servo requires more power than Arduino can provide)

## Binary Method Implementation

The binary method uses multiple digital outputs from the CNC to create a binary number representing the desired servo position.

### Wiring:
- CNC Output 1 → Arduino Pin 2 (LSB)
- CNC Output 2 → Arduino Pin 3
- CNC Output 3 → Arduino Pin 4
- CNC Output 4 → Arduino Pin 5 (MSB)
- Servo signal → Arduino Pin 9
- Servo power → 5V
- Servo ground → GND

### How It Works:
The sketch reads up to 4 digital inputs from the CNC machine, treating them as a binary number:
- With 2 bits: 4 possible positions (0-3)
- With 3 bits: 8 possible positions (0-7)
- With 4 bits: 16 possible positions (0-15)

The binary value is mapped to the servo's angle range (0-90 degrees by default).

## Configuration

The Arduino sketch has configurable parameters that you can adjust:

- Servo pulse width range (`SERVO_MIN_PULSE` and `SERVO_MAX_PULSE`)
- Servo angle range (`SERVO_MIN_ANGLE` and `SERVO_MAX_ANGLE`)
- Number of bits to use (`NUM_POSITION_BITS`)

## CNC Machine Configuration

Configure your StepCraft CNC machine to control the servo:

1. Configure 4 digital outputs in UCCNC
2. Use M-code commands in your G-code to set the binary pattern:
   - `M62 P[n]` - Turn ON output n
   - `M63 P[n]` - Turn OFF output n

### Binary Position Reference

| Decimal | Binary | Approx. Angle |
|---------|--------|---------------|
| 0       | 0000   | 0°            |
| 1       | 0001   | 6°            |
| 2       | 0010   | 12°           |
| 3       | 0011   | 18°           |
| 4       | 0100   | 24°           |
| 5       | 0101   | 30°           |
| 6       | 0110   | 36°           |
| 7       | 0111   | 42°           |
| 8       | 1000   | 48°           |
| 9       | 1001   | 54°           |
| 10      | 1010   | 60°           |
| 11      | 1011   | 66°           |
| 12      | 1100   | 72°           |
| 13      | 1101   | 78°           |
| 14      | 1110   | 84°           |
| 15      | 1111   | 90°           |

## Example G-Code Files

This repository includes several example files:

1. **ServoControl_Binary_Example.nc**: Basic example showing how to set different servo positions
2. **Practical_CNC_Servo_Example.nc**: More complex example showing practical applications
3. **servo_pos_45.txt** and **servo_pos_90.txt**: Example UCCNC macros for common positions

## Troubleshooting

- Check all connections
- Use the Serial Monitor (9600 baud) to debug
- Ensure your CNC is correctly outputting the expected signals
- Verify your servo's pulse width requirements match the configured values
- Add a short dwell (G4) after changing outputs to give the servo time to move

## Advanced Usage

- For more positions, you could use shift registers to expand the number of inputs
- Create macros for common positions to simplify your G-code
- Use the binary pattern to control multiple servos with careful pin assignments 