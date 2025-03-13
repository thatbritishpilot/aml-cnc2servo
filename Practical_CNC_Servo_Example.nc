(Practical CNC Operation with Servo Control)
(For StepCraft D420 with UCCNC)
(Uses binary method to control servo position)
(Servo range limited to 0-90 degrees)

(Program setup)
G21         (Set units to mm)
G90         (Set absolute positioning)
G17         (Select XY plane)
G54         (Use work coordinate system 1)
G0 Z10      (Move Z to safe height)

(Initialize servo to position 0 - fully retracted)
M63 P0      (Set output 0 LOW - LSB)
M63 P1      (Set output 1 LOW)
M63 P2      (Set output 2 LOW)
M63 P3      (Set output 3 LOW - MSB)

(--- Operation: Pocket milling with servo-controlled depth gauge ---)

(Move to starting position)
G0 X10 Y10 F3000

(First pocket - normal depth)
G1 Z-5 F500                 (Plunge into material)
G1 X50 Y10 F1000            (Cut along X axis)
G1 X50 Y50                  (Cut along Y axis)
G1 X10 Y50                  (Cut along X axis)
G1 X10 Y10                  (Cut along Y axis to complete pocket)
G0 Z10                      (Retract Z)

(Move to second pocket location)
G0 X70 Y10

(Deploy depth gauge - set servo to position 8 (binary 1000 = ~48 degrees))
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M63 P2                      (Set output 2 LOW)
M62 P3                      (Set output 3 HIGH - MSB)
G4 P1                       (Dwell for 1 second to allow servo to move)

(Second pocket - with depth gauge deployed)
G1 Z-5 F500                 (Plunge into material)
G1 X110 Y10 F1000           (Cut along X axis)
G1 X110 Y50                 (Cut along Y axis)
G1 X70 Y50                  (Cut along X axis)
G1 X70 Y10                  (Cut along Y axis to complete pocket)
G0 Z10                      (Retract Z)

(Retract depth gauge - set servo to position 0)
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M63 P2                      (Set output 2 LOW)
M63 P3                      (Set output 3 LOW - MSB)
G4 P1                       (Dwell for 1 second to allow servo to move)

(--- Operation: Contour cutting with servo-controlled tool angle ---)

(Move to starting position for contour)
G0 X10 Y70

(Set tool angle to 24 degrees - servo position 4 (binary 0100))
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M62 P2                      (Set output 2 HIGH)
M63 P3                      (Set output 3 LOW - MSB)
G4 P1                       (Dwell for 1 second to allow servo to move)

(Cut first contour at 24 degrees)
G1 Z-3 F500                 (Plunge into material)
G1 X60 Y70 F1000            (Cut along X axis)
G0 Z10                      (Retract Z)

(Move to second contour position)
G0 X10 Y90

(Set tool angle to 48 degrees - servo position 8 (binary 1000))
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M63 P2                      (Set output 2 LOW)
M62 P3                      (Set output 3 HIGH - MSB)
G4 P1                       (Dwell for 1 second to allow servo to move)

(Cut second contour at 48 degrees)
G1 Z-3 F500                 (Plunge into material)
G1 X60 Y90 F1000            (Cut along X axis)
G0 Z10                      (Retract Z)

(Move to third contour position)
G0 X10 Y110

(Set tool angle to 72 degrees - servo position 12 (binary 1100))
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M62 P2                      (Set output 2 HIGH)
M62 P3                      (Set output 3 HIGH - MSB)
G4 P1                       (Dwell for 1 second to allow servo to move)

(Cut third contour at 72 degrees)
G1 Z-3 F500                 (Plunge into material)
G1 X60 Y110 F1000           (Cut along X axis)
G0 Z10                      (Retract Z)

(Return to home position and reset servo to position 0)
G0 X10 Y10                  (Return to home position)
M63 P0                      (Set output 0 LOW - LSB)
M63 P1                      (Set output 1 LOW)
M63 P2                      (Set output 2 LOW)
M63 P3                      (Set output 3 LOW - MSB)

(End program)
M30 