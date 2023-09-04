#include "BluetoothHandler.hpp"

BluetoothHandler::BluetoothHandler() {
    esp_bt_controller_config_t controllerConfig = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    if (esp_bt_controller_init(&controllerConfig) != ESP_OK) {
        initialized = false;
        return;
    }

    esp_bredr_tx_power_set(ESP_PWR_LVL_P9, ESP_PWR_LVL_P9);

    if (esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT) != ESP_OK) {
        initialized = false;
        return;
    }

    if (esp_bluedroid_init() != ESP_OK) {
        initialized = false;
        return;
    }

    if (esp_bluedroid_enable() != ESP_OK) {
        initialized = false;
        return;
    }

    if (registerCallbacks() != 0) {
        initialized = false;
        return;
    }

    if (esp_spp_init(ESP_SPP_MODE_CB) != ESP_OK) {
        initialized = false;
        return;
    }

    if (esp_avrc_ct_init() != ESP_OK) {
        initialized = false;
        return;
    }

    if (esp_a2d_sink_init() != ESP_OK) {
        initialized = false;
        return;
    }
}

BluetoothHandler::~BluetoothHandler() {
    if (initialized) {
        esp_a2d_sink_deinit();
        esp_avrc_ct_deinit();
        esp_spp_deinit();

        esp_bluedroid_disable();
        esp_bluedroid_deinit();
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
    }
}

int BluetoothHandler::registerCallbacks() {
    callbackManager.setGAPController(&gapController);
    callbackManager.setSPPController(&sppController);
    callbackManager.setA2DPController(&a2dpController);
    callbackManager.setAVRCController(&avrcController);

    return callbackManager.registerCallbacks();
}

void BluetoothHandler::setDeviceName(const std::string& name) {
    esp_bt_dev_set_device_name(name.c_str());
}

void BluetoothHandler::setI2SOutput(I2SOutput* output) {
    a2dpController.setI2SOutput(output);
}

void BluetoothHandler::stopAudio() {
    //Stop A2DP and AVRC and their callbacks.
    avrcController.pause();
    a2dpController.disableOutput();
}

void BluetoothHandler::startAudio() {
    //Start A2DP and AVRC and their callbacks.
    a2dpController.enableOutput();
    avrcController.play();
}

void BluetoothHandler::setDiscoverable(bool discoverable) {
    if (discoverable) {
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
    } else {
        esp_bt_gap_set_scan_mode(ESP_BT_NON_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
    }
}

void BluetoothHandler::setOwner(esp_bd_addr_t owner) {
    memcpy(this->owner, owner, ESP_BD_ADDR_LEN);
}

void BluetoothHandler::reconnect() {
    if ((uint32_t) owner[0] == 0 && (uint16_t) owner[4] == 0) { //Everything is zeroes, no owner to reconnect to..
       //printf("No owner to reconnect to...\n");
        return;
    }

    else if (!a2dpController.isConnected()) {
        printf("Attempting bluetooth reconnect to owner: ");
        for(int i = 0; i < 6; i++) {
            printf("%02X ", owner[i]);
        }
        printf("\n");
        esp_a2d_sink_connect(owner);
    }
}

void BluetoothHandler::setDeviceController(DeviceControl *controller) {
    gapController.setDeviceController(controller);
    a2dpController.setDeviceController(controller);
}
