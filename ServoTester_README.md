# Servo Tester for StepCraft CNC to Servo Interface

This is a testing tool for the StepCraft CNC to Servo Interface. It provides a web interface to control the servo directly from your computer via USB, simulating the binary inputs that would come from your CNC machine.

## Setup Instructions

### 1. Arduino Setup

1. Connect your Arduino Nano to your computer via USB
2. Upload the `ServoTesterArduino.ino` sketch to your Arduino Nano
3. Connect your servo to the Arduino:
   - Servo signal wire → Arduino Pin 9
   - Servo power (red) → 5V
   - Servo ground (black/brown) → GND

### 2. Python Setup

1. Install Python 3.6 or higher if not already installed
2. Install the required Python packages:
   ```
   pip install -r requirements.txt
   ```

## Running the Servo Tester

1. Run the Python script:
   ```
   python servo_tester.py
   ```
2. A web browser should automatically open to the servo tester interface
3. If the browser doesn't open automatically, navigate to http://127.0.0.1:5000

## Using the Web Interface

### Connection

1. Select your Arduino's serial port from the dropdown menu (or use "Auto-detect")
2. Click "Connect"
3. Once connected, the status indicator will turn green

### Controlling the Servo

You can control the servo in several ways:

1. **Binary Bits**: Click the individual bit toggle buttons to set specific bits
2. **Position Buttons**: Click any of the numbered buttons (0-15) to set the servo to a specific position
3. **Slider**: Use the slider to smoothly move through all 16 positions

The interface shows:
- The current binary pattern (0000 to 1111)
- The position number (0 to 15)
- The corresponding angle (0° to 90°)
- A visual representation of the servo position

## How It Works

The system works as follows:

1. The web interface sends binary commands to the Arduino via USB
2. The Arduino sets its output pins (2, 3, 4, 5) to match the binary pattern
3. These pins are connected to the same pins that would normally receive input from the CNC
4. The Arduino also directly controls the servo based on the binary position

This allows you to test the full functionality of your servo setup without needing to connect it to your CNC machine.

## Troubleshooting

- **Can't connect to Arduino**: Make sure the Arduino is properly connected and the correct port is selected
- **Servo not moving**: Check all connections and verify the servo is properly powered
- **Incorrect servo movement**: Verify the servo parameters in the Arduino sketch match your servo's specifications 