# Agent Guidelines for HomeAssistantEntities

This document outlines verification procedures and standards for AI agents and contributors working in this repository.

## Local Docker Testing

Before completing or proposing changes, verify builds locally across the supported embedded ecosystems using the Dockerfiles in `dockerfiles/`:

### 1. Arduino CLI
Builds all Arduino examples (`actuators`, `sensors`, `child_devices`, `gateway_and_lifecycle`) targeting ESP32 (`esp32:esp32@3.3.12`):
```bash
docker build -f dockerfiles/arduino/Dockerfile .
```

### 2. ESP-IDF (Lowest Supported 5.x: release-v5.3)
Builds all 5 ESP-IDF examples (`actuators`, `sensors`, `all`, `child_devices`, `gateway_and_lifecycle`) targeting `esp32c6` on ESP-IDF v5.3:
```bash
docker build -f dockerfiles/espidf_v5/Dockerfile .
# or: docker build -f dockerfiles/espidf/v5/Dockerfile .
```

### 3. ESP-IDF (Highest Supported 6.x: release-v6.1)
Builds all 5 ESP-IDF examples (`actuators`, `sensors`, `all`, `child_devices`, `gateway_and_lifecycle`) targeting `esp32c6` on ESP-IDF v6.1:
```bash
docker build -f dockerfiles/espidf_v6/Dockerfile .
# or: docker build -f dockerfiles/espidf/v6/Dockerfile .
```

### 4. PlatformIO
Builds matrix targets (ESP8266 `d1_mini` with `ArduinoJson`, and ESP32 `lolin_c3_mini` with `nlohmann-json`) across all Arduino examples:
```bash
docker build -f dockerfiles/platformio/Dockerfile .
```

---

## Docker Image & Caching Standards

1. **Avoid Heavy Base Images**:
   - Do not use `ubuntu:latest` or heavy Ubuntu-based images for generic runner containers (`ubuntu` images are large and bloat build times and disk usage).
   - Use small, efficient base images such as `python:3-slim` / `debian:bookworm-slim` (~46-70MB), which provide small footprint and full GNU libc compatibility for cross-compilers (Xtensa GCC, RISC-V GCC).

2. **Alpine Linux & CVE-2026-123158**:
   - If using Alpine Linux images, always ensure packages are upgraded to patch security vulnerabilities, notably **CVE-2026-123158** (the musl libc OpenSSH bypass vulnerability):
     ```dockerfile
     RUN apk upgrade -U -a
     ```
   - Note that embedded cross-toolchains require `glibc`; glibc-compatible slim images (`python:3-slim`) are preferred over musl for embedded toolchains.

3. **Effective Layer Caching**:
   - Structure Dockerfiles so system packages, compiler toolchains, board definitions, and third-party dependencies are pre-installed in early layers.
   - Add repository source code and examples (`COPY`) last so developer code modifications rebuild in seconds without re-downloading toolchains or libraries.

---

## Code Formatting

All C++ source and header files (`*.h`, `*.cpp`, `*.ino`) must strictly conform to `.clang-format`:
```bash
# In PowerShell:
Get-ChildItem -Recurse -Include *.h,*.cpp,*.ino -Path src,examples | ForEach-Object { clang-format -i $_.FullName }

# Verification:
Get-ChildItem -Recurse -Include *.h,*.cpp,*.ino -Path src,examples | ForEach-Object { clang-format --dry-run --Werror $_.FullName }
```
