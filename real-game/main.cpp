#include "Callback.h"
#include "mbed.h"
#include <cstdio>

// sensor module header

// wifi module header
#include "my_wifi_sensor.h"

// BLE module header
// #include "my_ble_tag.h"
// #include "my_ble_sensor.h"

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
    /* Wifi Data Sensor */
    WifiDataSensor* wifi_sensor = new WifiDataSensor(event_queue);
    
    // wifi_sensor->scan_devices(15);

    event_thread.start(callback(&event_queue, &EventQueue::dispatch_forever));

    sensor_thread.start(callback(wifi_sensor, &WifiDataSensor::startSensing));

    wifi_sensor->connectWifi();

    wifi_sensor->connectHost();

    wifi_sensor->start();

    /* BLE Tag */
    // BLE &ble = BLE::Instance();
    // events::EventQueue event_queue;
    // RealGameTaggingService real_game_service;

    // // this process will handle basic ble setup and advertising for us 
    // GattServerTagProcess ble_process(event_queue, ble);

    // // once it's done it will let us continue with our demo 
    // ble_process.on_init(callback(&real_game_service, &RealGameTaggingService::start));

    // ble_process.start();

    /* BLE Data Sensor */
    // BLE &ble = BLE::Instance();
    // events::EventQueue event_queue;
    // BLEDataSensor motion_sensor;

    // event_thread.start(callback(&event_queue, &EventQueue::dispatch_forever));
    // sensor_thread.start(callback(&motion_sensor, &BLEDataSensor::startSensing));

    // // this process will handle basic ble setup and advertising for us 
    // GattServerMotionProcess ble_process(event_queue, ble);

    // // once it's done it will let us continue with our demo 
    // ble_process.on_init(callback(&motion_sensor, &BLEDataSensor::start));

    // ble_process.start();

    

    return 0;
}