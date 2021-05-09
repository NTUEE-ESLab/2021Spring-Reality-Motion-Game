#include "my_wifi_sensor.h"
#include "WiFiAccessPoint.h"
#include "my_sensor.h"
#include <cstdio>

void WifiDataSensor::print_wifi_info() {
    printf("MAC: %s\n", _wifi.get_mac_address()); 
    printf("IP: %s\n", _wifi.get_ip_address()); 
    printf("Netmask: %s\n", _wifi.get_netmask()); 
    printf("Gateway: %s\n", _wifi.get_gateway()); 
    printf("RSSI: %d\n\n", _wifi.get_rssi());
}  

const char* WifiDataSensor::sec2str(nsapi_security_t sec) {
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

int WifiDataSensor::scan_devices(int limit_count = 15) {
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

void WifiDataSensor::startSensing() {
    data_sensor.start();
}

void WifiDataSensor::send_sensor_data() {
    record_count++;
    xyz_std = data_sensor.getStdWifi();


    if(record_count % 10 == 0) {
        int len = 0;

        len = sprintf(buffer, "%s", std_records.c_str());
        
        response = socket.send(buffer,len); 
        if (0 >= response){
            printf("Error seding: %d\n", response); 
        }

        std_records.clear();

    } else {
        std_records += string(xyz_std);
    }
}

void WifiDataSensor::connectWifi() {
    int ret = _wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2); 
    
    if (ret != 0) {
        printf("\nConnection error\n");
        return; 
    }

    printf("Success\n\n");
    print_wifi_info();
}

void WifiDataSensor::connectHost() {
    NetworkInterface *net = &_wifi;
    printf("Sending data to host computer...\n");

    // Open a socket on the network interface, and create a TCP connection to //www.arm.com
    socket.open(net); 
    if(!a.set_ip_address(IP_ADDRESS)) {
        printf("Set IP address failed");
        return ;
    }

    a.set_port(PORT);

    
    response = socket.connect(a);

    if(0 != response) {
        printf("Error connecting: %d\n", response);
        disconnect();
        return;
    }
}

void WifiDataSensor::disconnect() {
    printf("Closing socket!\n");
    socket.close();
}

void WifiDataSensor::start()
{
    // Not successfully connected
    if (response != 0) return;

    // Start sending sensor data
    _event_queue.call_every(10, this, &WifiDataSensor::send_sensor_data);
}
