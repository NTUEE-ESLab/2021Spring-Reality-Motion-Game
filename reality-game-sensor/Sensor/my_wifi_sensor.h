/* 
 * A Wifi wrapper class over the motion data sensor.
 * 
 * This class provides wifi functionality that can send motion type to game server.
 * The transmission format is a simple [type:motion] pair.
 */

#ifndef __MY_WIFI_SENSOR_H
#define __MY_WIFI_SENSOR_H

#include "EventQueue.h"
#include "my_sensor.h"
#include "mbed.h"
#include "TCPSocket.h"
#include "mbed_wait_api.h"
#include "ISM43362Interface.h"
#include <string>

#define WIFI_IDW0XX1    2
// #define IP_ADDRESS "172.20.10.2"
#define IP_ADDRESS "192.168.50.101"
// #define IP_ADDRESS "192.168.1.254"
#define PORT 30006

class WifiDataSensor {
public:
    WifiDataSensor(EventQueue &event_queue) : 
        data_sensor(event_queue), _event_queue(event_queue), 
        _wifi(false), buffer(), record_count(0), xyz_std(NULL), type(0) {
            std_records = string();
        }

    /* 
     * Print wifi information including 
     *  - Mac address
     *  - IP address
     *  - Network mask
     *  - Gateway
     *  - RSSI
     */
    void print_wifi_info();

    /* 
     * Scan available wifi access point.
     * You can also set some limits, the default is 15
     */
    int scan_devices(int limit_count);

    /* 
     * Start the inner data sensor.
     * 
     * The data sensor class is at my_sensor.h file.
     */
    void startSensing();

    /* 
     * Send motion data to the host computer.
     * 
     * Get motion type from the inner data sensor.
     * The motion type is of ["type":$motion] pair.
     */
    void send_sensor_data();

    /* 
     * Register an event in the event queue.
     * 
     * The queue will call send_sensor_data() for every 100ms.
     */
    void start();

    /* 
     * Connect to the wifi access point.
     * 
     * The macro will be automatically determined by the Mbed OS. 
     * You have to modify the mbed_app.json file to configure the correct SSID and password.
     * 
     * Note that it seems that STM32 only support 802.11g wifi protocol. 
     * Make sure your wifi access point is compatible.
     * 
     * The default configuration is in mbed_app.json file.
     */
    void connectWifi();

    /* 
     * Connect to the host computer through wifi.
     * 
     * The IP address and PORT of the host computer is written in MACRO.
     */
    void connectHost();

    /* 
     * Disconnect from the host computer. 
     */
    void disconnect();

private:
    EventQueue &_event_queue;
    DataSensor data_sensor;
    TCPSocket socket;
    nsapi_error_t response;
    ISM43362Interface _wifi;
    SocketAddress a;

    // Socket data buffer
    char buffer[1024];

    string std_records;
    int record_count;
    char* xyz_std;
    int type;

    /* 
     * A converter function for security protocols.
     */
    const char *sec2str(nsapi_security_t sec);
};

#endif