#include "I2SOutput.hpp"
#include "ICESleep.hpp"
#include "QPrint.hpp"

I2SOutput::I2SOutput() {
    configureI2S();
}

I2SOutput::~I2SOutput() {
    i2s_driver_uninstall(I2SPort);
}

void I2SOutput::configureI2S() {
    //The DMA buffer is set to the same size as the ringbuffer, 4*1024*16/8 = 16k.
    //This allows us to hold 85ms of audio at 48khz.
    i2s_config_t config = {
        .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = sampleRate,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .tx_desc_auto_clear = true                                              //Auto clear tx descriptor on underflow
    };

    if (i2s_driver_install(I2SPort, &config, 0, NULL) != ESP_OK) {
        QPrint::println("I2S Driver install failed!\n");
    }

    i2s_pin_config_t pin_config = {
        .mck_io_num = 0,
        .bck_io_num = 4,
        .ws_io_num = 2,
        .data_out_num = 15,
        .data_in_num = -1 //We are only outputting..
    };

    i2s_set_pin(I2SPort, &pin_config);
}

void I2SOutput::setSampleRate(uint32_t sampleRate) {
    if (sampleRate > 8000 && sampleRate < 100000) { //Sanity check
        this->sampleRate = sampleRate;
        i2s_set_clk(I2SPort, sampleRate, bitWidth, channelMode);
    }
}

int I2SOutput::getSampleRate() {
    return this->sampleRate;
}

bool I2SOutput::writeDirect(const uint8_t *data, uint32_t size) {
    size_t writtenBytes = 0;
    esp_err_t status = i2s_write(I2SPort, data, size, &writtenBytes, 1); //Something is quite wrong with clocking if we end up waiting here..
    if (status != ESP_OK) {
        return false;
    }

    return true;
}
