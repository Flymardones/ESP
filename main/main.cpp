//#include "st25dv.hpp"
#include "ST25DVSensor.h"

#define I2C_SLAVE_MAX_SPEED 400000
#define TX_RX_BUF_SIZE 1024
#define BLUETOOTH 1

QueueHandle_t interruptQueue = NULL;
ST25DV st25dv;
DeviceControl* deviceControl;

static void IRAM_ATTR gpio_interrupt_handler(void* args) {
    uint32_t pin_number = (uint32_t) args;
    xQueueSendFromISR(interruptQueue, &pin_number, NULL);
}

void reconnect(DeviceControl* deviceControl) {
    deviceControl->btHandler->reconnect(); //Try to reconnect to owner every 10 seconds if connection is lost and we have a owner.
}

// Remove bluetooth owner when scanning nfc tag
void set_new_owner(void* params){
    static uint32_t pin_number;
    static bool removed_owner = false;
    static bool interrupted = false;
    static uint8_t owner[ESP_BD_ADDR_LEN];
    static uint32_t read;

    while(1) {
        if(xQueueReceive(interruptQueue, &pin_number, portMAX_DELAY)) {
            printf("Interrupt Detected!\n");
            if(pin_number == GPIO_NUM_19 && gpio_get_level(GPIO_NUM_19) == 1) {
                gpio_intr_disable(GPIO_NUM_19); // Disable gpio intr to ensure that it only connects once
                deviceControl->intr_status = 0;
                printf("Disabled interrupt on gpio pin 19\n");
                //interrupted = true;
                read = deviceControl->nvs.getBinaryValue("BluetoothOwner", owner, ESP_BD_ADDR_LEN);
                deviceControl->btHandler->setOwner(owner); // set owner for reconnect function
                printf("Got binary value for BT owner\n");
                if ((uint32_t) owner[0] != 0 && (uint16_t) owner[4] != 0) { // If there is no owner do nothing
                    printf("Owner to disconnect: ");
                    for(int i = 0; i < ESP_BD_ADDR_LEN; i++){
                        printf("%02X ", owner[i]);
                    }
                    printf("\n");
                    printf("Removing Bluetooth Owner\n");
                    if(esp_a2d_sink_disconnect(owner) != ESP_OK){
                        printf("Failed to disconnect a2p\n");
                    }
                    //ICESleep(1000);
                }
            }
        }
    }
}

extern "C" void app_main(void)
{
    // Setup bluetooth
    #if BLUETOOTH
    deviceControl = new DeviceControl();

    // Reset bluetooth owner as it is stored in nvs partition
    deviceControl->removeBluetoothOwner();

    // Get esp32 bluetooth mac address
    const uint8_t *bt_address = esp_bt_dev_get_address();
    
    if(bt_address == NULL) {
        printf("Failed to get BT Address!\n");
    }
    else {
        printf("Got the following BT Address: ");
        for(int i = 0; i < 6; i++) {
            printf("%02X", bt_address[i]);
        }
        printf("\n");
    }

    Ndef_Bluetooth_OOB_t bt_oob = {
        .Type = NDEF_BLUETOOTH_BREDR,
        .DeviceAddress = {bt_address[0], bt_address[1], bt_address[2], bt_address[3], bt_address[4], bt_address[5]},
        .OptionalMask = NDEF_BLUETOOTH_OPTION(BLUETOOTH_EIR_COMPLETE_LOCAL_NAME) |
                        NDEF_BLUETOOTH_OPTION(BLUETOOTH_EIR_DEVICE_CLASS) |
                        NDEF_BLUETOOTH_OPTION(BLUETOOTH_EIR_SERVICE_CLASS_UUID_COMPLETE_16),
        .ClassUUID16 = {0x111E, 0x110B},
        .nbUUID16 = 2,
        .LocalName = "ST25DV",
        .DeviceClass = {0x0A, 0x04, 0x20}
    };

    #endif

    gpio_config_t* ioConfig = new gpio_config_t;
    ioConfig->intr_type = GPIO_INTR_NEGEDGE;
    ioConfig->mode = GPIO_MODE_INPUT;
    ioConfig->pin_bit_mask = 1ULL<<GPIO_NUM_19;
    ioConfig->pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConfig->pull_up_en = GPIO_PULLUP_ENABLE;
    if(gpio_config(ioConfig) != ESP_OK) {
        printf("Failed to config gpio!\n");
    }
    delete ioConfig;

    interruptQueue = xQueueCreate(10, sizeof(int));

    xTaskCreate(set_new_owner, "set_new_owner", 2048, NULL, 1, NULL);

    if(gpio_install_isr_service(0) != ESP_OK) {
        printf("Failed to install isr service!\n");
    }

    if(gpio_isr_handler_add(GPIO_NUM_19, gpio_interrupt_handler, (void*) GPIO_NUM_19) != ESP_OK) {
        printf("Failed to add isr handler\n");
    }
    
    // Configure I2C controller 0 for master
    i2c_config_t conf0 = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,
        .scl_io_num = 22,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {.clk_speed = I2C_SLAVE_MAX_SPEED},
    };
    
    if (i2c_param_config(I2C_NUM_0, &conf0) != ESP_OK) {
        printf("I2C config failed\n");
    }
    else {
        printf("I2C config OK!\n");
    }
    
    if (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) != ESP_OK ) {
        printf("I2C driver install failed\n");
    }
    else {
        printf("I2C driver install OK!\n");
    }
    

    if(st25dv.begin() != ESP_OK) { // Init ST25DV module
        printf("NFC Tag Init Failed!\n");
    }
    else {
        printf("NFC Tag Init OK!\n");
    }

    // Write Bluetooth BR/EDR OOB NDEF message
    if (st25dv.writeBluetoothOOB(&bt_oob, NULL) != ESP_OK){
        printf("Failed to append BT OOB!\n");
    }

    #if BLUETOOTH
    while(1) {
        static uint32_t read;
        static uint8_t owner[ESP_BD_ADDR_LEN];

        // QPrint::println("Free heap size: " + std::to_string(xPortGetFreeHeapSize()));
        // QPrint::println("Minimum ever heap: " + std::to_string(xPortGetMinimumEverFreeHeapSize()));
        
        if(deviceControl->intr_status == 1) {
            read = deviceControl->nvs.getBinaryValue("BluetoothOwner", owner, ESP_BD_ADDR_LEN);
            printf("Bluetooth Owner: ");
            for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
                printf("%02X ", owner[i]);
            }
            printf("\n");
            //reconnect(deviceControl);
        }
        
        ICESleep(2000);
        
    }
    #endif
}
