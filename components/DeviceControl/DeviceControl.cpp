//
// Created by Kris on 09/05/2022.
//

#include "DeviceControl.hpp"
#include "ICESleep.hpp"
#include "PrintfQPrinter.hpp"
#include "ICEString.hpp"

DeviceControl::DeviceControl() {
    //Start printfprinter for QPrint
    PrintfQPrinter* printfQPrinter = new PrintfQPrinter(); //Make QPrint debug appear in as printf messages
    QPrint::registerPrinter(printfQPrinter);
    QPrint::registerPrinter(this, controlInput); //Register the devicecontrol as a QPrint control input

    //Set bluetooth name value in flash
    nvs.setStringValue("BTDeviceName", "ESP32-NFCTag");
    //Initialize bluetooth as bluetooth should be always on
    startBluetooth();

    //nvs.reset(); //for debug

    //For init wifi
    //nvs.setStringValue("WiFiSSID", "Asuna");
    //nvs.setStringValue("WiFiPass", "kuffertpapir2017");
    //nvs.setStringValue("RadioLink", "http://live-icy.gslb01.dr.dk/A/A05L.mp3");

    //For no init wifi
    //nvs.setStringValue("WiFiSSID", "");
    //nvs.setStringValue("WiFiPass", "");
    //nvs.setStringValue("RadioLink", "");

    //connectToSavedWiFi();
}

void DeviceControl::startBluetooth() {
    btHandler = new BluetoothHandler();
    btHandler->setI2SOutput(&i2sOutput);
    std::string btName = nvs.getStringValue("BTDeviceName");
    if (btName == "") {
        btName = "Kris-ESP32";
    }
    btHandler->setDeviceName(btName);
    btHandler->setDeviceController(this);

    esp_bd_addr_t owner;
    uint32_t read = nvs.getBinaryValue("BluetoothOwner", owner, ESP_BD_ADDR_LEN);
    printf("Owner: ");
    for(int i = 0; i < 6; i++) {
        printf("%02X ", owner[i]);
    }
    printf("\n");
    
    int i;
    for(i = 0; i < ESP_BD_ADDR_LEN; i++) {
        if(owner[i] != 0) {
            break;
        }
    }
    if (read != ESP_BD_ADDR_LEN || i == 6) {
        btHandler->setDiscoverable(true);
    } else {
        btHandler->setDiscoverable(false);
        btHandler->setOwner(owner);
    }
}

/*
void DeviceControl::connectToSavedWiFi() {
    std::string wifissid = nvs.getStringValue("WiFiSSID");
    if (!wifissid.empty()) {
        if (wiFiHandler == nullptr) {
            wiFiHandler = new WiFiHandler();
        }
        std::string passkey = nvs.getStringValue("WiFiPass");

        WiFiInfo wifi;
        wifi.ssid = wifissid;
        wifi.passkey = passkey;
        wiFiHandler->connect(&wifi);

        std::string savedRadio = nvs.getStringValue("RadioLink");
        if (!savedRadio.empty()) {
            startRadioStream(savedRadio);
        }
    }
}


void DeviceControl::startRadioStream(std::string link) {
    //Check length is more than HTTP://
    if (link.length() < 7) {
        QPrint::println("Invalid link was input", controlOutput, true);
        return;
    }
    //Convert http to lowercase
    for(int i = 0; i < 4; i++) {
        link.at(0) = std::tolower(link.at(0));
    }
    if (strncmp(link.c_str(), "http:", 5) != 0) {
        QPrint::println("Invalid link was input. Not http", controlOutput, true);
        return;
    }

    std::string streamUrl = link.substr(7);

    btHandler->stopAudio();
    ICESleep(6000); //Give a little time for BT audio to shut down
    mp3Decoder = new StreamDecoder(&i2sOutput);
    ICECastClient = new IcecastHTTPClient(mp3Decoder);
    ICECastClient->startStream(streamUrl);
}

WiFiInfo* DeviceControl::scanWifis(int* foundNetworks) {
    if (wiFiHandler == nullptr) {
        wiFiHandler = new WiFiHandler;
    }

    WiFiInfo* wifis = new WiFiInfo[maxWifisToScan];
    *foundNetworks = wiFiHandler->scan(wifis, maxWifisToScan);

    return wifis;
}

void DeviceControl::stopRadioStream() {
    if (ICECastClient != nullptr) {
        ICECastClient->stopStream();
        ICECastClient->stop();
        delete ICECastClient;
        ICECastClient = nullptr;
    }
    if (mp3Decoder != nullptr) {
        delete mp3Decoder;
        mp3Decoder = nullptr;
    }
}
*/

void DeviceControl::print(std::string input) {
    switch(deviceState) {
        case DEVICE_IDLE:
            processIdleInput(input);
            break;
        /*case DEVICE_CHOOSING_WIFI:
            processWiFiChoice(input);
            break;
        case DEVICE_ENTER_WIFI_PASS:
            processWiFiPass(input);
            break;
        */
    }
}

void DeviceControl::processIdleInput(std::string input) {
    std::vector<std::string> tokenizedInput = ICETokenize(input, " ");
    /*if (tokenizedInput.front() == "SCANWIFI") {
        wifiNetworks = scanWifis(&foundWiFis);
        if (foundWiFis == 0) {
            QPrint::println("No WiFi networks were found... :/", controlOutput, true);
            return;
        }
        printWifis();
        deviceState = DEVICE_CHOOSING_WIFI;
    } 
    else if (tokenizedInput.front() == "STREAM") {
        if (wiFiHandler == nullptr || !wiFiHandler->isConnected()) {
            QPrint::println("Not connected to WiFi. Please connect to WiFi before starting a stream. Wait a few seconds for WiFi to connect.", controlOutput, true);
        }
        if (tokenizedInput.size() <= 1) {
            QPrint::println("No stream link provided. Please input link", controlOutput, true);
            return;
        }
        stopRadioStream(); //Make sure if we were playing radio, to stop the old one...
        startRadioStream(tokenizedInput.at(1));
        QPrint::println("Command OK", controlOutput, true);
    } else if (tokenizedInput.front() == "STREAMNO") {
        if (wiFiHandler == nullptr || !wiFiHandler->isConnected()) {
            QPrint::println("Not connected to WiFi. Please connect to WiFi before starting a stream. Wait a few seconds for WiFi to connect.", controlOutput, true);
        }

        if (tokenizedInput.size() <= 1) {
            QPrint::println("No Number provided. List of channels are:", controlOutput, true);
            QPrint::println("1: DR P1", controlOutput, true);
            QPrint::println("2: Romanian radio station", controlOutput, true);
            QPrint::println("3: DR P3", controlOutput, true);
            return;
        }

        stopRadioStream(); //Make sure if we were playing radio, to stop the old one...
        if (tokenizedInput.at(1) == "1") {
            startRadioStream("http://live-icy.gslb01.dr.dk/A/A03L.mp3");
        } else if (tokenizedInput.at(1) == "2") {
            startRadioStream("http://89.205.123.70/;?type=http&nocache=26141");
        } else if (tokenizedInput.at(1) == "3") {
            startRadioStream("http://live-icy.gslb01.dr.dk/A/A05L.mp3");
        } else {
            QPrint::println("Invalid number", controlOutput, true);
            return;
        }
        QPrint::println("OK", controlOutput, true);
    } */
    if (tokenizedInput.front() == "BLUETOOTH") {
        //stopRadioStream();
        ICESleep(800); //Give some time for I2S to clear buffers and such
        btHandler->startAudio();
        QPrint::println("OK", controlOutput, true);
    } else if (tokenizedInput.front() == "PLAY") {
        if (btHandler->avrcController.play()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "PAUSE") {
        if (btHandler->avrcController.pause()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "NEXT") {
        if (btHandler->avrcController.next()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "PREV") {
        if (btHandler->avrcController.previous()) {
            QPrint::println("OK", controlOutput, true);
        } else {
            QPrint::println("NOT OK", controlOutput, true);
        }
    } else if (tokenizedInput.front() == "HELP") {
        QPrint::println("SCANWIFI\nSTREAM\nSTREAMNO\nBLUETOOTH\nNEXT\nPREV\nPAUSE\nPLAY\nHELP", controlOutput, true);
    } else {
        QPrint::println("Invalid command?", controlOutput, true);
    }
}

/*void DeviceControl::printWifis() {
    QPrint::println("Please type the number of the WiFi network to attempt connection.", controlOutput, true);
    for (int i = 0; i < foundWiFis; i++) {
        QPrint::println(std::to_string(i) + ": " + wifiNetworks[i].ssid, controlOutput, true);
    }
}

void DeviceControl::processWiFiChoice(std::string input) {
    chosenWiFi = stoi(input);
    if (chosenWiFi > foundWiFis) {
        delete[] wifiNetworks;
        foundWiFis = 0;
        deviceState = DEVICE_IDLE;
        QPrint::println("Input WiFi number does not exist! Aborting.", controlOutput, true);
        return;
    }

    if (wifiNetworks->authenticationMode != WIFI_AUTH_OPEN) {
        QPrint::println("Please input the password for the WiFi network", controlOutput, true);
        deviceState = DEVICE_ENTER_WIFI_PASS;
    } else {
        QPrint::println("Connecting to WiFi!", controlOutput, true);
        wiFiHandler->connect(&wifiNetworks[chosenWiFi]);
        nvs.setStringValue("WiFiSSID", wifiNetworks[chosenWiFi].ssid);
        nvs.setStringValue("WiFiPass", wifiNetworks[chosenWiFi].passkey);
        delete[] wifiNetworks;
        deviceState = DEVICE_IDLE;
    }
}

void DeviceControl::processWiFiPass(std::string input) {
    wifiNetworks[chosenWiFi].passkey = input;
    wiFiHandler->connect(&wifiNetworks[chosenWiFi]);
    nvs.setStringValue("WiFiSSID", wifiNetworks[chosenWiFi].ssid);
    nvs.setStringValue("WiFiPass", wifiNetworks[chosenWiFi].passkey);
    deviceState = DEVICE_IDLE;
    delete[] wifiNetworks;
}
*/

void DeviceControl::setBluetoothOwner(esp_bd_addr_t deviceAddress) {
    nvs.setBinaryValue("BluetoothOwner", deviceAddress, ESP_BD_ADDR_LEN);
    btHandler->setDiscoverable(false);
}

void DeviceControl::removeBluetoothOwner() {
    uint8_t zero[ESP_BD_ADDR_LEN];
    memset(zero, 0, ESP_BD_ADDR_LEN);
    nvs.setBinaryValue("BluetoothOwner", zero, ESP_BD_ADDR_LEN);
    btHandler->setDiscoverable(true);
}






