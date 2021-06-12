## Add sliding window to accelerometer and gyro

### Sensor class
- Calibration.
- Update sensor values.
- Update standard values.
- Detect motion type.

### Accelerometer
- Set sliding window number to 20.
- Buffer the current accelerometer value with the previous one.
- Compute the root-mean-square of the buffer values.
- Also keep track of the current amplitude.

### Gyro
- Compute the summation (in place of integral) to get the range of rotation (angle[i]).
- Compare the 3D rotation range values with the previous ones and store in buffer.
- Compute the root-mean-square of the buffer values to determine the rotation axis.
- Sign of angle indicates clockwise or counter-clockwise.

### Motion detect
- Use standard value of angle[1] to tell twisting.
- Use stm-diff to tell intensity of motion, group (stand) (run) (walk/raise/punch)
- Use stm-all and stm-y to distinguish between raise and punch.

### EventQueue
- Calls sensor update every TIMESTEP interval (1ms)
- Calculate std values motion type every STD-TIMESTEP interval (20ms)
- Sends motion type to server every 0.1s

### User Button
- Button thread.
- Long press (>2s) triggers calibration.

### Problem
- Delay in motion update
