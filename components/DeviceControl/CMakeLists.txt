idf_component_register(
    SRCS 
    "DeviceControl.cpp"

    INCLUDE_DIRS 
    "include"

    REQUIRES FreeRTOSIntegration bluetooth I2S AudioDecoding QPrinter WiFi NVS
)


