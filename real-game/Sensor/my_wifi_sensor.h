
#ifndef __MY_WIFI_SENSOR_H
#define __MY_WIFI_SENSOR_H

#include "my_sensor.h"
#include "mbed.h"
#include "TCPSocket.h"
#include "mbed_wait_api.h"
#include "ISM43362Interface.h"

#define WIFI_IDW0XX1    2


class WifiDataSensor {
public:
    // Constructor
    WifiDataSensor(EventQueue &event_queue) : 
        data_sensor(event_queue), _wifi(false) {}

    void print_wifi_info() {
        printf("MAC: %s\n", _wifi.get_mac_address()); 
        printf("IP: %s\n", _wifi.get_ip_address()); 
        printf("Netmask: %s\n", _wifi.get_netmask()); 
        printf("Gateway: %s\n", _wifi.get_gateway()); 
        printf("RSSI: %d\n\n", _wifi.get_rssi());
    }   

    const char *sec2str(nsapi_security_t sec)
    {
        switch (sec) {
            case NSAPI_SECURITY_NONE:
                return "None";
            case NSAPI_SECURITY_WEP:
                return "WEP";
            case NSAPI_SECURITY_WPA:
                return "WPA";
            case NSAPI_SECURITY_WPA2:
                return "WPA2";
            case NSAPI_SECURITY_WPA_WPA2:
                return "WPA/WPA2";
            case NSAPI_SECURITY_UNKNOWN:
            default:
                return "Unknown";
        }
    }

    int scan_devices(int limit_count = 15)
    {
        WiFiInterface *wifi = &_wifi;
        WiFiAccessPoint *ap;

        printf("Scan:\n");

        int count = wifi->scan(NULL,0);
        printf("%d networks available.\n", count);

        /* Limit number of network arbitrary to 15 */
        count = count < limit_count ? count : limit_count;

        ap = new WiFiAccessPoint[count];
        count = wifi->scan(ap, count);

        for (int i = 0; i < count; i++)
        {
            printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
                sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
                ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
        }

        delete[] ap;
        return count;
    }

    void send_sensor_data(char* IP_ADDRESS = (char*)"192.168.50.252", int port = 30007)
    {
        NetworkInterface *net = &_wifi;
        printf("Sending data to host computer...\n");

        // Open a socket on the network interface, and create a TCP connection to //www.arm.com
        socket.open(net); 
        if(!a.set_ip_address(IP_ADDRESS)) {
            printf("Set IP address failed");
            return ;
        }

        a.set_port(port);
        
        response = socket.connect(a);

        if(0 != response) {
            printf("Error connecting: %d\n", response);
            socket.close();
            return;
        }

        int count = 0;

        char* xyz_sen = NULL;
        char* xyz_stm = NULL;
        char* xyz = NULL;

        while(1) {
                data_sensor.update();
                xyz_stm = data_sensor.printStd();
                nsapi_size_t size = strlen(xyz_stm);
                xyz = xyz_stm;
                printf("%s\n", xyz);
                ThisThread::sleep_for(10);
        }

        socket.close();
    }

private:
    DataSensor data_sensor;
    TCPSocket socket;
    nsapi_error_t response;
    ISM43362Interface _wifi;
    SocketAddress a;
    
};

#endif