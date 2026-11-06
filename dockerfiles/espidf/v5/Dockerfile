# Official ESP-IDF image for release-v5.3 (lowest supported 5.x)
FROM espressif/idf:release-v5.3

WORKDIR /HomeAssistantEntities

# Use ESP-IDF entrypoint to ensure environment (PATH, tools, venv) is active for RUN commands
SHELL ["/opt/esp/entrypoint.sh", "bash", "-c"]

# Copy workspace last so code changes do not invalidate toolchain layers
COPY . /HomeAssistantEntities

# Build all 5 ESP-IDF examples targeting esp32c6
RUN for ex in sensors actuators all child_devices gateway_and_lifecycle; do \
      echo "=== Building ESP-IDF example: ${ex} ===" && \
      cd /HomeAssistantEntities/examples/espidf/${ex} && \
      idf.py set-target esp32c6 && \
      idf.py build || exit 1; \
    done

CMD ["echo", "All ESP-IDF v5.3 examples built successfully!"]
