# UCCNC Binary Servo Control Setup Guide

This guide explains how to configure UCCNC on a StepCraft D420 to control a servo using the binary method with our Arduino interface.

## Hardware Setup

1. Connect your Arduino Nano to your computer and upload the `StepCraftCNCToServo.ino` sketch.

2. Connect the StepCraft D420 outputs to the Arduino:
   - StepCraft Output 1 → Arduino Pin 2 (LSB)
   - StepCraft Output 2 → Arduino Pin 3
   - StepCraft Output 3 → Arduino Pin 4
   - StepCraft Output 4 → Arduino Pin 5 (MSB)

3. Connect the servo to the Arduino:
   - Servo signal wire → Arduino Pin 9
   - Servo power (red) → 5V
   - Servo ground (black/brown) → GND

## UCCNC Configuration

### Configure Output Pins

1. Open UCCNC and go to **Configuration → I/O Setup**
2. Navigate to the **Outputs** tab
3. Configure the first 4 outputs:

   | Output # | Function | Pin # | Active Low | Description |
   |----------|----------|-------|------------|-------------|
   | Output 0 | General  | 1     | No         | Servo LSB (Bit 0) |
   | Output 1 | General  | 2     | No         | Servo Bit 1 |
   | Output 2 | General  | 3     | No         | Servo Bit 2 |
   | Output 3 | General  | 4     | No         | Servo MSB (Bit 3) |

   > Note: The actual pin numbers may vary depending on your StepCraft controller. Check your StepCraft documentation for the correct output pin numbers.

4. Click **OK** to save the configuration

### Testing the Outputs

1. Go to the **Diagnostics** screen in UCCNC
2. Find the **Outputs** section
3. Click on each output button to toggle it on/off
4. Verify that the servo moves to different positions as you change the binary pattern

## Using G-Code to Control the Servo

Use M-code commands in your G-code to control the servo position:

- `M62 P[n]` - Turn ON output n
- `M63 P[n]` - Turn OFF output n

### Binary Position Reference

| Decimal | Binary | Approx. Angle |
|---------|--------|---------------|
| 0       | 0000   | 0°            |
| 1       | 0001   | 12°           |
| 2       | 0010   | 24°           |
| 3       | 0011   | 36°           |
| 4       | 0100   | 48°           |
| 5       | 0101   | 60°           |
| 6       | 0110   | 72°           |
| 7       | 0111   | 84°           |
| 8       | 1000   | 96°           |
| 9       | 1001   | 108°          |
| 10      | 1010   | 120°          |
| 11      | 1011   | 132°          |
| 12      | 1100   | 144°          |
| 13      | 1101   | 156°          |
| 14      | 1110   | 168°          |
| 15      | 1111   | 180°          |

> Note: Actual angles may vary depending on your servo's specifications and the `SERVO_MIN_ANGLE` and `SERVO_MAX_ANGLE` settings in the Arduino sketch.

## Creating Macros for Common Positions (Optional)

To simplify your G-code, you can create macros for common servo positions:

1. Go to the UCCNC installation folder and find the `Macros` subfolder
2. Create a new text file for each position (e.g., `servo_pos_0.txt`, `servo_pos_90.txt`, etc.)

Example macro for position 5 (binary 0101 ≈ 60°):
```
(Servo Position 5 - approximately 60 degrees)
M62 P0      (Set output 0 HIGH - LSB)
M63 P1      (Set output 1 LOW)
M62 P2      (Set output 2 HIGH)
M63 P3      (Set output 3 LOW - MSB)
G4 P0.5     (Dwell for 0.5 seconds to allow servo to move)
```

Then in your G-code, you can simply call:
```
M1000       (Call macro 1000 - servo_pos_0.txt)
```

## Troubleshooting

- **Servo not moving**: Check all connections and verify that the outputs are toggling in the UCCNC Diagnostics screen
- **Incorrect positions**: Verify the binary pattern being sent and check the `NUM_POSITION_BITS` setting in the Arduino sketch
- **Erratic movement**: Add a short dwell (G4) after changing outputs to give the servo time to move 