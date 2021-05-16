#ifndef __MY_BLE_H
#define __MY_BLE_H

#include "my_ble_char.h"

/*
 * Example:
    BLE &ble = BLE::Instance();
    events::EventQueue event_queue;
    RealGameTaggingService demo_service;

    // this process will handle basic ble setup and advertising for us 
    GattServerProcess ble_process(event_queue, ble);

    // once it's done it will let us continue with our demo 
    ble_process.on_init(callback(&demo_service, &RealGameTaggingService::start));

    ble_process.start();

    return 0;
 */


using mbed::callback;
using namespace std::literals::chrono_literals;

class RealGameTaggingService : public ble::GattServer::EventHandler {
public:
    RealGameTaggingService();

    void start(BLE &ble, events::EventQueue &event_queue);

private:
    void updateButtonState(bool newState);

    void button_pressed(void);

    void button_released(void);

    void blink(void);

    void led_turn_on(void);

    void led_turn_off(void);

    void led_client_write(GattWriteAuthCallbackParams *e);

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

    InterruptIn _button;
    DigitalOut  _led1;

    // event service and characteristic
    GattService _event_service;
    GattCharacteristic* _event_characteristics[6];

    ReadOnlyCharacteristic<uint8_t> _event_id;
    ReadOnlyCharacteristic<uint8_t> _event_level;
    ReadWriteOnlyCharacteristic<uint8_t> _event_item;
    ReadWriteOnlyCharacteristic<uint8_t> _event_challenger;
    NotifyOnlyCharacteristic<uint8_t> _event_sign_in;
    WriteOnlyCharacteristic<uint8_t> _event_signal;

    // button service and characteristic
    GattService _button_service;
    GattCharacteristic* _button_characteristics[1];

    ReadWriteNotifyCharacteristic<bool> _button_state;

    // led service and characteristic
    GattService _led_service;
    GattCharacteristic* _led_characteristics[1];

    ReadWriteNotifyCharacteristic<bool> _led_state;

    // try to combine three charateristic into one service
    GattService _general_service;
    GattCharacteristic* _general_characteristics[3];
};

#endif

