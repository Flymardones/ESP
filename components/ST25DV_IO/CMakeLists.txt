idf_component_register(
        SRCS
        "st25dv_io.cpp"

        INCLUDE_DIRS
        "include"

        REQUIRES ST25DV_IO libNDEF FreeRTOSIntegration QPrinter DeviceControl

)