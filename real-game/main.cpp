#include "Callback.h"
#include "mbed.h"
#include <cstdio>

// sensor module header

// wifi module header
// #include "my_wifi_sensor.h"

// BLE module header
#include "my_ble_tag.h"
#include "my_ble_scan_tag.h"


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
Thread adv_thread;


int main() 
{
    // WifiDataSensor* wifi_sensor = new WifiDataSensor(event_queue);

    // event_thread.start(callback(&event_queue, &EventQueue::dispatch_forever));

    // sensor_thread.start(callback(wifi_sensor, &WifiDataSensor::startSensing));

    // wifi_sensor->connectWifi();

    // wifi_sensor->connectHost();

    // wifi_sensor->start();
    BLE &ble = BLE::Instance();
    events::EventQueue event_queue;
    RealGameTaggingService real_game_service;

    // this process will handle basic ble setup and advertising for us 
    GattServerProcess ble_process(event_queue, ble);

    // once it's done it will let us continue with our demo 
    ble_process.on_init(callback(&real_game_service, &RealGameTaggingService::start));

    ble_process.start();

    return 0;
}