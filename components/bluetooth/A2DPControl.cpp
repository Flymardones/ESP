#include "A2DPControl.hpp"
#include "BluetoothHandler.hpp"

void A2DPControl::control_callback(esp_a2d_cb_event_t event, esp_a2d_cb_param_t* parameter) {
    switch (event) {
    case ESP_A2D_CONNECTION_STATE_EVT:
        connectionEvent(parameter);
        break;
    case ESP_A2D_AUDIO_STATE_EVT:
        stateEvent(parameter);
        break;
    case ESP_A2D_AUDIO_CFG_EVT:
        configEvent(parameter);
        break;
    
    default:
        printf("Received unknown A2DP event: %d\n", event);
        break;
    }
}

void A2DPControl::data_callback(const uint8_t* data, uint32_t size) {
    if (!enabled) {
        return;   //If we are not enabled and someone tries to play music, we just do nothing..
    }

    if (i2s != nullptr) {
        i2s->writeDirect(data, size);
    }
}

void A2DPControl::setI2SOutput(I2SOutput* output) {
    this->i2s = output;
}

void A2DPControl::connectionEvent(esp_a2d_cb_param_t* parameter) {
    if (parameter->conn_stat.state == ESP_A2D_CONNECTION_STATE_DISCONNECTED) {
        QPrint::println("A2DP was disconnected");
        connected = false;
    } else if (parameter->conn_stat.state == ESP_A2D_CONNECTION_STATE_CONNECTED) {
        connected = true;
        QPrint::println("A2DP was connected");
    }
}

void A2DPControl::stateEvent(esp_a2d_cb_param_t* parameter) {
    if (parameter->audio_stat.state == ESP_A2D_AUDIO_STATE_STARTED) {
        //Maybe we want to do something when the audio starts/stops in the future..
    }
}

void A2DPControl::configEvent(esp_a2d_cb_param_t* parameter) {
    if (!enabled) {
        return;   //If we are not enabled and someone tries to play music, we just do nothing..
    }
        if (parameter->audio_cfg.mcc.type == ESP_A2D_MCT_SBC) {
            uint8_t esp_sbc_header = parameter->audio_cfg.mcc.cie.sbc[0];

            if (esp_sbc_header & A2DP_48000_mask) {
                i2s->setSampleRate(48000);
                samplerate = 48000;
            } else if (esp_sbc_header & A2DP_44100_mask) {
                i2s->setSampleRate(44100);
                samplerate = 44100;
            } else if (esp_sbc_header & A2DP_32000_mask) {
                i2s->setSampleRate(32000);
                samplerate = 32000;
            } else {
                i2s->setSampleRate(16000);
                samplerate = 16000;
            }
        }
}

void A2DPControl::disableOutput() {
    enabled = false;
}

void A2DPControl::enableOutput() {
    enabled = true;
    i2s->setSampleRate(samplerate);
}

bool A2DPControl::isConnected() {
    return connected;
}
