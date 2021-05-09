#include "Callback.h"
#include "mbed.h"
#include <cstdio>

// sensor module header

// wifi module header
#include "Sensor/my_wifi_sensor.h"
#include "my_wifi.h"

// BLE module header


// handle console ouput
static BufferedSerial serial_port(USBTX, USBRX);
FileHandle *mbed::mbed_override_console(int fd)
{
    return &serial_port; 
}

// I/O Devices


// EventQueue
EventQueue event_queue(64 * EVENTS_EVENT_SIZE);
Thread sensor_thread;
Thread event_thread;


int main() 
{
    WifiDataSensor* wifi_sensor = new WifiDataSensor(event_queue);

    event_thread.start(callback(&event_queue, &EventQueue::dispatch_forever));

    sensor_thread.start(callback(wifi_sensor, &WifiDataSensor::startSensing));

    wifi_sensor->connectWifi();

    wifi_sensor->connectHost();

    wifi_sensor->start();

    return 0;
}