#include "GAPControl.hpp"
#include "DeviceControl.hpp"

void GAPControl::callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* parameter) {
    switch(event) {
        case ESP_BT_GAP_AUTH_CMPL_EVT:
            authentication_complete(parameter);
            break;
        case ESP_BT_GAP_PIN_REQ_EVT:
            handle_pin_request(parameter);
            break;
        // case ESP_BT_GAP_MODE_CHG_EVT:
        //     disconnect(parameter); // Clear device address when disconnecting
        //     break;
        case ESP_BT_GAP_ACL_CONN_CMPL_STAT_EVT:
            //reconnect_paired_dev(parameter); // Set BT device address when a paired device is connecting
            break;
        default:
            printf("Unknown GAP callback received: %d\n", event);
            break;
    }
}

void GAPControl::reconnect_paired_dev(esp_bt_gap_cb_param_t* parameter) {
    if(deviceController != nullptr){
        deviceController->setBluetoothOwner(parameter->auth_cmpl.bda);
        printf("Setting BT Owner in GAP Callback: ");
        for(int i = 0; i < sizeof(parameter->auth_cmpl.bda); i++) {
            printf("%02X ", parameter->auth_cmpl.bda[i]);
        }
        printf("\n");
    }
}

void GAPControl::authentication_complete(esp_bt_gap_cb_param_t* parameter) {
    if (parameter->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
        printf("GAP Authentication successful!\n");
        if (deviceController != nullptr) {
            deviceController->setBluetoothOwner(parameter->auth_cmpl.bda);
            printf("Setting BT owner in gap callback: ");
            for(int i = 0; i < sizeof(parameter->auth_cmpl.bda); i++) {
                printf("%02X ", parameter->auth_cmpl.bda[i]);
            }
            printf("\n");
        }
    } else {
        printf("GAP Authentication failed. Status: %d\n", parameter->auth_cmpl.stat);
    }
}

void GAPControl::handle_pin_request(esp_bt_gap_cb_param_t* parameter) {
    esp_bt_pin_code_t pin;
    uint8_t pinLength = 4;
    
    if (parameter->pin_req.min_16_digit) {    
        pinLength =  16;
        memset(pin, '0', ESP_BT_PIN_CODE_LEN);
    } else {
        pin[0] = '0';
        pin[1] = '0';
        pin[2] = '0';
        pin[3] = '0';
    }

    printf("Expecting this pin code: %.*s\n", pinLength, pin);
    esp_bt_gap_pin_reply(parameter->pin_req.bda, true, pinLength, pin);
}

void GAPControl::setDeviceController(DeviceControl* deviceController) {
    this->deviceController = deviceController;
}