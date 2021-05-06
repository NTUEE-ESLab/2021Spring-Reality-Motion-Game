## Add sliding window to accelerometer and gyro

### Accelerometer
- Choose sliding window number to 50.
- Compute the square-root-mean.

### Gyro
- Compare the three gyro values with the previous values.
- The value of angle shows how much turning in that direction is.
- Modify this part for future use.

### Sensor class
- Currently sets update to public function and calls it at TIMESTEP interval.
- Should put it to eventqueue in the future.
- Calibration added.

### Problem
- Error sending the data to python server.
