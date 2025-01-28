# HomeAssistantEntities
[![PlatformIO CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/platformio.yaml/badge.svg)](https://registry.platformio.org/libraries/johboh/HomeAssistantEntities)
[![ESP-IDF CI](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/espidf.yaml/badge.svg)](https://components.espressif.com/components/johboh/homeassistantentities)
[![GitHub release](https://img.shields.io/github/release/Johboh/HomeAssistantEntities.svg)](https://github.com/Johboh/HomeAssistantEntities/releases)
[![Clang-format](https://github.com/Johboh/HomeAssistantEntities/actions/workflows/clang-format.yaml/badge.svg)](https://github.com/Johboh/HomeAssistantEntities)

Arduino (using Arduino IDE or PlatformIO) and ESP-IDF (using Espressif IoT Development Framework or PlatformIO) compatible library for providing sensors and actuators to Home Assistant using MQTT.

Home Assistant has a well defined way of defining sensors and actuators using MQTT. This library register a device with one or mulitple sensors and provide a way to publish new values for them, as well as listen for commands from Home Assistant.

### Currently supported sensors*
- Atmospheric Preassure (cbar, bar, hPa, mmHg, inHg, kPa, mbar, Pa, psi)
- Boolean
- Binary sensor (Generic sensor, with any supported [device class](https://www.home-assistant.io/integrations/binary_sensor/#device-class) using a device class from [HaDeviceClasses.h](./src/entities/HaDeviceClasses.h))
- Brightness (%)
- Carbon Dioxide (ppm)
- Door (open/closed)
- Humidity (%)
- Json (raw "json"-sensor)
- Lock (locked/unlocked)
- Motion (detected/not detected)
- Number
- Particulate matter (μg/m³)
- Sound (detected/not detected)
- String (raw "String"-sensor)
- Temperature (°C, °F, K)
- Text
- Unit Concentration (parts per volume)
- Volatile organic compounds (µg/m³, ppb)
- Voltage (V = 1, mV, µV)
- Weight (kg, g, mg, ug, oz, lb, st)
- Sensor (Generic sensor, with any supported [device class](https://www.home-assistant.io/integrations/sensor/#device-class) and unit of measurement using a device class from [HaDeviceClasses.h](./src/entities/HaDeviceClasses.h))

### Currently supported actuators (and sensors)*
- Curtain (open/opening, close/closing, position)
- Button
- Event
- Light (brightness, rgb, effect)
- Select
- Switch (on/off)

*_If you can't find a sensor/actuator you need, either use the genric sensor, create a pull request to add one or raise an issue on GitHub and we'll add it to this library._

### Installation
#### PlatformIO (Arduino or ESP-IDF):
Add the following to `libs_deps`:
```
   Johboh/HomeAssistantEntities@^8.0.2
```
You also need a MQTT library and a JSON library. See Dependencies section. Suggestion:
```
   johboh/MQTTRemote@^4.0.6
   bblanchon/ArduinoJson@^7.1.0
```

### Arduino IDE
Search for `HomeAssistantEntities` by johboh in the library manager. You also need a MQTT library and a JSON library. See Dependencies section. Suggestion: `MQTTRemote` by `johboh` and `ArduinoJson` by `bblanchon`.

#### Espressif IoT Development Framework:
In your existing `idf_component.yml` or in a new `idf_component.yml` next to your main component:
```
dependencies:
  johboh/homeassistantentities:
    version: ">=8.0.2"
```

You also need a MQTT library. See Dependencies section. Suggestion:
```
  johboh/mqttrmote:
    version: ">=4.0.5"
```

### Examples
- [Arduino: Sensors](examples/arduino/sensors/Sensors.ino)
- [Arduino: Actuators](examples/arduino/actuators/Actuators.ino)
- [ESP-IDF: Sensors](examples/espidf/sensors/main/main.cpp)
- [ESP-IDF: Actuators](examples/espidf/actuators/main/main.cpp)

### Functionallity verified on the following platforms and frameworks
- ESP32 (tested with PlatformIO [espressif32@6.4.0](https://github.com/platformio/platform-espressif32) / [arduino-esp32@2.0.11](https://github.com/espressif/arduino-esp32) / [ESP-IDF@4.4.6](https://github.com/espressif/esp-idf) / [ESP-IDF@5.1.2](https://github.com/espressif/esp-idf) on ESP32-S2 and ESP32-C3)
- ESP8266 (tested with PlatformIO [espressif8266@4.2.1](https://github.com/platformio/platform-espressif8266) / [ardunio-core@3.1.2](https://github.com/esp8266/Arduino))

Newer version most probably work too, but they have not been verified.

### Dependencies
- For ESP-IDF: https://github.com/Johboh/nlohmann-json @^3.11.3, for Arduino: https://github.com/Johboh/nlohmann-json @^3.11.3 OR https://github.com/bblanchon/ArduinoJson@^7.1.0
- *An MQTT implementation is required.* There is a copy of [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/includes/IMQTTRemote.h) in this library from [Johboh/MQTTRemote](https://github.com/Johboh/MQTTRemote). You can either add a dependency on [MQTTRemote](https://github.com/Johboh/MQTTRemote) to get a fully working MQTT client (the examples are using this dependency), or you can implement/adapt/forward to your own MQTT implementation. This library only depend on the [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/includes/IMQTTRemote.h) interface.
- Needs C++17 for `std::optional`.
  - For PlatformIO in `platformio.ini`:
    ```C++
    build_unflags=-std=gnu++11 # "Disable" C++11
    build_flags=-std=gnu++17 # "Enable" C++17
    ```
