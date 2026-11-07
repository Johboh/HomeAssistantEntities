# Small debian-slim Python base image (~46MB) avoiding large Ubuntu images
FROM python:3-slim

# Install system dependencies required by arduino-cli and ESP32 compiler tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    ca-certificates \
    python3-serial \
    && rm -rf /var/lib/apt/lists/*

# Install Arduino CLI
RUN wget -q https://downloads.arduino.cc/arduino-cli/arduino-cli_latest_Linux_64bit.tar.gz && \
    tar xf arduino-cli_latest_Linux_64bit.tar.gz -C /usr/local/bin && \
    rm arduino-cli_latest_Linux_64bit.tar.gz

# Pre-install ESP32 core and dependencies in early layer so code changes don't re-trigger downloads
RUN arduino-cli config init && \
    arduino-cli config set board_manager.additional_urls "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json" && \
    arduino-cli config set library.enable_unsafe_install true && \
    arduino-cli core update-index && \
    arduino-cli core install esp32:esp32@3.3.12 && \
    arduino-cli lib install MQTT@2.5.2 && \
    arduino-cli lib install MQTTRemote@6.0.5 && \
    arduino-cli lib install ArduinoJson@7.4.3

# Copy workspace files last for effective caching
WORKDIR /workspace
COPY . /workspace

# Link library so arduino-cli discovers HomeAssistantEntities
RUN mkdir -p /root/Arduino/libraries && \
    ln -s /workspace /root/Arduino/libraries/HomeAssistantEntities

# Build all Arduino examples
RUN arduino-cli compile --jobs 4 --fqbn "esp32:esp32:esp32:PartitionScheme=default_8MB,FlashSize=8M" examples/arduino/actuators && \
    arduino-cli compile --jobs 4 --fqbn "esp32:esp32:esp32:PartitionScheme=default_8MB,FlashSize=8M" examples/arduino/sensors && \
    arduino-cli compile --jobs 4 --fqbn "esp32:esp32:esp32:PartitionScheme=default_8MB,FlashSize=8M" examples/arduino/child_devices && \
    arduino-cli compile --jobs 4 --fqbn "esp32:esp32:esp32:PartitionScheme=default_8MB,FlashSize=8M" examples/arduino/gateway_and_lifecycle

CMD ["echo", "All Arduino examples built successfully!"]
