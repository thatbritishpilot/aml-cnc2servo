#!/usr/bin/env python3
"""
Servo Tester - Web Interface for Arduino Nano CNC-to-Servo Controller
This script provides a web interface to test the binary input servo controller.
"""

import serial
import serial.tools.list_ports
import time
import webbrowser
from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Global variables
arduino = None
connected = False
current_position = 0
is_vibrating = False
vibration_start_time = 0

def find_arduino():
    """Find the Arduino Nano by listing all available serial ports"""
    ports = list(serial.tools.list_ports.comports())
    for port in ports:
        # Look for Arduino Nano or similar device
        if "Arduino" in port.description or "CH340" in port.description or "USB Serial" in port.description:
            return port.device
    return None

def connect_to_arduino(port):
    """Connect to the Arduino on the specified port"""
    global arduino, connected
    try:
        arduino = serial.Serial(port, 9600, timeout=1)
        time.sleep(2)  # Wait for Arduino to reset
        connected = True
        return True
    except Exception as e:
        print(f"Error connecting to Arduino: {e}")
        connected = False
        return False

def set_binary_position(position):
    """Send a command to the Arduino to set the binary position"""
    global arduino, connected, current_position, is_vibrating, vibration_start_time
    
    if not connected or arduino is None:
        return False
    
    try:
        # Convert position to binary string (4 bits)
        binary = format(position, '04b')
        
        # Send command to Arduino
        command = f"B{binary}\n"
        arduino.write(command.encode())
        
        # Wait for response
        time.sleep(0.1)
        response = arduino.readline().decode().strip()
        
        if response.startswith("OK"):
            current_position = position
            is_vibrating = True
            vibration_start_time = time.time()
            return True
        else:
            return False
    except Exception as e:
        print(f"Error sending command: {e}")
        return False

@app.route('/')
def index():
    """Render the main web interface"""
    return render_template('index.html')

@app.route('/connect', methods=['POST'])
def connect():
    """Connect to the Arduino"""
    port = request.form.get('port')
    if port == 'auto':
        port = find_arduino()
        if port is None:
            return jsonify({'success': False, 'message': 'Arduino not found'})
    
    success = connect_to_arduino(port)
    return jsonify({'success': success, 'port': port})

@app.route('/disconnect', methods=['POST'])
def disconnect():
    """Disconnect from the Arduino"""
    global arduino, connected
    if connected and arduino is not None:
        arduino.close()
    connected = False
    return jsonify({'success': True})

@app.route('/ports', methods=['GET'])
def get_ports():
    """Get a list of available serial ports"""
    ports = []
    for port in serial.tools.list_ports.comports():
        ports.append({
            'device': port.device,
            'description': port.description
        })
    return jsonify({'ports': ports})

@app.route('/set_position', methods=['POST'])
def set_position():
    """Set the servo position"""
    position = int(request.form.get('position'))
    success = set_binary_position(position)
    return jsonify({
        'success': success, 
        'position': position,
        'is_vibrating': is_vibrating
    })

@app.route('/status', methods=['GET'])
def status():
    """Get the current status"""
    global is_vibrating, vibration_start_time
    
    # Check if vibration should end (after 5 seconds)
    if is_vibrating and time.time() - vibration_start_time > 5:
        is_vibrating = False
    
    return jsonify({
        'connected': connected,
        'position': current_position,
        'is_vibrating': is_vibrating
    })

def create_templates():
    """Create the HTML template file"""
    import os
    
    if not os.path.exists('templates'):
        os.makedirs('templates')
    
    with open('templates/index.html', 'w', encoding='utf-8') as f:
        f.write("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Servo Tester</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 0 auto;
            padding: 20px;
        }
        .container {
            display: flex;
            flex-direction: column;
            gap: 20px;
        }
        .card {
            border: 1px solid #ccc;
            border-radius: 5px;
            padding: 20px;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        .controls {
            display: flex;
            flex-wrap: wrap;
            gap: 10px;
        }
        button {
            padding: 10px 15px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        button:hover {
            background-color: #45a049;
        }
        button:disabled {
            background-color: #cccccc;
            cursor: not-allowed;
        }
        select, input {
            padding: 8px;
            border-radius: 4px;
            border: 1px solid #ccc;
        }
        .status {
            display: flex;
            align-items: center;
            gap: 10px;
        }
        .status-indicator {
            width: 15px;
            height: 15px;
            border-radius: 50%;
            background-color: #ff0000;
        }
        .status-indicator.connected {
            background-color: #00ff00;
        }
        .binary-display {
            display: flex;
            gap: 5px;
            margin: 10px 0;
        }
        .bit {
            width: 30px;
            height: 30px;
            display: flex;
            align-items: center;
            justify-content: center;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-weight: bold;
        }
        .bit.active {
            background-color: #4CAF50;
            color: white;
        }
        .position-buttons {
            display: grid;
            grid-template-columns: repeat(4, 1fr);
            gap: 10px;
            margin-top: 20px;
        }
        .slider-container {
            margin-top: 20px;
        }
        .angle-display {
            font-size: 24px;
            font-weight: bold;
            text-align: center;
            margin: 20px 0;
        }
        .servo-visual {
            position: relative;
            width: 200px;
            height: 200px;
            margin: 20px auto;
        }
        .servo-base {
            position: absolute;
            width: 100px;
            height: 100px;
            background-color: #888;
            border-radius: 50%;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
        }
        .servo-arm {
            position: absolute;
            width: 10px;
            height: 100px;
            background-color: #333;
            top: 50%;
            left: 50%;
            transform-origin: bottom center;
            transform: translate(-50%, -100%) rotate(0deg);
            transition: transform 0.3s ease;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Servo Tester for CNC-to-Servo Controller</h1>
        
        <div class="card">
            <h2>Connection</h2>
            <div class="controls">
                <select id="port-select">
                    <option value="auto">Auto-detect</option>
                </select>
                <button id="refresh-ports">Refresh Ports</button>
                <button id="connect">Connect</button>
                <button id="disconnect" disabled>Disconnect</button>
            </div>
            <div class="status">
                <div id="status-indicator" class="status-indicator"></div>
                <span id="status-text">Disconnected</span>
            </div>
        </div>
        
        <div class="card">
            <h2>Binary Control</h2>
            <div class="binary-display">
                <div class="bit" id="bit3">0</div>
                <div class="bit" id="bit2">0</div>
                <div class="bit" id="bit1">0</div>
                <div class="bit" id="bit0">0</div>
            </div>
            <div class="controls">
                <button id="bit3-toggle">Toggle MSB</button>
                <button id="bit2-toggle">Toggle Bit 2</button>
                <button id="bit1-toggle">Toggle Bit 1</button>
                <button id="bit0-toggle">Toggle LSB</button>
            </div>
            <div class="position-buttons">
                <button class="pos-button" data-position="0">0</button>
                <button class="pos-button" data-position="1">1</button>
                <button class="pos-button" data-position="2">2</button>
                <button class="pos-button" data-position="3">3</button>
                <button class="pos-button" data-position="4">4</button>
                <button class="pos-button" data-position="5">5</button>
                <button class="pos-button" data-position="6">6</button>
                <button class="pos-button" data-position="7">7</button>
                <button class="pos-button" data-position="8">8</button>
                <button class="pos-button" data-position="9">9</button>
                <button class="pos-button" data-position="10">10</button>
                <button class="pos-button" data-position="11">11</button>
                <button class="pos-button" data-position="12">12</button>
                <button class="pos-button" data-position="13">13</button>
                <button class="pos-button" data-position="14">14</button>
                <button class="pos-button" data-position="15">15</button>
            </div>
            <div class="slider-container">
                <input type="range" id="position-slider" min="0" max="15" value="0" step="1" style="width: 100%;">
            </div>
        </div>
        
        <div class="card">
            <h2>Servo Visualization</h2>
            <div class="angle-display">
                Position: <span id="position-display">0</span> | Angle: <span id="angle-display">0°</span>
            </div>
            <div class="servo-visual">
                <div class="servo-base"></div>
                <div class="servo-arm" id="servo-arm"></div>
            </div>
        </div>
    </div>

    <script>
        // Global variables
        let connected = false;
        let currentPosition = 0;
        
        // DOM elements
        const portSelect = document.getElementById('port-select');
        const refreshPortsButton = document.getElementById('refresh-ports');
        const connectButton = document.getElementById('connect');
        const disconnectButton = document.getElementById('disconnect');
        const statusIndicator = document.getElementById('status-indicator');
        const statusText = document.getElementById('status-text');
        const bits = [
            document.getElementById('bit0'),
            document.getElementById('bit1'),
            document.getElementById('bit2'),
            document.getElementById('bit3')
        ];
        const bitToggles = [
            document.getElementById('bit0-toggle'),
            document.getElementById('bit1-toggle'),
            document.getElementById('bit2-toggle'),
            document.getElementById('bit3-toggle')
        ];
        const posButtons = document.querySelectorAll('.pos-button');
        const positionSlider = document.getElementById('position-slider');
        const positionDisplay = document.getElementById('position-display');
        const angleDisplay = document.getElementById('angle-display');
        const servoArm = document.getElementById('servo-arm');
        
        // Functions
        async function refreshPorts() {
            try {
                const response = await fetch('/ports');
                const data = await response.json();
                
                // Clear existing options except Auto-detect
                while (portSelect.options.length > 1) {
                    portSelect.remove(1);
                }
                
                // Add new options
                data.ports.forEach(port => {
                    const option = document.createElement('option');
                    option.value = port.device;
                    option.textContent = `${port.device} - ${port.description}`;
                    portSelect.appendChild(option);
                });
            } catch (error) {
                console.error('Error refreshing ports:', error);
            }
        }
        
        async function connect() {
            try {
                const port = portSelect.value;
                const response = await fetch('/connect', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `port=${port}`
                });
                
                const data = await response.json();
                
                if (data.success) {
                    connected = true;
                    statusIndicator.classList.add('connected');
                    statusText.textContent = `Connected to ${data.port}`;
                    connectButton.disabled = true;
                    disconnectButton.disabled = false;
                    enableControls(true);
                } else {
                    statusText.textContent = data.message || 'Connection failed';
                }
            } catch (error) {
                console.error('Error connecting:', error);
                statusText.textContent = 'Connection error';
            }
        }
        
        async function disconnect() {
            try {
                await fetch('/disconnect', { method: 'POST' });
                connected = false;
                statusIndicator.classList.remove('connected');
                statusText.textContent = 'Disconnected';
                connectButton.disabled = false;
                disconnectButton.disabled = true;
                enableControls(false);
            } catch (error) {
                console.error('Error disconnecting:', error);
            }
        }
        
        function enableControls(enabled) {
            bitToggles.forEach(button => {
                button.disabled = !enabled;
            });
            
            posButtons.forEach(button => {
                button.disabled = !enabled;
            });
            
            positionSlider.disabled = !enabled;
        }
        
        function updateBitDisplay(position) {
            const binary = position.toString(2).padStart(4, '0');
            
            for (let i = 0; i < 4; i++) {
                const bitValue = binary[3 - i] === '1';
                bits[i].textContent = bitValue ? '1' : '0';
                if (bitValue) {
                    bits[i].classList.add('active');
                } else {
                    bits[i].classList.remove('active');
                }
            }
        }
        
        function updateAngleDisplay(position) {
            // Calculate angle (0-90 degrees)
            const angle = Math.round(position * (90 / 15));
            angleDisplay.textContent = `${angle}°`;
            positionDisplay.textContent = position;
            
            // Update servo visualization
            servoArm.style.transform = `translate(-50%, -100%) rotate(${angle}deg)`;
        }
        
        async function setPosition(position) {
            if (!connected) return;
            
            try {
                const response = await fetch('/set_position', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/x-www-form-urlencoded',
                    },
                    body: `position=${position}`
                });
                
                const data = await response.json();
                
                if (data.success) {
                    currentPosition = position;
                    updateBitDisplay(position);
                    updateAngleDisplay(position);
                }
            } catch (error) {
                console.error('Error setting position:', error);
            }
        }
        
        // Event listeners
        refreshPortsButton.addEventListener('click', refreshPorts);
        connectButton.addEventListener('click', connect);
        disconnectButton.addEventListener('click', disconnect);
        
        // Bit toggle buttons
        bitToggles.forEach((button, index) => {
            button.addEventListener('click', () => {
                const bitIndex = 3 - index;
                const bitValue = 1 << bitIndex;
                const newPosition = currentPosition ^ bitValue;
                setPosition(newPosition);
            });
        });
        
        // Position buttons
        posButtons.forEach(button => {
            button.addEventListener('click', () => {
                const position = parseInt(button.dataset.position);
                setPosition(position);
                positionSlider.value = position;
            });
        });
        
        // Position slider
        positionSlider.addEventListener('input', () => {
            const position = parseInt(positionSlider.value);
            setPosition(position);
        });
        
        // Initialize
        document.addEventListener('DOMContentLoaded', () => {
            refreshPorts();
            enableControls(false);
            updateBitDisplay(0);
            updateAngleDisplay(0);
        });
    </script>
</body>
</html>""")

if __name__ == '__main__':
    # Create template files
    create_templates()
    
    # Open browser
    webbrowser.open('http://127.0.0.1:5000')
    
    # Start Flask app
    app.run(debug=True) 