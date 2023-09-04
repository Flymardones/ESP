//
// Created by Kris on 09/05/2022.
//

#include "NVS.hpp"
#include "nvs.h"
#include "QPrint.hpp"

NVS::NVS() {
    if (nvs_flash_init() != ESP_OK) {
        printf("NVS flash not working??\n");
        return;
    }
    else {
        printf("NVS flash init OK\n");
    }

    if (nvs_open("nvs", NVS_READWRITE, &nvsHandle) != ESP_OK) {
        //QPrint::println("Could not open NVSRam partition - creating it.");
        printf("Could not open NVSRam partition - creating it.");
        createNVSPartition();

        if (nvs_open("nvs", NVS_READWRITE, &nvsHandle) != ESP_OK) {
            //QPrint::println("Something is wrong with NVS.");
            printf("Something is wrong with NVS.");
            return;
        }
    }

    printf("Everything good with nvs\n");
}

NVS::~NVS() {
    nvs_flash_deinit();
}

uint32_t NVS::getBinaryValue(std::string key, uint8_t* data, uint32_t maxSize) {
    size_t length = MAX_LEN_BYTES;
    if (nvs_get_blob(nvsHandle, key.c_str(), data, &length) == ESP_OK) {
        return length;
    }

    QPrint::println("Failed to get nvs key: " + key);
    return 0;
}

std::string NVS::getStringValue(std::string key) {
    char content[MAX_LEN_BYTES];
    size_t length = MAX_LEN_BYTES;
    if (nvs_get_str(nvsHandle, key.c_str(), content, &length) != ESP_OK) {
        QPrint::println("Failed to get nvs key: " + key);
        return "";
    }

    if (length < MAX_LEN_BYTES) {
        content[length] = '\0'; //Terminate c string
    }

    return content;
}

bool NVS::getBoolValue(std::string key) {
    uint8_t value = 0;
    if (nvs_get_u8(nvsHandle, key.c_str(), &value) != ESP_OK) {
        QPrint::println("Failed to get nvs key: " + key);
        return false;
    }

    if (value > 0) {
        return true;
    }
    return false;
}

bool NVS::setBinaryValue(std::string key, uint8_t* data, uint32_t size) {
    if (size > MAX_LEN_BYTES) {
        return false;
    }

    if (nvs_set_blob(nvsHandle, key.c_str(), data, size) != ESP_OK) {
        QPrint::println("Failed to set nvs key: " + key);
        return false;
    }

    return true;
}

bool NVS::setStringValue(std::string key, std::string value) {
    if (value.length() > MAX_LEN_BYTES) {
        return false;
    }
    if (nvs_set_str(nvsHandle, key.c_str(), value.c_str()) != ESP_OK) {
        QPrint::println("Failed to set nvs key: " + key);
        return false;
    }

    return true;
}

bool NVS::setBoolValue(std::string key, bool value) {
    //In theory the bool can be anything but 0 to be true from this. We handle that in the get method.
    if (nvs_set_u8(nvsHandle, key.c_str(), value) != ESP_OK) {
        QPrint::println("Failed to set nvs key: " + key);
        return false;
    }

    return true;
}

bool NVS::createNVSPartition() {
    if (nvs_flash_init_partition("nvs") != ESP_OK) {
        QPrint::println("Could not create nvs partition!!");
        return false;
    }
    return true;
}

void NVS::reset() {
    //This sets default parameters. Current only setting WiFi is relevant.
    nvs_close(nvsHandle);
    if (nvs_flash_erase_partition("nvs") != ESP_OK) {
        printf("Failed to erase nvs partition\n");
    }
    createNVSPartition();
    if (nvs_open("nvs", NVS_READWRITE, &nvsHandle) != ESP_OK) {
        printf("Failed to open nvs partition\n");
    }

}






