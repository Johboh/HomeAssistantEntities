# Small debian-slim Python base image (~46MB) avoiding large Ubuntu images
FROM python:3-slim

# Install system dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Install PlatformIO
RUN pip install --no-cache-dir --upgrade platformio

# Pre-install platforms and external libraries so code edits do not re-trigger package downloads
RUN pio pkg install --global --platform "espressif8266" && \
    pio pkg install --global --platform "espressif32" && \
    pio pkg install --global --library "johboh/MQTTRemote8266@^6.0.5" && \
    pio pkg install --global --library "johboh/MQTTRemote@^6.0.5" && \
    pio pkg install --global --library "bblanchon/ArduinoJson@^7.1.0" && \
    pio pkg install --global --library "johboh/nlohmann-json@^3.12.0"

# Copy library and examples last for effective caching
WORKDIR /workspace
COPY . /workspace

# Build all PlatformIO matrix targets (ESP8266 + ArduinoJson, ESP32 + ArduinoJson, ESP32 + nlohmann-json)
RUN for ex in actuators sensors child_devices gateway_and_lifecycle; do \
      echo "=== Building PlatformIO ESP8266 d1_mini: ${ex} ===" && \
      PLATFORMIO_CI_SRC="examples/arduino/${ex}" pio ci --lib="." --board=d1_mini \
        --project-option="build_unflags=-std=gnu++11" \
        --project-option="build_flags=-std=gnu++17" \
        --project-option="lib_ldf_mode=deep" \
        --project-option="lib_deps=johboh/MQTTRemote8266@^6.0.5, bblanchon/ArduinoJson@^7.1.0" && \
      echo "=== Building PlatformIO ESP32 lolin_c3_mini: ${ex} ===" && \
      PLATFORMIO_CI_SRC="examples/arduino/${ex}" pio ci --lib="." --board=lolin_c3_mini \
        --project-option="build_unflags=-std=gnu++11" \
        --project-option="build_flags=-std=gnu++17" \
        --project-option="lib_ldf_mode=deep" \
        --project-option="lib_deps=johboh/MQTTRemote@^6.0.5, johboh/nlohmann-json@^3.12.0" \
        --project-option="board_build.partitions=min_spiffs.csv" || exit 1; \
    done

CMD ["echo", "All PlatformIO targets built successfully!"]
