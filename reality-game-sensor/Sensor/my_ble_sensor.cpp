#include "my_ble_sensor.h"
#include <cstdint>

void BLEDataSensor::startSensing() {
    data_sensor.start();
}


void BLEDataSensor::start(BLE &ble, EventQueue &event_queue)
{
    _server = &ble.gattServer();
    _event_queue = &event_queue;
    ble_error_t err;

    printf("Registering motion service\r\n");
    err = _server->addService(_motion_service);

    const static uint8_t motion_status[5] = {0, 0, 0, 0}; 

    _motion_char.set(*_server, motion_status);


    if (err)
    {
        printf("Error %u during demo service registration.\r\n", err);
        return;
    }

    /* register handlers */
    _server->setEventHandler(this);

    // Start sending sensor data
    _event_queue->call_every(100, this, &BLEDataSensor::updateMotionStatus);
}

void BLEDataSensor::updateMotionStatus() {
    return;
}

void BLEDataSensor::onDataSent(const GattDataSentCallbackParams &params) 
{
    printf("sent updates\r\n");
}

void BLEDataSensor::onDataWritten(const GattWriteCallbackParams &params)
{
    printf("data written:\r\n");
    printf("connection handle: %u\r\n", params.connHandle);
    printf("attribute handle: %u", params.handle);
    printf("\r\n");
}

void BLEDataSensor::onDataRead(const GattReadCallbackParams &params)
{
    printf("data read:\r\n");
    printf("connection handle: %u\r\n", params.connHandle);
    printf("attribute handle: %u", params.handle);
}

void BLEDataSensor::onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params)
{
    printf("update enabled on handle %d\r\n", params.attHandle);
}

void BLEDataSensor::onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params)
{
    printf("update disabled on handle %d\r\n", params.attHandle);
}

void BLEDataSensor::onConfirmationReceived(const GattConfirmationReceivedCallbackParams &params)
{
    printf("confirmation received on handle %d\r\n", params.attHandle);
}


