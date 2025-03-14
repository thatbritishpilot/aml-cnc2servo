# CNC to Servo Interface Tester

This project provides a web interface for testing a servo motor that interfaces with CNC machine binary outputs. It includes an Arduino sketch for controlling the servo and a Python web application for easy testing and control.

## Requirements

### Hardware
- Arduino Nano or compatible board
- Servo motor (tested with ReadyTOSky 12kg servo)
- USB cable for Arduino connection

### Software
- Python 3.6 or higher
- Arduino IDE
- Required Python packages:
  ```
  flask
  pyserial
  ```

## Setup Instructions

1. **Install Dependencies**
   ```bash
   pip install flask pyserial
   ```

2. **Upload Arduino Code**
   - Open `ServoTesterArduino/ServoTesterArduino.ino` in the Arduino IDE
   - Select your Arduino board and port
   - Upload the sketch to your Arduino

3. **Connect Hardware**
   - Connect the servo to the Arduino:
     - Signal wire to pin 9 (PWM capable)
     - Power and ground to appropriate pins
   - Connect the Arduino to your computer via USB

## Running the Web Interface

1. **Start the Python Server**
   ```bash
   python servo_tester.py
   ```
   The web interface will be available at `http://localhost:5000`

2. **Using the Interface**
   - Select the correct COM port from the dropdown
   - Click "Connect" to establish communication with the Arduino
   - Use the position buttons (0-15) to test different servo positions
   - The servo will:
     - Move smoothly to the selected position
     - Vibrate briefly to indicate arrival
     - Display current position and status in the web interface

3. **Features**
   - Smooth servo movement with PWM control
   - Visual feedback of servo position
   - Vibration effect on position change
   - Real-time status updates
   - Binary position control (4-bit, 16 positions)

## Troubleshooting

1. **Permission Errors**
   - If you get "Access is denied" when connecting:
     - Close any other applications using the COM port
     - Try unplugging and reconnecting the Arduino
     - Check if you have necessary permissions to access the port

2. **Connection Issues**
   - Verify the correct COM port is selected
   - Ensure the Arduino is properly connected
   - Check if the Arduino sketch is uploaded successfully

3. **Servo Movement**
   - Verify servo power supply is adequate
   - Check servo signal wire connection to pin 9
   - Ensure servo is not mechanically blocked

## Development

The project consists of three main components:
- `servo_tester.py`: Python Flask web server
- `ServoTesterArduino/ServoTesterArduino.ino`: Arduino control sketch
- `templates/index.html`: Web interface template

## License

This project is open source and available under the MIT License. 