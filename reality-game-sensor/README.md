# Usage

## Sensor
This directory contains several sensor files, including pure data sensor, wifi data sensor, and BLE data sensor.

## Wifi Sensor
You have to modify several files to enable wifi connection.
- In mbed_app.json, you have to modify the `wifi-ssid` and `wifi-password` field
- In Sensor/my_wifi_sensor.h, you have to modify the macro `IP_ADDRESS` and `PORT`, these are the host ip address and port you have to connect with.

## Game server
The game project file is located at ../game/project
- Type `python treasure_hunt` to activate the game.
- The port specified in the `start()` function in the game class should be identical to the configuration at the motion sensor.