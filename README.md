# 2021eslab_final

## Progress

### 5/2 Eric
- Setup project directory structure
- Refactor sensor, wifi, and BLE code
- Separate sensor, wifi, and BLE code to .h and .cpp files
- Combine and Test whether these services can still work

### 5/3 Eric
- Fix wifi program decode error (modify minimal_printf to std)
- Collect some data about several motion patterns
- Draw figures based on above data
- Add wifi server and BLE programs to this repo

### 5/6 Emily
- Add sensor sliding window method
- Add calibration method
- Midterm Proposal: https://docs.google.com/presentation/d/1zUISQAgCSKkXEW6G_4c_JL1AxVgvxVhpq2gYIAT18C8/edit?usp=sharing

### 5/8 Eric
- Refactor sensor code from Emily to "my_sensor.h" and "my_sensor.cpp" files
- Wrap codes into a "DataSensor" class and break some functions into smaller pieces
- Add EventQueue to the sensor class
- Add long press features in the class
- Modify Calibration methods (need to be reconfirmed)
- Write a "WifiDataSensor" class based on "DataSensor", not complete

### 5/9 Eric
- Modify printStd and printSensorValue in DataSensor class
- Create two event for updateSensor and updateStd with different timestep
- Complete Wifi Sensor wrapper class
- There are 3 threads in the program: main thread, sensor thread, and event thread
- Separate Wifi send data process into: connectWifi, connectHost, and send_sensor_data
- Able to connect to python program 
- Add a test server (wifi-test.py) under connection folder
- Collect 10 records instead of a single record to improve efficiency

### 5/10, 5/11 Eric
- Add BLE Tag Service
- Refactor BLE code, separate BLE characteristics apart
- Design a little bit about tag service

### 5/21 Emily
- Distinguish between standing, walking, running, and jumping
- Increase calibration time and decrease update motion frequency
- Bug: getSensorTypeWifi() cannot return char* (replaced by int instead)

### 5/24 Eric
- Add BLE Motion Service
- It it a wrapper of data sensor, one can connect the "MotionService" to get data
- I just write a template, not yet integrated with the true motion value

### 6/1 Emily
- Still working on motion detection
- Quick demo video: https://youtu.be/3e2XkRUkQSk
