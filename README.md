# 2021eslab_final

## Setup

This is an example of a Wi-Fi application using the Wi-Fi and network socket APIs that [Mbed OS](https://github.com/ARMmbed/mbed-os) provides.

The program brings up the Wi-Fi and the underlying network interface and uses it to scan available networks, connects to a network, prints interface and connection details and performs an HTTP operation.

For more information about Wi-Fi APIs, please visit the [Mbed OS Wi-Fi](https://os.mbed.com/docs/latest/reference/wi-fi.html) documentation.

### Supported hardware ###

* [NUCLEO-F401RE](https://os.mbed.com/platforms/ST-Nucleo-F401RE/) with [X-NUCLEO-IDW04A1](http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idw04a1.html) Wi-Fi expansion board using pins D8 and D2 _(of the Arduino connector)_.
* [NUCLEO-F401RE](https://os.mbed.com/platforms/ST-Nucleo-F401RE/) with [X-NUCLEO-IDW01M1](https://os.mbed.com/components/X-NUCLEO-IDW01M1/) Wi-Fi expansion board using pins PA_9 and PA_10 _(of the Morpho connector)_.
* [DISCO_L475VG_IOT01A](https://os.mbed.com/platforms/ST-Discovery-L475E-IOT01A/) with ISM43362 built-in module
* [DISCO_F413ZH](https://os.mbed.com/platforms/ST-Discovery-F413H/) with ISM43362 built-in module
* Other Mbed targets with  [X-NUCLEO-IDW04A1](http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idw04a1.html) or [X-NUCLEO-IDW01M1](https://os.mbed.com/components/X-NUCLEO-IDW01M1/) expansion board.
  *(The Mbed target board the Wi-Fi shield connects to shouldn't have any other network interface, for example Ethernet.)*

#### Connecting the X-NUCLEO-IDW0XX1 ####

To connect the [X-NUCLEO-IDW04A1](http://www.st.com/content/st_com/en/products/ecosystems/stm32-open-development-environment/stm32-nucleo-expansion-boards/stm32-ode-connect-hw/x-nucleo-idw04a1.html) or [X-NUCLEO-IDW01M1](https://developer.mbed.org/components/X-NUCLEO-IDW01M1/) expansion board to your NUCLEO development board, plug the expansion board on top of the NUCLEO board using the Arduino or Morpho connector.

##  Getting started ##

1. Import the example.

   ```
   mbed import mbed-os-example-wifi
   cd mbed-os-example-wifi
   ```
   
2. Configure the Wi-Fi shield to use.

   Edit ```mbed_app.json``` to include the correct Wi-Fi shield, SSID and password:

   ```
       "config": {
 	  "wifi-shield": {
               "help": "Options are WIFI_IDW0XX1",
               "value": "WIFI_IDW0XX1"
        	  },
           "wifi-ssid": {
               "help": "WiFi SSID",
               "value": "\"SSID\""
           },
           "wifi-password": {
               "help": "WiFi Password",
               "value": "\"Password\""
           }
       },
   ```

   Sample ```mbed_app.json``` files are provided for X-NUCLEO-IDW04A1 (```mbed_app_idw04a1.json```),  X-NUCLEO-IDW01M1 (```mbed_app_idw01m1```), DISCO_L475VG_IOT01A/ISM43362 (```mbed_app_ism43362.json```) and DISCO_F413ZH/ISM43362 (```mbed_app_ism43362.json```)
   
   For built-in Wi-Fi, ignore the value of `wifi-shield`.

3. Compile and generate binary.

   For example, for `GCC`:

   ```
   mbed compile -t GCC_ARM -m UBLOX_EVK_ODIN_W2
   ```
   
 4. Open a serial console session with the target platform using the following parameters:
 
    * **Baud rate:** 9600
    * **Data bits:** 8
    * **Stop bits:** 1
    * **Parity:** None
 
 5. Copy or drag the application `mbed-os-example-wifi.bin` in the folder `mbed-os-example-wifi/BUILD/<TARGET NAME>/<PLATFORM NAME>` onto the target board.
 
 6. The serial console should display a similar output to below, indicating a successful Wi-Fi connection:
 
 ```
 WiFi example

Scan:
Network: Dave Hot Spot secured: Unknown BSSID: 00:01:02:03:04:05 RSSI: -58 Ch: 1
1 network available.

Connecting...
Success

MAC: 00:01:02:03:04:05
IP: 192.168.0.5
Netmask: 255.255.255.0
Gateway: 192.168.0.1
RSSI: -27

Sending HTTP request to www.arm.com...
sent 38 [GET / HTTP/1.1]
recv 64 [HTTP/1.1 301 Moved Permanently]

Done
```

## Troubleshooting

If you have problems, you can review the [documentation](https://os.mbed.com/docs/latest/tutorials/debugging.html) for suggestions on what could be wrong and how to fix it.


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

### Progress Report 
https://docs.google.com/presentation/d/1QpmcDUexZokhLhzRx4198VfW1-WgqPi4YffoX4_mxNY/edit?usp=sharing

### 6/7 Eric
- Write a game template under game/project, type `python treasure_hunt` to activate the game
- Fix the reused port problem
- Integrate wifi with game
- Add another thread to listen from sensor to prevent blocking (I/O bound) to boose game speed

### 6/12 Emily
- Complete 7 motion types: stand, walk, run, raise, punch, right, left
- Right and left lags more
- Wifi send motion type to server (server-simple.py)
- Demo video: https://youtu.be/QfCRsLYa2cc

### Final Project Demo
https://youtu.be/kcvfzlNczBo
