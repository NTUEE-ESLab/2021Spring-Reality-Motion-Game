/* mbed Microcontroller Library
 * Copyright (c) 2017-2019 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "my_ble.h"


void LocationService::start(BLE &ble, events::EventQueue &event_queue)
{
    _server = &ble.gattServer();
    _event_queue = &event_queue;
    ble_error_t err;

    printf("Registering demo service\r\n");
    // err = _server->addService(_button_service);
    // err = _server->addService(_led_service);
    // err = _server->addService(_stu_id_service);
    err = _server->addService(_general_service);

    if (err) {
        printf("Error %u during demo service registration.\r\n", err);
        return;
    }

    /* register handlers */
    _server->setEventHandler(this);

    printf("button service registered\r\n");
    _event_queue->call_every(1000ms, callback(this, &LocationService::send_std_id));
    // _event_queue->call_every(500ms, this, &LocationService::blink);
    _button.fall(Callback<void()>(this, &LocationService::button_pressed));
    _button.rise(Callback<void()>(this, &LocationService::button_released));
}

void LocationService::send_std_id(void) 
{
    const static uint8_t stu_id[10] = "B07901184";
    ble_error_t err = _stu_id_char.set(*_server, stu_id);
    if (err) {
        printf("write of the second value returned error %u\r\n", err);
        return;
    }
}

void LocationService::led_client_write(GattWriteAuthCallbackParams *e)
{
    printf("characteristic %u write authorization\r\n", e->handle);

    if (e->offset != 0) {
        printf("Error invalid offset\r\n");
        e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_OFFSET;
        return;
    }

    if (e->len != 1) {
        printf("Error invalid len\r\n");
        e->authorizationReply = AUTH_CALLBACK_REPLY_ATTERR_INVALID_ATT_VAL_LENGTH;
        return;
    }

    if (e->data[0] != 0) {
        led_turn_on();
    }
    else led_turn_off();

    e->authorizationReply = AUTH_CALLBACK_REPLY_SUCCESS;
}


/*
 * Example:
    // wifi variables
    BLE &ble = BLE::Instance();
    events::EventQueue event_queue;
    LocationService demo_service;

    // this process will handle basic ble setup and advertising for us 
    GattServerProcess ble_process(event_queue, ble);

    // once it's done it will let us continue with our demo 
    ble_process.on_init(callback(&demo_service, &LocationService::start));

    ble_process.start();

    return 0;
 */

