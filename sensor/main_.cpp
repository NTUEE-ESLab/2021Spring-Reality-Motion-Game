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

#include "BUILD/DISCO_L475VG_IOT01A/ARMC6/mbed_config.h"
#include "EventQueue.h"
#include "ThisThread.h"
#include "mbed.h"
#include "TCPSocket.h"
#include "mbed_power_mgmt.h"
#include "BufferedBlockDevice.h"
#include "FileHandle.h"

// Sensors drivers present in the BSP library
#include "mbed_retarget.h"
// #include "stm32l475e_iot01_tsensor.h"
// #include "stm32l475e_iot01_hsensor.h"
// #include "stm32l475e_iot01_psensor.h"
// #include "stm32l475e_iot01_magneto.h"
#include "nsapi_types.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <cstdint>
#include <cstdio>

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);

#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif

static BufferedSerial serial_port(USBTX,USBRX);
FileHandle *mbed::mbed_override_console(int fd) {
    return &serial_port;
}

const char* HOST = "192.168.50.101";
const int PORT = 65432;

const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

#define TIMESTEP 50

static events::EventQueue event_queue(16 * EVENTS_EVENT_SIZE);

char* ret = new char[1024];

class Sensors {
#define BUFFER_SIZE 50
#define SCALE_MULTIPLIER 0.045
public: 
    Sensors(events::EventQueue &event_queue) :
        _event_queue(event_queue) {
            BSP_ACCELERO_Init();
            BSP_GYRO_Init();
            calibration();
            _event_queue.call_every(1, this, &Sensors::update);
        }
    
    void calibration() {
        printf("starting calibration... ");
        for (int i = 0; i < 3; i++) {
            GyroOffset[i] = 0; 
            AccOffset[i] = 0;
        }
        int _sample_num = 0;
        
        while (_sample_num < 500) {
            _sample_num++;
            BSP_GYRO_GetXYZ(pGyroDataXYZ);
            BSP_ACCELERO_AccGetXYZ(pDataXYZ);

            for (int i = 0; i < 3; i++) {
                GyroOffset[i] += pGyroDataXYZ[i];
                AccOffset[i] += pDataXYZ[i];
            }
            ThisThread::sleep_for(0.5);
        }

        for (int i = 0; i < 3; i++) {
            GyroOffset[i] /= _sample_num;
            AccOffset[i] /= _sample_num;
        }

        _sample_num = 0;
        printf("done!\n");
    }
    
    char* printSensorValue() {
        int n = sprintf(ret, "sensor{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
        printf("ACC %d, %d, %d  Gyro %.2f %.2f %.2f\n", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], pGyroDataXYZ[0]/1000, pGyroDataXYZ[1]/1000, pGyroDataXYZ[2]/1000);
        return ret;
    }

    char* printStd() {
        float stm_x = getStd(buffer_stm_x);
        float stm_y = getStd(buffer_stm_y);
        float stm_z = getStd(buffer_stm_z);
        float stm_val = getStd(buffer_stm);
        int n = sprintf(ret, "{stm \"ax\":%.2f,\"ay\":%.2f,\"az\":%.2f,\"stm\":%.2f  dir \"ang0\":%.0f,\"ang1\":%.0f,\"ang2\":%.0f\n}", stm_x, stm_y, stm_z, stm_val, angle[0], angle[1], angle[2]);
        // printf("acc x %.2f y %.2f z %.2f stm %.2f ang0 %.2f ang1 %.2f ang2 %.2f\n", stm_x, stm_y, stm_z, stm_val, angle[0], angle[1], angle[2]);
        return ret;
    }

    void update() {
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        for (int i = 0; i < 3; ++i) {
            pDataXYZ[i] = pDataXYZ[i] - AccOffset[i];
            pGyroDataXYZ[i] = pGyroDataXYZ[i] - GyroOffset[i];
        }

        buffer_stm_x[buffer_p] = (float)pDataXYZ[0];
        buffer_stm_y[buffer_p] = (float)pDataXYZ[1];
        buffer_stm_z[buffer_p] = (float)pDataXYZ[2];

        ThisThread::sleep_for(TIMESTEP);

        // relative directions
        for (int i = 0; i < 3; i++) {
            if (abs(pGyroDataXYZ[i]) * SCALE_MULTIPLIER > 50) {
                angle[i] += (pGyroDataXYZ[i] + pGyroDataXYZ_prev[i]) / 2 * TIMESTEP * SCALE_MULTIPLIER;
            }
            pGyroDataXYZ[i] = pGyroDataXYZ[i];
        }

        buffer_stm[buffer_p] = sqrt(pow((float)pDataXYZ[0],2) + pow((float)pDataXYZ[1],2) + pow((float)pDataXYZ[2],2));

        buffer_p = (buffer_p + 1) % BUFFER_SIZE;
    }

private: 
    events::EventQueue &_event_queue;
    int AccOffset[3] = {};          // calibration value
    float GyroOffset[3] = {};       // calibration value
    int16_t pDataXYZ[3] = {};
    float pGyroDataXYZ[3] = {};
    float pGyroDataXYZ_prev[3] = {};
    float angle[3] = {};

    // slinding window buffers
    int buffer_stm_x[BUFFER_SIZE] = {};
    int buffer_stm_y[BUFFER_SIZE] = {};
    int buffer_stm_z[BUFFER_SIZE] = {};
    int buffer_stm[BUFFER_SIZE] = {};

    // buffer pointer
    int buffer_p = 0;


    /* drag update to public
    void update() {
        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        BSP_GYRO_GetXYZ(pGyroDataXYZ);
        for (int i = 0; i < 3; ++i) {
            pDataXYZ[i] = pDataXYZ[i] - AccOffset[i];
            pGyroDataXYZ[i] = pGyroDataXYZ[i] - GyroOffset[i];
        }

        buffer_stm_x[buffer_p] = (float)pDataXYZ[0];
        buffer_stm_y[buffer_p] = (float)pDataXYZ[1];
        buffer_stm_z[buffer_p] = (float)pDataXYZ[2];

        ThisThread::sleep_for(TIMESTEP);
        buffer_stm[buffer_p] = sqrt(pow((float)pDataXYZ[0],2) + pow((float)pDataXYZ[1],2) + pow((float)pDataXYZ[2],2));

        buffer_p = (buffer_p + 1) % BUFFER_SIZE;
        printSensorValue();
        printStd();
    }
    */

    float getStd(int* buffer) {
        float sum = 0, mean = 0, std = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            sum += buffer[i];
        }
        mean = sum / BUFFER_SIZE;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            std += pow(buffer[i] - mean, 2);
        }
        return sqrt(std / BUFFER_SIZE);
    }

    float getAvg(int* buffer) {
        float sum = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            sum += buffer[i];
        }
        return sum / BUFFER_SIZE;
    }
};

// char* ret = new char[64];
char* get_coor() {
    // char* ret = new char[64];
    int16_t pDataXYZ[3] = {0};
    float gDataXYZ[3] = {0};
    printf("Start sensor init\n");
    BSP_ACCELERO_Init();
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
    printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
    printf("ACCELERO_Z = %d\n", pDataXYZ[2]);
    printf("GYRO_X = %.2f\n", gDataXYZ[0]/1000);
    printf("GYRO_Y = %.2f\n", gDataXYZ[1]/1000);
    printf("GYRO_Z = %.2f\n", gDataXYZ[2]/1000);
    BSP_GYRO_Init();
    BSP_GYRO_GetXYZ(gDataXYZ);
    int n = sprintf(ret, "{\"ax\":%d,\"ay\":%d,\"az\":%d,\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f}", pDataXYZ[0], pDataXYZ[1], pDataXYZ[2], gDataXYZ[0]/1000, gDataXYZ[1]/1000, gDataXYZ[2]/1000);
    printf("n = %d", n);
    ThisThread::sleep_for(1000);
    return ret;
}

void http_demo(NetworkInterface *net, events::EventQueue &event_queue)
{
    TCPSocket socket;
    nsapi_error_t response;
    // int16_t ppDataXYZ[3] = {0};
    char* xyz_sen = NULL;
    char* xyz_stm = NULL;
    char* xyz = NULL;

    printf("Sending request to PC server...\n");
    
    // Open a socket on the network interface, and create a TCP connection to www.arm.com
    // SocketAddress a;
    // net->get_ip_address(&a);
    // printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None"); 
    // printf("Sending HTTP request to PC server...\n");
    // // Open a socket on the network interface, and create a TCP connection to //www.arm.com
    // socket.open(net); 
    // a.set_ip_address(HOST);
    // a.set_port(PORT);
    // response = socket.connect(a);
    // printf("Socket Address is %s\n", a.get_ip_address());

    // if(0 != response) {
    //     printf("Error connecting: %d\n", response);
    //     socket.close();
    //     return;
    // }
    
    Sensors* sensor = new Sensors(event_queue);

    while(1) {
        sensor->update();
        // xyz_sen = sensor->printSensorValue();
        xyz_stm = sensor->printStd();
        nsapi_size_t size = strlen(xyz_stm);
        xyz = xyz_stm;
        printf("xyz %s\n", xyz);
        response = 0;
        // while(size)
        // {
        //     response = socket.send(xyz+response, size);
        //     if (response < 0) {
        //         printf("Error sending data: %d\n", response);
        //         socket.close();
        //         return;
        //     } else {
        //         size -= response;
        //         // Check if entire message was sent or not
        //         printf("sent %d [%.*s]\n", response, strstr(xyz, "\r\n")-xyz, xyz);
        //     }
        // }
        // char rbuffer[1024];
        // response = socket.recv(rbuffer, sizeof rbuffer);
        // if (response < 0) {
        //     printf("Error receiving data: %d\n", response);
        // } else {
        //     printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
        // }
        ThisThread::sleep_for(1000);
        
    }

    socket.close();
}


int main()
{
    printf("\nWiFi-Socket practice\n\n");
    
    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error\n");
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi.get_mac_address());
    printf("IP: %s\n", wifi.get_ip_address());
    printf("Netmask: %s\n", wifi.get_netmask());
    printf("Gateway: %s\n", wifi.get_gateway());
    printf("RSSI: %d\n\n", wifi.get_rssi());

    // Sensors* sensor;

    http_demo(&wifi, event_queue);

    wifi.disconnect();

    printf("\nDone\n");
}
