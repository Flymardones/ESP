#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "ICESleep.hpp"
#include "QPrint.hpp"
#include "DeviceControl.hpp"

int failedWiFiReconnects = 0;

void reconnect(DeviceControl* deviceControl) {
    deviceControl->btHandler->reconnect(); //Try to reconnect to owner every 10 seconds if connection is lost and we have a owner.
    // if (deviceControl->wiFiHandler != nullptr) {
    //     QPrint::println("Ip Address: " + deviceControl->wiFiHandler->getIPAsString());
    //     if (!deviceControl->wiFiHandler->isConnected() && failedWiFiReconnects < 3) { //Keep WiFi alive
    //         deviceControl->connectToSavedWiFi();
    //         failedWiFiReconnects++;
    //     }
    // }
}

void buttonCheck(DeviceControl* deviceControl) {
    uint32_t pinlevel = gpio_get_level(GPIO_NUM_32);
    ICESleep(2000);
    uint32_t pinlevelNow = gpio_get_level(GPIO_NUM_32);
    if (pinlevel == 0 && pinlevelNow == 0) {
        QPrint::println("Button held for 2 seconds!");
        deviceControl->removeBluetoothOwner();
    }
}

extern "C" void app_main(void)
{
    DeviceControl* deviceControl = new DeviceControl();

    gpio_config_t* ioConfig = new gpio_config_t;
    ioConfig->intr_type = GPIO_INTR_DISABLE;
    ioConfig->mode = GPIO_MODE_INPUT;
    ioConfig->pin_bit_mask = 1ULL<<GPIO_NUM_32;
    ioConfig->pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConfig->pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(ioConfig);
    delete ioConfig;

    while(1) {
        buttonCheck(deviceControl); //Each button check sleeps 2 seconds, so this is 4 seconds
        buttonCheck(deviceControl);

        QPrint::println("Free heap size: " + std::to_string(xPortGetFreeHeapSize()));
        QPrint::println("Minimum ever heap: " + std::to_string(xPortGetMinimumEverFreeHeapSize()));
        
        buttonCheck(deviceControl); //Each button check sleeps 2 seconds, so this is 4 seconds
        buttonCheck(deviceControl);
        reconnect(deviceControl);
    }
    
}

