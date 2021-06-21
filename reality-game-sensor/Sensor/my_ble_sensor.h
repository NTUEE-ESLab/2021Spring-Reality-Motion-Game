/* 
 * A BLE wrapper class for motion data sensor.
 *
 * [Warning!] This class is incomplete. Be careful before using this class.
 */

#ifndef __MY_BLE_SENSOR_H
#define __MY_BLE_SENSOR_H

#include "my_sensor.h"
#include "my_ble_char.h"

using mbed::callback;
using namespace std::literals::chrono_literals;

/**
 * ReadOnlyMotion Characteristic declaration helper.
 * 
 * The motion has default length of 5 (uint8_t).
 * 
 * [Warning!] This class may be incompatible with the data sensor.
 * Pay attention to modify some of the value before using it.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class ReadOnlyMotionCharacteristic : public GattCharacteristic {
public:
    /**
     * Construct a characteristic that is used to manipulate motion value.
     *
     * @param[in] uuid The UUID of the characteristic.
     * @param[in] initial_value Initial value contained by the characteristic.
     */
    ReadOnlyMotionCharacteristic(const UUID & uuid, const T& initial_value) :
        GattCharacteristic(
            /* UUID */ uuid,
            /* Initial value */ &_value,
            /* Value size */ 5,
            /* Value capacity */ 5,
            /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ,
            /* Descriptors */ nullptr,
            /* Num descriptors */ 0,
            /* variable len */ false
        ),
        _value(initial_value) {
    }

    /**
     * Get the value of this characteristic.
     *
     * @param[in] server GattServer instance that contain the characteristic
     * value.
     * @param[in] dst Variable that will receive the characteristic value.
     *
     * @return BLE_ERROR_NONE in case of success or an appropriate error code.
     */
    ble_error_t get(GattServer &server, T& dst) const
    {
        uint16_t value_length = sizeof(dst);
        return server.read(getValueHandle(), &dst, &value_length);
    }

    /**
     * Assign a new value to this characteristic.
     *
     * @param[in] server GattServer instance that will receive the new value.
     * @param[in] value The new value to set.
     * @param[in] local_only Flag that determine if the change should be kept
     * locally or forwarded to subscribed clients.
     */
    ble_error_t set(GattServer &server, const uint8_t* value, bool local_only = false) const
    {
        return server.write(getValueHandle(), value, 5, local_only);
    }

private:
    uint8_t _value;
};


class BLEDataSensor : public ble::GattServer::EventHandler {
public:
    BLEDataSensor() :
    data_sensor(event_queue),
    _motion_char(1, *_motion),
    _motion_service(
        /* uuid */ "A000",
        /* characteristics */ _motion_characteristics,
        /* numCharacteristics */ 1
    )
    {
        /* update internal pointers (value, descriptors and characteristics array) */
        _motion_characteristics[0] = &_motion_char;
    }

    /*
     * Start the BLE service.
     */
    void start(BLE &ble, events::EventQueue &event_queue);

    /*
     * Start the inner data sensor.
     */
    void startSensing();

    /*
     * Get motion type from data sensor.
     * 
     * [Warning!] This method is not implemented. You can reference wifi counterpart for more information.
     */
    void updateMotionStatus();

    /* GattServer::EventHandler */
private:
    /**
     * Handler called when a notification or an indication has been sent.
     */
    void onDataSent(const GattDataSentCallbackParams &params) override;

    /**
     * Handler called after an attribute has been written.
     */
    void onDataWritten(const GattWriteCallbackParams &params) override;

    /**
     * Handler called after an attribute has been read.
     */
    void onDataRead(const GattReadCallbackParams &params) override;

    /**
     * Handler called after a client has subscribed to notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params) override;

    /**
     * Handler called after a client has cancelled his subscription from
     * notification or indication.
     *
     * @param handle Handle of the characteristic value affected by the change.
     */
    void onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params) override;

    /**
     * Handler called when an indication confirmation has been received.
     *
     * @param handle Handle of the characteristic value that has emitted the
     * indication.
     */
    void onConfirmationReceived(const GattConfirmationReceivedCallbackParams &params) override;

private:
    GattServer *_server = nullptr;
    EventQueue *_event_queue = nullptr;
    DataSensor data_sensor;

    uint8_t _motion[5] = "ABCD";

    // Event service and characteristic
    GattService _motion_service;
    GattCharacteristic* _motion_characteristics[1];

    // Motion characteristic
    ReadOnlyMotionCharacteristic<uint8_t> _motion_char;
};


#endif