#pragma once

#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "ICESleep.hpp"
#include "QPrint.hpp"
#include "DeviceControl.hpp"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/i2c.h"
#include "driver/ledc.h"


#define ST25DV_ADDR_DATA_I2C (0xA6 >> 1) // The 7 bit address used to access user data (0xA6 >> 1 = 0xA7 >> 1 = 0x53)
#define ST25DV_ADDR_SYST_I2C (0xAE >> 1) // I2C address to be used for ST25DV System accesses.

esp_err_t st25dv_read_bytes(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *rx_buf, size_t data_len, TickType_t ticks_to_wait);

esp_err_t st25dv_read_byte(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *rx_buf, TickType_t ticks_to_wait);

esp_err_t st25dv_write_bytes(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *tx_buf, size_t data_len, TickType_t ticks_to_wait);

esp_err_t st25dv_write_byte(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *tx_buf, TickType_t ticks_to_wait);

void std25dv_poll(i2c_port_t i2c_num, TickType_t ticks_to_wait);
