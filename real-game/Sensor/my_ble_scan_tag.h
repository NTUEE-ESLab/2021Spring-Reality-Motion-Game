#ifndef __MY_BLE_SCAN_TAG_H
#define __MY_BLE_SCAN_TAG_H

#include <events/mbed_events.h>
#include "ble/BLE.h"
#include "ble/Gap.h"
#include "pretty_printer.h"

const static char DEVICE_NAME[] = "BATTERY";

using namespace std::literals::chrono_literals;

extern EventQueue event_queue;

class BatteryDemo : ble::Gap::EventHandler {
public:
    BatteryDemo(BLE &ble, events::EventQueue &event_queue) :
        _ble(ble),
        _gap(ble.gap()),
        _event_queue(event_queue),
        _battery_level(50),
        _adv_data_builder(_adv_buffer)
    {
    }

    void start()
    {
        printf("Ble process started.\r\n");

        if (_ble.hasInitialized()) {
            printf("Error: the ble instance has already been initialized.\r\n");
            return;
        }

        /* handle gap events */
        _gap.setEventHandler(this);

        /* This will inform us off all events so we can schedule their handling
         * using our event queue */
        _ble.onEventsToProcess(
            makeFunctionPointer(this, &BatteryDemo::schedule_ble_events)
        );
        /* mbed will call on_init_complete when when ble is ready */
        _ble.init(this, &BatteryDemo::on_init_complete);

        /* this will never return */
        // _event_queue.dispatch_forever();
    }

private:
    /** Callback triggered when the ble initialization process has finished */
    void on_init_complete(BLE::InitializationCompleteCallbackContext *params)
    {
        if (params->error != BLE_ERROR_NONE) {
            print_error(params->error, "Ble initialization failed.");
            return;
        }

        print_mac_address();

        start_advertising();
    }

    void start_advertising()
    {
        /* create advertising parameters and payload */

        ble::AdvertisingParameters adv_parameters(
            /* you cannot connect to this device, you can only read its advertising data,
             * scannable means that the device has extra advertising data that the peer can receive if it
             * "scans" it which means it is using active scanning (it sends a scan request) */
            ble::advertising_type_t::SCANNABLE_UNDIRECTED,
            ble::adv_interval_t(ble::millisecond_t(1000))
        );

        _adv_data_builder.setFlags();
        _adv_data_builder.setName(DEVICE_NAME);

        /* we add the battery level as part of the payload so it's visible to any device that scans */
        _adv_data_builder.setServiceData(GattService::UUID_BATTERY_SERVICE, {&_battery_level, 1});

        /* setup advertising */

        ble_error_t error = _ble.gap().setAdvertisingParameters(
            ble::LEGACY_ADVERTISING_HANDLE,
            adv_parameters
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingParameters() failed");
            return;
        }

        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingPayload() failed");
            return;
        }

        /* when advertising you can optionally add extra data that is only sent
         * if the central requests it by doing active scanning */
        _adv_data_builder.clear();
        const uint8_t _vendor_specific_data[4] = { 0xAD, 0xDE, 0xBE, 0xEF };
        _adv_data_builder.setManufacturerSpecificData(_vendor_specific_data);

        _ble.gap().setAdvertisingScanResponse(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        /* start advertising */

        error = _ble.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);

        if (error) {
            print_error(error, "_ble.gap().startAdvertising() failed");
            return;
        }

        /* we simulate battery discharging by updating it every second */
        _event_queue.call_every(
            1000ms,
            [this]() {
                update_battery_level();
            }
        );
    }

    void update_battery_level()
    {
        if (_battery_level-- == 10) {
            _battery_level = 100;
        }

        /* update the payload with the new value */
        ble_error_t error = _adv_data_builder.setServiceData(GattService::UUID_BATTERY_SERVICE, make_Span(&_battery_level, 1));

        if (error) {
            print_error(error, "_adv_data_builder.setServiceData() failed");
            return;
        }

        /* set the new payload, we don't need to stop advertising */
        error = _ble.gap().setAdvertisingPayload(
            ble::LEGACY_ADVERTISING_HANDLE,
            _adv_data_builder.getAdvertisingData()
        );

        if (error) {
            print_error(error, "_ble.gap().setAdvertisingPayload() failed");
            return;
        }
    }

    /* Schedule processing of events from the BLE middleware in the event queue. */
    void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context)
    {
        event_queue.call(Callback<void()>(&context->ble, &BLE::processEvents));
    }



private:
    BLE &_ble;
    ble::Gap &_gap;
    events::EventQueue &_event_queue;

    uint8_t _battery_level;

    uint8_t _adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder _adv_data_builder;
};



// int main()
// {
//     BLE &ble = BLE::Instance();
//     ble.onEventsToProcess(schedule_ble_events);

//     BatteryDemo demo(ble, event_queue);
//     demo.start();

//     return 0;
// }

#endif