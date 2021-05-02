/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"

// sensor module header
#include "Sensor/sensor.h"

// wifi module header
#include "Wifi/wifi.h"

// BLE module header

// handle console ouput
static BufferedSerial serial_port(USBTX, USBRX);
FileHandle *mbed::mbed_override_console(int fd)
{
    return &serial_port; 
}

// I/O Devices
DigitalOut led(LED1);


int main() 
{
    // wifi variables
    int count = 0;

    // printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2); 
    
    if (ret != 0) {
        printf("\nConnection error\n");
        return -1; 
    }

    printf("Success\n\n");
    print_wifi_info(wifi);

    send_sensor_data(&wifi);
    printf("sensor data complete");
    wifi.disconnect();
    printf("\nDone\n"); 
}