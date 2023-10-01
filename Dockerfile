FROM ubuntu:22.04 as esp_base

ENV DEBIAN_FRONTEND=noninteractive

# Use /bin/bash instead of /bin/sh
SHELL ["/bin/bash", "-c"]

RUN echo "Installing Prerequisites"

# Install prerequisites for esp-idf

RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    wget \
    flex \
    bison \
    gperf \
    python3 \
    python3-pip \
    python3-venv \
    cmake \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0

RUN echo "Get ESP-IDF repo"

RUN mkdir -p ~/esp && \
    cd ~/esp && \
    git clone --recursive https://github.com/espressif/esp-idf.git


RUN echo "Setup Tools"

RUN cd ~/esp/esp-idf && \
    ./install.sh esp32


RUN echo "Setup Environment Variables"

RUN . $HOME/esp/esp-idf/export.sh && \
    echo "alias get_idf='. $HOME/esp/esp-idf/export.sh'" >> ~/.bashrc && \
    source ~/.bashrc