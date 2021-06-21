/*
 * The BLE tagging service class.
 * 
 * The class is simply a data advertiser, which has few functionalities other than providing information.
 */

#ifndef __MY_BLE_H
#define __MY_BLE_H

#include "my_ble_char.h"

using mbed::callback;
using namespace std::literals::chrono_literals;

class RealGameTaggingService : public ble::GattServer::EventHandler {
public:
    RealGameTaggingService();

    void start(BLE &ble, events::EventQueue &event_queue);

private:
    /* 
     * Update button state for new state
     */
    void updateButtonState(bool newState);

    /* 
     * The button pressed handler, set button state to true.
     */
    void button_pressed(void);

    /* 
     * The button released handler, set button state to false.
     */
    void button_released(void);

    /* 
     * Toggle the led.
     */
    void blink(void);

    /* 
     * Turn on the led.
     */
    void led_turn_on(void);

    /* 
     * Trun off the led.
     */
    void led_turn_off(void);

    /* 
     * Authenticate client write, which will perform the corresponding action.
     */
    void led_client_write(GattWriteAuthCallbackParams *e);

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

    /*
     * event_id -- read only
     * event_level -- read only
     * event_item -- read, write only
     * event_challenger -- read, write only
     * event_sign_in (button) -- notify only
     * event_signal (led) -- write only
     */
    ReadOnlyCharacteristic<uint8_t> _event_id;
    ReadOnlyCharacteristic<uint8_t> _event_level;
    ReadWriteOnlyCharacteristic<uint8_t> _event_item;
    ReadWriteOnlyCharacteristic<uint8_t> _event_challenger;
    NotifyOnlyCharacteristic<bool> _event_sign_in;
    WriteOnlyCharacteristic<bool> _event_signal;
};

/*
 * Example:
    BLE &ble = BLE::Instance();
    events::EventQueue event_queue;
    RealGameTaggingService real_game_service;

    // this process will handle basic ble setup and advertising for us 
    GattServerProcess ble_process(event_queue, ble);

    // once it's done it will let us continue with our demo 
    ble_process.on_init(callback(&real_game_service, &RealGameTaggingService::start));

    ble_process.start();

    return 0;
 */

#endif

