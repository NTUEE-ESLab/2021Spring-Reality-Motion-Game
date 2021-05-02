#ifndef __MY_WIFI_H
#define __MY_WIFI_H

#include "mbed.h"
#include "TCPSocket.h"
#include "mbed_wait_api.h"

#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
extern ISM43362Interface wifi;

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
extern SpwfSAInterface wifi;
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif


const char *sec2str(nsapi_security_t);
void print_wifi_info(ISM43362Interface &);
int scan_devices(WiFiInterface *, int);
void send_sensor_data(NetworkInterface *);

#endif