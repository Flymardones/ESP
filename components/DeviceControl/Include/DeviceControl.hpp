//
// Created by Kris on 09/05/2022.
//

#ifndef ESPSTREAMER_DEVICECONTROL_HPP
#define ESPSTREAMER_DEVICECONTROL_HPP

#include "NVS.hpp"
//#include "IcecastHTTPClient.hpp"
#include "BluetoothHandler.hpp"
//#include "WiFiHandler.hpp"
#include "QPrint.hpp"
#include "esp_bt_defs.h"

#define maxWifisToScan 32

class DeviceControl : QPrinter {
private:
    enum DeviceControlState {
        DEVICE_IDLE,
        DEVICE_CHOOSING_WIFI,
        DEVICE_ENTER_WIFI_PASS,
    };

    DeviceControlState deviceState = DEVICE_IDLE;
    //WiFiInfo* wifiNetworks = nullptr;
    int foundWiFis = 0;
    int chosenWiFi;

    void printWifis();

public:
    NVS nvs;
    I2SOutput i2sOutput;
    BluetoothHandler* btHandler = nullptr;
    //WiFiHandler* wiFiHandler = nullptr;
    //IcecastHTTPClient* ICECastClient = nullptr;
    //StreamDecoder* mp3Decoder = nullptr;
    uint8_t intr_status = 1;
    DeviceControl();
    //void connectToSavedWiFi();
    void startBluetooth();
    //void startRadioStream(std::string link);
    //void stopRadioStream();
    void setBluetoothOwner(esp_bd_addr_t deviceAddress);
    void removeBluetoothOwner();
    //WiFiInfo* scanWifis(int* foundNetworks);

    void print(std::string input) override; //This takes a string input and processes it depending on the state of DeviceControl
    void processIdleInput(std::string input);
    //void processWiFiChoice(std::string input);
    //void processWiFiPass(std::string input);

};

#endif //ESPSTREAMER_DEVICECONTROL_HPP
