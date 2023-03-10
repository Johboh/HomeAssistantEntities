# HomeAssistantEntities
Arduino library for providing sensors and actuators to Home Assistant using MQTT.

Home Assistant has a well defined way of defining sensors and actuators using MQTT. This library register a device with one or mulitple sensors and provide a way to publish new values for them, as well as listen for commands from Home Assistant.

### Currently supported sensors
- Atmospheric Preassure (hPa)
- Brightness (%)
- Door (open/closed)
- Humidity (%)
- Json (raw "json"-sensor)
- Lock (locked/unlocked)
- Motion (detected/not detected)
- Sound (detected/not detected)
- Temperature (°C/°F)
- Voltage (mV/V)
- Weight (g/kg)

### Currently supported actuators (and sensors)
- Curtain (open/opening, close/closing, position)
- Light (on/off & brightness)
- Switch (on/off)

### Examples
See Examples for [sensors](examples/Sensors/Sensors.ino) as well as [actuators](examples/Actuators/Actuators.ino).

### Supported platforms
- ESP32 (tested with platform I/O [espressif32@5.3.0](https://github.com/platformio/platform-espressif32) / [arduino-esp32@2.0.6](https://github.com/espressif/arduino-esp32) on ESP32-S2 and ESP32-C3)
- ESP8266 (tested with platform I/O [espressif8266@4.0.1](https://github.com/platformio/platform-espressif8266) / [ardunio-core@3.0.2](https://github.com/esp8266/Arduino))

### Dependencies
- https://github.com/Johboh/MQTTRemote @^1.0.3
  - _Note_: If you don't want to depend on [MQTTRemote](https://github.com/Johboh/MQTTRemote), you can copy the [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/src/IMQTTRemote.h) interface from the MQTTRemote repo and implement/adapt/forward to your own MQTT implementation. This library only depend on the [IMQTTRemote](https://github.com/Johboh/MQTTRemote/blob/main/src/IMQTTRemote.h) interface.
- https://github.com/bblanchon/ArduinoJson @^6.20.0
- Needs C++17 for `std::optional`.
  - For platform I/O in `platformio.ini`:
    ```C++
    build_unflags=-std=gnu++11 # "Disable" C++11
    build_flags=-std=gnu++17 # "Enable" C++17
    ```
