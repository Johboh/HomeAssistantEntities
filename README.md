# HomeAssistantEntities
[![PlatformIO CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/platformio.yaml/badge.svg)](https://registry.platformio.org/libraries/johboh/HomeAssistantEntities)
[![ESP-IDF CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/espidf.yaml/badge.svg)](https://components.espressif.com/components/johboh/homeassistantentities)
[![Arduino IDE](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/arduino_cli.yaml/badge.svg)](https://downloads.arduino.cc/libraries/logs/github.com/Johboh/HomeAssistantEntities/)
[![GitHub release](https://img.shields.io/github/release/Johboh/HomeAssistantEntities.svg)](https://github.com/Johboh/HomeAssistantEntities/releases)
[![Clang-format](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/clang-format.yaml/badge.svg)](https://github.com/Johboh/HomeAssistantEntities)

Arduino (using Arduino IDE or PlatformIO) and ESP-IDF (using Espressif IoT Development Framework or PlatformIO) compatible library for providing sensors and actuators to Home Assistant using MQTT.

Home Assistant has a well defined way of defining sensors and actuators using MQTT. This library registers a device with one or multiple sensors and provides a way to publish new values for them, as well as listen for commands from Home Assistant.

### Currently supported sensors*
- Atmospheric Pressure (cbar, bar, hPa, mmHg, inHg, kPa, mbar, Pa, psi)
- Boolean
- Binary sensor (Generic sensor, with any supported [device class](https://www.home-assistant.io/integrations/binary_sensor/#device-class) using a device class from [HaDeviceClasses.h](./src/entities/HaDeviceClasses.h))
- Brightness (%)
- Carbon Dioxide (ppm)
- Carbon Monoxide (ppm, ppb, mg/m³, μg/m³)
- Current (A, mA, μA)
- Distance (km, m, cm, mm, mi, nmi, yd, in, ft)
- Door (open/closed)
- Energy (J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal, thm)
- Humidity (%)
- Illuminance (lx)
- Json (raw "json"-sensor)
- Motion (detected/not detected)
- Particulate matter (μg/m³)
- Power (mW, W, kW, MW, GW, TW)
- Signal strength/RSSI (dB, dBm)
- Sound (detected/not detected)
- String (raw "String"-sensor)
- Temperature (°C, °F, K)
- Timestamp
- Unit Concentration (parts per volume)
- Volatile organic compounds (µg/m³, ppb)
- Voltage (V, mV, µV)
- Weight (kg, g, mg, ug, oz, lb, st)
- Sensor (Generic composable sensor via [HaEntitySensor.h](./src/entities/HaEntitySensor.h), with any supported [device class](https://www.home-assistant.io/integrations/sensor/#device-class) and unit of measurement from [HaDeviceClasses.h](./src/entities/HaDeviceClasses.h))

### Currently supported actuators (and controls)*
- Actuator (Generic composable actuator via [HaEntityActuator.h](./src/entities/HaEntityActuator.h), with typed device classes from [HaDeviceClasses.h](./src/entities/HaDeviceClasses.h))
- Button (press, device classes `identify`, `restart`, `update`)
- Cover (open/opening, close/closing, stop/stopped, position, tilt, typed cover device classes)
- Device trigger
- Event
- Fan (on/off, speed percentage, presets, direction, oscillation)
- Light (brightness, color temperature in Kelvin/mireds, rgb, effect)
- Lock (lock/unlock/open commands, locked/unlocked/locking/unlocking/jammed/opening/open states, PIN `code_format`)
- Number (value, slider/box, step, min/max, unit, device class)
- Select (options list)
- Switch (on/off, device classes `outlet`, `switch`)
- Text (text input, regex pattern, password mode)
- Update (installed/latest firmware or software version, release notes, progress, OTA `INSTALL` command)
- Valve (open/opening, close/closing, stop/stopped, position, device classes `water`, `gas`)

*_If you can't find a sensor/actuator you need, either use the generic composable `HaEntitySensor` / `HaEntityActuator`, create a pull request to add one, or raise an issue on GitHub and we'll add it to this library._

### Device Topologies & Home Assistant MQTT Discovery
In addition to standard single-component discovery (`entity.publishConfiguration()`), [HaBridge.h](./src/HaBridge.h) and [HaDeviceInformation.h](./src/HaDeviceInformation.h) support the full [Home Assistant MQTT Discovery specification](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery):

- **Child devices (`child_device` / `child_devices`)**: Create logical sub-units on a single physical device (e.g., individual outlets on a smart power strip or multi-channel relays) with `ha_bridge.childDevice(child_node_id, child_identifier, child_name)`. Entities bound to a child bridge scope their state/command topics under the child while linking to the parent device in Home Assistant.
- **Gateways & sub-devices (`via_device`)**: Model separate physical sub-devices (BLE/Zigbee/RF nodes) routed through a hub by setting `HaBridge::DeviceInfo::via_device` to the hub's identifier (plus `connections`, `suggested_area`, `configuration_url`, `serial_number`, etc.) via `HaBridge::populateDeviceJson(...)`.
- **Unified device-based discovery & migration**: Publish an entire device (including all components and `child_devices`) in a single MQTT discovery payload (`<prefix>/device/<node_id>/config`) with `ha_bridge.publishDeviceConfiguration({sensor_a, switch_b, ...})`. Existing deployments using per-entity discovery can migrate without losing entity IDs or history via `ha_bridge.migrateToDeviceConfiguration(...)`.
- **Automatic ESP hardware & firmware metadata**: Populate `sw_version`, `hw_version` (chip model & silicon revision), MAC `connections`, `configuration_url` (local IPv4), and `HaBridge::OriginInfo` automatically on ESP-IDF, Arduino ESP32, and ESP8266 using `HaDeviceInformation::populateDeviceJson(...)` and `HaDeviceInformation::detectOriginInfo(...)` from [HaDeviceInformation.h](./src/HaDeviceInformation.h).
- **Lifecycle, birth messages & MQTT Subentries**:
  - Subscribe to Home Assistant restarts (`homeassistant/status`) with `ha_bridge.onHomeAssistantBirth(...)` to re-publish discovery and live state (used alongside your MQTT connection callback, since Home Assistant's birth message is published with `retain: false` by default).
  - Dynamically remove entities or whole devices from Home Assistant with `entity.unpublishConfiguration()` / `ha_bridge.unpublishDeviceConfiguration()`.
  - Customize discovery prefix (`setDiscoveryPrefix`), availability payloads (`setAvailabilityPayloads`), and entity metadata (`entity_category`, `icon`, `expire_after`, `off_delay`, `suggested_display_precision`, `optimistic`, `enabled_by_default`).
  - Inspect exact state, command, availability, and discovery topics via `getTopic(...)`, `getAvailabilityTopic()`, `getDiscoveryTopic(...)`, and `getDeviceDiscoveryTopic()` when configuring Home Assistant MQTT Subentries manually.

### Installation
#### PlatformIO (Arduino or ESP-IDF):
Add the following to `lib_deps`:
```
   Johboh/HomeAssistantEntities@^10.0.0
```
You also need a MQTT library and a JSON library. See Dependencies section. Suggestion:
```
   johboh/MQTTRemote@^6.0.5
   bblanchon/ArduinoJson@^7.4.3
```

### Arduino IDE
Search for `HomeAssistantEntities` by johboh in the library manager. You also need a MQTT library and a JSON library. See Dependencies section. Suggestion: `MQTTRemote` by `johboh` and `ArduinoJson` by `bblanchon`.

#### Espressif IoT Development Framework:
In your existing `idf_component.yml` or in a new `idf_component.yml` next to your main component:
```
dependencies:
  johboh/homeassistantentities:
    version: ">=10.0.0"
```

You also need a MQTT library. See Dependencies section. Suggestion:
```
  johboh/mqttremote:
    version: ">=6.0.5"
```

### Examples
- [Arduino: Sensors](examples/arduino/sensors/sensors.ino)
- [Arduino: Actuators](examples/arduino/actuators/actuators.ino)
- [Arduino: Child Devices & Device Discovery](examples/arduino/child_devices/child_devices.ino)
- [Arduino: Gateway (`via_device`) & Discovery Lifecycle](examples/arduino/gateway_and_lifecycle/gateway_and_lifecycle.ino)
- [ESP-IDF: Sensors](examples/espidf/sensors/main/main.cpp)
- [ESP-IDF: Actuators](examples/espidf/actuators/main/main.cpp)
- [ESP-IDF: All Entities](examples/espidf/all/main/main.cpp)
- [ESP-IDF: Child Devices & Device Discovery](examples/espidf/child_devices/main/main.cpp)
- [ESP-IDF: Gateway (`via_device`) & Discovery Lifecycle](examples/espidf/gateway_and_lifecycle/main/main.cpp)

### Functionality verified on the following platforms and frameworks
- ESP32 (tested with PlatformIO [espressif32@6.4.0](https://github.com/platformio/platform-espressif32) / [arduino-esp32@2.0.11](https://github.com/espressif/arduino-esp32) / [ESP-IDF@4.4.6](https://github.com/espressif/esp-idf) / [ESP-IDF@5.1.2](https://github.com/espressif/esp-idf) on ESP32-S2 and ESP32-C3), [ESP-IDF@5.4.1](https://github.com/espressif/esp-idf) on ESP32-S2 and ESP32-C6)
- ESP8266 (tested with PlatformIO [espressif8266@4.2.1](https://github.com/platformio/platform-espressif8266) / [arduino-core@3.2.0](https://github.com/esp8266/Arduino))

Newer versions most probably work too, but they have not been verified.

### Dependencies
- For ESP-IDF: https://github.com/Johboh/nlohmann-json @^3.12.0, for Arduino: https://github.com/Johboh/nlohmann-json @^3.12.0 OR https://github.com/bblanchon/ArduinoJson@^7.4.3
- *An MQTT implementation is required.* There is a copy of [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/includes/IMQTTRemote.h) in this library from [Johboh/MQTTRemote](https://github.com/Johboh/MQTTRemote). You can either add a dependency on [MQTTRemote](https://github.com/Johboh/MQTTRemote) to get a fully working MQTT client (the examples are using this dependency), or you can implement/adapt/forward to your own MQTT implementation. This library only depends on the [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/includes/IMQTTRemote.h) interface.
- Needs C++17 for `std::optional`.
  - For PlatformIO in `platformio.ini`:
    ```ini
    build_unflags=-std=gnu++11 # "Disable" C++11
    build_flags=-std=gnu++17 # "Enable" C++17
    ```
- When using ESP32 with `nlohmann-json`, the compiled binary size can exceed the default 1.25MB partition. Use a larger partition scheme such as `min_spiffs.csv` (1.9MB with OTA) or `huge_app.csv` (3MB no OTA):
  - In `platformio.ini`:
    ```ini
    board_build.partitions = min_spiffs.csv
    ```
  - In Arduino IDE: Tools -> Partition Scheme -> "Minimal SPIFFS (Large APPS with OTA)" or "Huge APP (3MB No OTA/1MB SPIFFS)".

### License
Licensed under the MIT License
