# The Motion Sensor

## The Data Sensor

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

## WiFi wrapper
This is a class that wraps over the data sensor class. That is, we apply composition instead of inheritance.

The WiFi wrapper will call a method exposed by the inner data sensor to get the motion type. Then the WiFi wrapper will send the motion type to the game server for further processing.

Most of the WiFi configuration is recorded in the macro in the header file like `IP_ADDRESS` and `PORT`. Make sure to modity these value to connect to your game server.

## BLE Tag
This class is a simple BLE advertiser that users can connect to get some information. 
The tag information contains:
- event id: The id of the event.
- event level: The hardness of the event.
- event item: Item that can be collected if the user pass the challenge.
- event challenger: The previous winner of the event.
- event sign in: The button signal.
- event signal: The LED control signal.

This class is completed, so you can directly use this class for further development. In our vision, we consider the BLE tag as an information provider. All actions actually are determined in the game program given the tag information. In this model, we can easily change the configuration of the game by only modifying the tag information of each IoT board.

## BLE wrapper
The BLE wrapper is not completed because of hardware problems. However, this part of code is almost complete, you can reference the WiFi wrapper for more information or [my homework file](https://github.com/woodcutter-eric/embedded-system-lab/blob/main/BLE_GattServer_CharacteristicUpdates/source/main.cpp).