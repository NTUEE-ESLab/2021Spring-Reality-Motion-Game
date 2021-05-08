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