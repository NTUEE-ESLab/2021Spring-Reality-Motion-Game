#include "my_ble_tag.h"

// Reference
// https://docs.microsoft.com/zh-tw/cpp/cpp/class-templates?view=msvc-160
// https://stackoverflow.com/questions/54095888/cant-use-override-in-function-c

/* Public */
RealGameTaggingService::RealGameTaggingService() :
    _event_id(1, 20),
    _event_service(
        /* uuid */ "A000",
        /* characteristics */ _event_characteristics,
        /* numCharacteristics */ sizeof(_event_characteristics) /
                                    sizeof(_event_characteristics[0])
    ), 
    _led1(LED1, 1),
    _button(USER_BUTTON, PullUp),
    _button_state(2, false),
    _button_service(
        /* uuid */ "A001",
        /* characteristics */ _button_characteristics,
        /* numCharacteristics */ sizeof(_button_characteristics) /
                                    sizeof(_button_characteristics[0])
    ), 
    _led_state(3, true),
    _led_service(
        /* uuid */ "A002",
        /* characteristics */ _led_characteristics,
        /* numCharacteristics */ sizeof(_led_characteristics) /
                                    sizeof(_led_characteristics[0])
    ),
    _general_service(
        /* uuid */ "A003",
        /* characteristics */ _general_characteristics,
        /* numCharacteristics */ 3
    )
{
    /* update internal pointers (value, descriptors and characteristics array) */
    _event_characteristics[0] = &_event_id;
    _button_characteristics[0] = &_button_state;
    _led_characteristics[0] = &_led_state;

    // set up general characteristics
    _general_characteristics[0] = &_event_id;
    _general_characteristics[1] = &_button_state;
    _general_characteristics[2] = &_led_state;
    /* setup authorization handlers */
    _led_state.setWriteAuthorizationCallback(this, &RealGameTaggingService::led_client_write);
}

void RealGameTaggingService::start(BLE &ble, events::EventQueue &event_queue)
{
    _server = &ble.gattServer();
    _event_queue = &event_queue;
    ble_error_t err;

    printf("Registering demo service\r\n");
    // err = _server->addService(_button_service);
    // err = _server->addService(_led_service);
    // err = _server->addService(_stu_id_service);
    err = _server->addService(_general_service);

    if (err)
    {
        printf("Error %u during demo service registration.\r\n", err);
        return;
    }

    /* register handlers */
    _server->setEventHandler(this);

    printf("button service registered\r\n");
    // _event_queue->call_every(500ms, this, &RealGameTaggingService::blink);
    _button.fall(Callback<void()>(this, &RealGameTaggingService::button_pressed));
    _button.rise(Callback<void()>(this, &RealGameTaggingService::button_released));
}

/* Private */
void RealGameTaggingService::updateButtonState(bool newState) 
{
    ble_error_t err = _button_state.set(*_server, newState);
    if (err) {
        printf("write of the second value returned error %u\r\n", err);
        return;
    }
}

void RealGameTaggingService::button_pressed(void) 
{
    // updateButtonState(true);
    _event_queue->call(this, &RealGameTaggingService::updateButtonState, true);
}

void RealGameTaggingService::button_released(void) 
{
    // updateButtonState(false);
    _event_queue->call(this, &RealGameTaggingService::updateButtonState, false);
}

void RealGameTaggingService::blink(void) 
{
    _led1 = !_led1;
}

void RealGameTaggingService::led_turn_on(void) 
{
    _led1 = 1;
}

void RealGameTaggingService::led_turn_off(void) 
{
    _led1 = 0;
}

void RealGameTaggingService::led_client_write(GattWriteAuthCallbackParams *e)
{
    printf("characteristic %u write authorization\r\n", e->handle);

    if (e->offset != 0)
    {
        printf("Error invalid offset\r\n");
        e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_OFFSET;
        return;
    }

    if (e->len != 1)
    {
        printf("Error invalid len\r\n");
        e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_ATT_VAL_LENGTH;
        return;
    }

    if (e->data[0] != 0)
    {
        led_turn_on();
    }
    else
        led_turn_off();

    e->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
}

void RealGameTaggingService::onDataSent(const GattDataSentCallbackParams &params) 
{
    printf("sent updates\r\n");
}

void RealGameTaggingService::onDataWritten(const GattWriteCallbackParams &params)
{
    printf("data written:\r\n");
    printf("connection handle: %u\r\n", params.connHandle);
    printf("attribute handle: %u", params.handle);
    printf("\r\n");
}

void RealGameTaggingService::onDataRead(const GattReadCallbackParams &params)
{
    printf("data read:\r\n");
    printf("connection handle: %u\r\n", params.connHandle);
    printf("attribute handle: %u", params.handle);
}

void RealGameTaggingService::onUpdatesEnabled(const GattUpdatesEnabledCallbackParams &params)
{
    printf("update enabled on handle %d\r\n", params.attHandle);
}

void RealGameTaggingService::onUpdatesDisabled(const GattUpdatesDisabledCallbackParams &params)
{
    printf("update disabled on handle %d\r\n", params.attHandle);
}

void RealGameTaggingService::onConfirmationReceived(const GattConfirmationReceivedCallbackParams &params)
{
    printf("confirmation received on handle %d\r\n", params.attHandle);
}


template<typename T> 
RealGameTaggingService::
ReadWriteNotifyIndicateCharacteristic<T>::
ReadWriteNotifyIndicateCharacteristic(const UUID & uuid, const T& initial_value) :
    GattCharacteristic(
        /* UUID */ uuid,
        /* Initial value */ &_value,
        /* Value size */ sizeof(_value),
        /* Value capacity */ sizeof(_value),
        /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY |
                            GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE,
        /* Descriptors */ nullptr,
        /* Num descriptors */ 0,
        /* variable len */ false
    ),
    _value(initial_value) {}

template<typename T> 
ble_error_t 
RealGameTaggingService::
ReadWriteNotifyIndicateCharacteristic<T>::
get(GattServer &server, T& dst) const
{
    uint16_t value_length = sizeof(dst);
    return server.read(getValueHandle(), &dst, &value_length);
}

template<typename T> 
ble_error_t 
RealGameTaggingService::
ReadWriteNotifyIndicateCharacteristic<T>::
set(GattServer &server, const uint8_t &value, bool local_only) const
{
    return server.write(getValueHandle(), &value, sizeof(value), local_only);
}

template<typename T> 
uint8_t 
RealGameTaggingService::
ReadWriteNotifyIndicateCharacteristic<T>::
get_value(void) {
    return _value;
}