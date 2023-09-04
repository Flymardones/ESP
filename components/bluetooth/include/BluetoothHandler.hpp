#pragma once

#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_err.h"
#include <string>
#include "BluetoothCallback.hpp"

class DeviceControl; //Forward declaration, BluetoothHandler will pass device controller on to GAPControl.

class BluetoothHandler {
private:
    BluetoothCallbackManager callbackManager;
    bool initialized = true;
    esp_bd_addr_t owner = {0,0,0,0,0,0};

    int registerCallbacks();

public:
    GAPControl gapController;
    SPPControl sppController;
    A2DPControl a2dpController;
    AVRCControl avrcController;
    BluetoothHandler();
    ~BluetoothHandler();
    
    void setDeviceName(const std::string& name);
    void setI2SOutput(I2SOutput* output);

    void stopAudio();
    void startAudio();
    void setDiscoverable(bool discoverable);
    void setOwner(esp_bd_addr_t owner);
    void setDeviceController(DeviceControl* controller);
    void reconnect();
};