# HomeAssistantEntities
[![PlatformIO CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/platformio.yaml/badge.svg)](https://registry.platformio.org/libraries/johboh/HomeAssistantEntities)
[![ESP-IDF CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/espidf.yaml/badge.svg)](https://components.espressif.com/components/johboh/homeassistantentities)
[![Arduino IDE](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/arduino_cli.yaml/badge.svg)](https://downloads.arduino.cc/libraries/logs/github.com/Johboh/HomeAssistantEntities/)
[![GitHub release](https://img.shields.io/github/release/Johboh/HomeAssistantEntities.svg)](https://github.com/Johboh/HomeAssistantEntities/releases)
[![Clang-format](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/clang-format.yaml/badge.svg)](https://github.com/Johboh/HomeAssistantEntities)
[![Documentation](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/pages.yaml/badge.svg)](https://homeassistantentities.johboh.dev/)

Arduino (using Arduino IDE or PlatformIO) and ESP-IDF (using Espressif IoT Development Framework or PlatformIO) compatible library for providing sensors and actuators to Home Assistant using MQTT.

Home Assistant has a well-defined way of defining sensors and actuators using MQTT. This library registers a device with one or multiple sensors and actuators and provides a way to publish new values for them, as well as listen for commands from Home Assistant.

- **GitHub Repository**: [https://github.com/Johboh/HomeAssistantEntities](https://github.com/Johboh/HomeAssistantEntities)
- **API & Class Documentation**: [https://homeassistantentities.johboh.dev/](https://homeassistantentities.johboh.dev/)

### Currently supported sensors*
- [Atmospheric Pressure](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityAtmosphericPressure.h) (cbar, bar, hPa, mmHg, inHg, kPa, mbar, Pa, psi, mPa, inH₂O, atm)
- Binary sensor (generic composable binary sensor via [HaEntitySensor.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySensor.h), with any supported [device class](https://www.home-assistant.io/integrations/binary_sensor/#device-class) from [HaDeviceClasses.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaDeviceClasses.h))
- [Boolean](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityBoolean.h) (generic on/off binary sensor)
- [Brightness](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityBrightness.h) (%)
- [Carbon Dioxide](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityCarbonDioxide.h) (ppm)
- [Carbon Monoxide](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityCarbonMonoxide.h) (ppm, ppb, mg/m³, μg/m³)
- [Current](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityCurrent.h) (A, mA, μA)
- [Distance](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityDistance.h) (km, m, cm, mm, mi, nmi, yd, in, ft)
- [Door](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityDoor.h) (open/closed binary sensor)
- [Energy](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityEnergy.h) (J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal, thm)
- [Humidity](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityHumidity.h) (%)
- [Illuminance](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityIlluminance.h) (lx)
- [Json](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityJson.h) (raw JSON sensor)
- [Motion](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityMotion.h) (detected/clear binary sensor)
- [Particulate Matter](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityParticulateMatter.h) (PM1, PM2.5, PM4, PM10 in µg/m³)
- [Power](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityPower.h) (mW, W, kW, MW, GW, TW)
- [Sensor](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySensor.h) (generic composable sensor with optional JSON attributes and any supported [device class](https://www.home-assistant.io/integrations/sensor/#device-class) and unit of measurement from [HaDeviceClasses.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaDeviceClasses.h))
- [Signal Strength / RSSI](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySignalStrength.h) (dB, dBm)
- [Sound](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySound.h) (detected/clear binary sensor)
- [String](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityString.h) (raw string sensor)
- [Temperature](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityTemperature.h) (°C, °F, K)
- [Timestamp](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityTimestamp.h) (ISO 8601 timestamp)
- [Unit Concentration](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityUnitConcentration.h) (parts per volume: /mL, /dL, /L)
- [Volatile Organic Compounds](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityVolatileOrganicCompounds.h) (µg/m³, ppb)
- [Voltage](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityVoltage.h) (V, mV, µV, kV, MV)
- [Weight](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityWeight.h) (kg, g, mg, µg, oz, lb, st)

### Currently supported actuators (and controls)*
- [Actuator](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityActuator.h) (generic composable actuator with optional state topic, JSON attributes, and typed device classes from [HaDeviceClasses.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaDeviceClasses.h))
- [Button](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityButton.h) (stateless press command; device classes `identify`, `restart`, `update`)
- [Cover](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityCover.h) (open/opening, close/closing, stop/stopped, position, slat tilt, optional read-only mode; device classes `awning`, `blind`, `curtain`, `damper`, `door`, `garage`, `gate`, `shade`, `shutter`, `window`)
- [Device Trigger](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityDeviceTrigger.h) (stateless automation triggers such as `button_short_press`, `button_double_press`, `button_long_press`, etc.)
- [Event](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityEvent.h) (discrete typed events with optional attributes; device classes `button`, `motion`, `doorbell`)
- [Fan](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityFan.h) (on/off, numeric speed range mapped to percentage, preset modes, forward/reverse direction, oscillation)
- [Light](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityLight.h) (on/off, brightness, color temperature in Kelvin or mireds, RGB color, effects)
- [Lock](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityLock.h) (`LOCK`/`UNLOCK`/optional `OPEN` commands; `LOCKED`, `UNLOCKED`, `LOCKING`, `UNLOCKING`, `JAMMED`, `OPENING`, `OPEN` states; optional PIN `code_format`)
- [Number](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityNumber.h) (numeric value control, `auto`/`slider`/`box` mode, `min`/`max`/`step`, unit of measurement, device class)
- [Select](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySelect.h) (selectable options list with active option state)
- [Switch](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntitySwitch.h) (bidirectional on/off control; device classes `outlet`, `switch`)
- [Text](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityText.h) (free-form text input with optional state reporting, `min`/`max` length, regex `pattern`, password mode)
- [Update](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityUpdate.h) (installed/latest firmware or software version, release summary/URL, progress reporting, optional OTA `INSTALL` command; device class `firmware`)
- [Valve](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/entities/HaEntityValve.h) (open/opening, close/closing, optional stop/stopped, discrete or numeric position control; device classes `water`, `gas`)

*_If you can't find a sensor/actuator you need, either use the generic composable `HaEntitySensor` / `HaEntityActuator`, create a pull request to add one, or raise an issue on GitHub and we'll add it to this library._

### Device Topologies & Home Assistant MQTT Discovery
In addition to standard single-component discovery (`entity.publishConfiguration()`), [HaBridge.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/HaBridge.h) and [HaDeviceInformation.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/HaDeviceInformation.h) support the full [Home Assistant MQTT Discovery specification](https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery):

- **Child devices (`child_device` / `child_devices`)**: Create logical sub-units on a single physical device (e.g., individual outlets on a smart power strip or multi-channel relays) with `ha_bridge.childDevice(child_node_id, child_identifier, child_name)`. Entities bound to a child bridge scope their state/command topics under the child while linking to the parent device in Home Assistant.
- **Gateways & sub-devices (`via_device`)**: Model separate physical sub-devices (BLE/Zigbee/RF nodes) routed through a hub by setting `HaBridge::DeviceInfo::via_device` to the hub's identifier (plus `connections`, `suggested_area`, `configuration_url`, `serial_number`, etc.) via `HaBridge::populateDeviceJson(...)`.
- **Unified device-based discovery & migration**: Publish an entire device (including all components and `child_devices`) in a single MQTT discovery payload (`<prefix>/device/<node_id>/config`) with `ha_bridge.publishDeviceConfiguration({sensor_a, switch_b, ...})`. Existing deployments using per-entity discovery can migrate without losing entity IDs or history via `ha_bridge.migrateToDeviceConfiguration(...)`.
- **Automatic ESP hardware & firmware metadata**: Populate `sw_version`, `hw_version` (chip model & silicon revision), MAC `connections`, `configuration_url` (local IPv4), and `HaBridge::OriginInfo` automatically on ESP-IDF, Arduino ESP32, and ESP8266 using `HaDeviceInformation::populateDeviceJson(...)` and `HaDeviceInformation::detectOriginInfo(...)` from [HaDeviceInformation.h](https://github.com/Johboh/HomeAssistantEntities/blob/main/src/HaDeviceInformation.h).
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
You also need an MQTT library and a JSON library. See Dependencies section. Suggestion:
```
   johboh/MQTTRemote@^6.0.5
   bblanchon/ArduinoJson@^7.4.3
```

### Arduino IDE
Search for `HomeAssistantEntities` by johboh in the library manager. You also need an MQTT library and a JSON library. See Dependencies section. Suggestion: `MQTTRemote` by `johboh` and `ArduinoJson` by `bblanchon`.

#### Espressif IoT Development Framework:
In your existing `idf_component.yml` or in a new `idf_component.yml` next to your main component:
```
dependencies:
  johboh/homeassistantentities:
    version: ">=10.0.0"
```

You also need an MQTT library. See Dependencies section. Suggestion:
```
  johboh/mqttremote:
    version: ">=6.0.5"
```

### Examples
- [Arduino: Sensors](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/arduino/sensors/sensors.ino)
- [Arduino: Actuators](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/arduino/actuators/actuators.ino)
- [Arduino: Child Devices & Device Discovery](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/arduino/child_devices/child_devices.ino)
- [Arduino: Gateway (`via_device`) & Discovery Lifecycle](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/arduino/gateway_and_lifecycle/gateway_and_lifecycle.ino)
- [ESP-IDF: Sensors](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/espidf/sensors/main/main.cpp)
- [ESP-IDF: Actuators](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/espidf/actuators/main/main.cpp)
- [ESP-IDF: All Entities](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/espidf/all/main/main.cpp)
- [ESP-IDF: Child Devices & Device Discovery](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/espidf/child_devices/main/main.cpp)
- [ESP-IDF: Gateway (`via_device`) & Discovery Lifecycle](https://github.com/Johboh/HomeAssistantEntities/blob/main/examples/espidf/gateway_and_lifecycle/main/main.cpp)

### Functionality verified on the following platforms and frameworks
- ESP32 (tested with PlatformIO [espressif32@6.4.0](https://github.com/platformio/platform-espressif32) / [arduino-esp32@2.0.11](https://github.com/espressif/arduino-esp32) / [arduino-esp32@3.3.12](https://github.com/espressif/arduino-esp32) / [ESP-IDF@4.4.6](https://github.com/espressif/esp-idf) / [ESP-IDF@5.1.2](https://github.com/espressif/esp-idf) on ESP32-S2 and ESP32-C3, [ESP-IDF@5.3](https://github.com/espressif/esp-idf) / [ESP-IDF@5.4.1](https://github.com/espressif/esp-idf) / [ESP-IDF@6.1](https://github.com/espressif/esp-idf) on ESP32-S2 and ESP32-C6)
- ESP8266 (tested with PlatformIO [espressif8266@4.2.1](https://github.com/platformio/platform-espressif8266) / [arduino-core@3.2.0](https://github.com/esp8266/Arduino))

Newer versions most probably work too, but they have not been verified.

### Dependencies
- For ESP-IDF: [Johboh/nlohmann-json](https://github.com/Johboh/nlohmann-json) `@^3.12.0`, for Arduino: [Johboh/nlohmann-json](https://github.com/Johboh/nlohmann-json) `@^3.12.0` OR [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson) `@^7.4.3`
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
