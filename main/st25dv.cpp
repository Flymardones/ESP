#include "st25dv.hpp"

esp_err_t st25dv_read_bytes(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *rx_buf, size_t data_len, TickType_t ticks_to_wait)
{
    esp_err_t err;
    uint8_t mem_addr_split[2] = {mem_addr & 0xFF00 >> 8, mem_addr & 0x00FF};

    err = i2c_master_write_to_device(I2C_NUM_0, ST25DV_ADDR_DATA_I2C, mem_addr_split, 2, 1000);
    if (err != ESP_OK)
        return err;

    err = i2c_master_read_from_device(I2C_NUM_0, ST25DV_ADDR_DATA_I2C, rx_buf, data_len, 1000);

    return err;
}

esp_err_t st25dv_read_byte(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *rx_buf, TickType_t ticks_to_wait)
{
    return st25dv_read_bytes(i2c_num, mem_addr, rx_buf, 1, ticks_to_wait);
}

esp_err_t st25dv_write_bytes(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *tx_buf, size_t data_len, TickType_t ticks_to_wait)
{
    esp_err_t err;

    // TODO: Maybe use stack allocation for simplicity if arrays are not too large
    uint8_t *mem_addr_and_data = (uint8_t *)malloc(sizeof(uint8_t) * (2 + data_len));
    mem_addr_and_data[0] = mem_addr & 0xFF00;
    mem_addr_and_data[1] = mem_addr & 0x00FF;
    memcpy((void *)(mem_addr_and_data + 2), (void *)tx_buf, sizeof(uint8_t) * data_len);

    err = i2c_master_write_to_device(I2C_NUM_0, ST25DV_ADDR_DATA_I2C, mem_addr_and_data, 2 + data_len, ticks_to_wait);

    free(mem_addr_and_data);
    return err;
}

esp_err_t st25dv_write_byte(i2c_port_t i2c_num, uint16_t mem_addr, uint8_t *tx_buf, TickType_t ticks_to_wait)
{
    return st25dv_write_bytes(i2c_num, mem_addr, tx_buf, 1, ticks_to_wait);
}

/* void std25dv_poll(i2c_port_t i2c_num, TickType_t ticks_to_wait)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();

    esp_err_t err;
    do
    {
        i2c_master_start(cmd_handle);
        i2c_master_write_byte(cmd_handle, (0x53 << 1) | I2C_MASTER_WRITE, ACK_EN);
        i2c_master_stop(cmd_handle);
        err = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000);
    } 
    while (err == ESP_FAIL);

    // TODO: Test implementation.
} */
