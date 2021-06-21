#include "my_ble_tag.h"

// Reference
// https://docs.microsoft.com/zh-tw/cpp/cpp/class-templates?view=msvc-160
// https://stackoverflow.com/questions/54095888/cant-use-override-in-function-c



/* Public */
RealGameTaggingService::RealGameTaggingService() :
    _led1(LED1, 1),
    _button(USER_BUTTON, PullUp),
    _event_id(1, 1),
    _event_level(2, 2),
    _event_item(3, 3),
    _event_challenger(4, 4),
    _event_sign_in(5, false),
    _event_signal(6, true),
    _event_service(
        /* uuid */ "A000",
        /* characteristics */ _event_characteristics,
        /* numCharacteristics */ 6
    )
{
    /* update internal pointers (value, descriptors and characteristics array) */
    _event_characteristics[0] = &_event_id;
    _event_characteristics[1] = &_event_level;
    _event_characteristics[2] = &_event_item;
    _event_characteristics[3] = &_event_challenger;
    _event_characteristics[4] = &_event_sign_in;
    _event_characteristics[5] = &_event_signal;

    /* setup authorization handlers */
    _event_signal.setWriteAuthorizationCallback(this, &RealGameTaggingService::led_client_write);
}

void RealGameTaggingService::start(BLE &ble, EventQueue &event_queue)
{
    _server = &ble.gattServer();
    _event_queue = &event_queue;
    ble_error_t err;

    printf("Registering demo service\r\n");
    err = _server->addService(_event_service);

    if (err)
    {
        printf("Error %u during demo service registration.\r\n", err);
        return;
    }

    /* register handlers */
    _server->setEventHandler(this);

    printf("button service registered\r\n");
    _button.fall(Callback<void()>(this, &RealGameTaggingService::button_pressed));
    _button.rise(Callback<void()>(this, &RealGameTaggingService::button_released));
}

void RealGameTaggingService::updateButtonState(bool newState) 
{
    ble_error_t err = _event_sign_in.set(*_server, newState);
    if (err) {
        printf("write of the second value returned error %u\r\n", err);
        return;
    }
}

void RealGameTaggingService::button_pressed(void) 
{
    _event_queue->call(this, &RealGameTaggingService::updateButtonState, true);
}

void RealGameTaggingService::button_released(void) 
{
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


