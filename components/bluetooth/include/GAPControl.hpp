#pragma once

#include <cstring>
#include "esp_gap_bt_api.h"   

class DeviceControl;

class GAPControl {
private:
    DeviceControl* deviceController = nullptr;
    void authentication_complete(esp_bt_gap_cb_param_t* parameter);
    void handle_pin_request(esp_bt_gap_cb_param_t* parameter);
    void reconnect_paired_dev(esp_bt_gap_cb_param_t* parameter);

public:
    void callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* parameter);
    void setDeviceController(DeviceControl* deviceController);
};