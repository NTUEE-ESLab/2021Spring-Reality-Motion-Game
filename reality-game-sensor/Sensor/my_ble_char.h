/*
 * This file contains several BLE characteristics template:
 *   - GeneralCharacteristic
 *   - ReadOnlyCharacteristic
 *   - WriteOnlyCharacteristic
 *   - ReadWriteOnlyCharacteristic
 *   - NotifyOnlyCharacteristic
 *   - ReadWriteNotifyCharacteristic
 *   - ReadWriteNotifyIndicateCharacteristic
 */

#ifndef __MY_BLE_CHAR_H
#define __MY_BLE_CHAR_H

#include "ble/gatt/GattCharacteristic.h"
#include "ble/gatt/GattService.h"
#include "PinNames.h"
#include "PinNamesTypes.h"
#include "platform/Callback.h"
#include "events/EventQueue.h"
#include "ble/BLE.h"
#include "gatt_server_process.h"
#include "pretty_printer.h"
#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <functional>

/**
 * General Characteristic declaration helper. It is the parent of other characteristic classes.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class GeneralCharacteristic : public GattCharacteristic {
public:
    /**
     * Construct a characteristic that has no characteristic properties.
     *
     * @param[in] uuid The UUID of the characteristic.
     * @param[in] initial_value Initial value contained by the characteristic.
     */
    GeneralCharacteristic(const UUID & uuid, const T& initial_value, 
                        uint8_t props = GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NONE) :
        GattCharacteristic(
            /* UUID */ uuid,
            /* Initial value */ &_value,
            /* Value size */ sizeof(_value),
            /* Value capacity */ sizeof(_value),
            /* Properties */ props,
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
    ble_error_t set(GattServer &server, const uint8_t &value, bool local_only = false) const
    {
        return server.write(getValueHandle(), &value, sizeof(value), local_only);
    }

    uint8_t get_value(void) {
        return _value;
    }

private:
    uint8_t _value;
};


/**
 * ReadOnly Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class ReadOnlyCharacteristic : public GeneralCharacteristic<T> {
public:
    ReadOnlyCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ) {}
};

/**
 * WriteOnly Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class WriteOnlyCharacteristic : public GeneralCharacteristic<T> {
public:

    WriteOnlyCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE) {}    
};

/**
 * ReadWriteOnly Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class ReadWriteOnlyCharacteristic : public GeneralCharacteristic<T> {
public:
    ReadWriteOnlyCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE) {} 
};

/**
 * NotifyOnly Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class NotifyOnlyCharacteristic : public GeneralCharacteristic<T> {
public:

    NotifyOnlyCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {}
};

/**
 * ReadWriteNotify Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class ReadWriteNotifyCharacteristic : public GeneralCharacteristic<T> {
public:

    ReadWriteNotifyCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE|
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {}
};

/**
 * ReadWriteNotifyIndicate Characteristic declaration helper.
 *
 * @tparam T type of data held by the characteristic.
 */
template<typename T>
class ReadWriteNotifyIndicateCharacteristic : public GeneralCharacteristic<T> {
public:

    ReadWriteNotifyIndicateCharacteristic(const UUID & uuid, const T& initial_value) :
        GeneralCharacteristic<T>(uuid, initial_value, 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ    | 
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE   |
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY  |
                                GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE) {}
};


#endif