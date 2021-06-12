
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
#define IP_ADDRESS "172.20.10.2"
// #define IP_ADDRESS "192.168.1.254"
#define PORT 30006
// #define SAMPLE_RATE

class WifiDataSensor {
public:
    // Constructor
    WifiDataSensor(EventQueue &event_queue) : 
        data_sensor(event_queue), _event_queue(event_queue), 
        _wifi(false), buffer(), record_count(0), xyz_std(NULL), type(0) {
            std_records = string();
        }

    void print_wifi_info();

    const char *sec2str(nsapi_security_t sec);

    int scan_devices(int limit_count);

    void startSensing();

    void send_sensor_data();

    void start();

    void connectWifi();

    void connectHost();

    void disconnect();

private:
    EventQueue &_event_queue;
    DataSensor data_sensor;
    TCPSocket socket;
    nsapi_error_t response;
    ISM43362Interface _wifi;
    SocketAddress a;
    char buffer[1024];
    string std_records;
    int record_count;
    char* xyz_std;
    int type;
};

#endif