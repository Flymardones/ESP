#include "driver/i2s.h"

#pragma once

class I2SOutput {
    private:
        i2s_port_t I2SPort = I2S_NUM_0;
        i2s_bits_per_sample_t bitWidth = I2S_BITS_PER_SAMPLE_16BIT;
        uint32_t sampleRate = 48000;
        i2s_channel_t channelMode = I2S_CHANNEL_STEREO;

        void configureI2S();

    public:
        void runTask(void* unused);
        I2SOutput();
        ~I2SOutput();

        void setSampleRate(uint32_t sampleRate);
        int getSampleRate();
        bool writeDirect(const uint8_t* data, uint32_t size);
};