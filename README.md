# HomeAssistantEntities
Library for providing sensors and actuators to Home Assistant using MQTT.

Home Assistant has a well defined way of defining sensors and actuators using MQTT. This library register a device with one or mulitple sensors and provide a way to publish new values for them, as well as listen for commands from Home Assistant.

See Examples for [sensors](examples/Sensors/Sensors.ino) as well as [actuators](examples/Actuators/Actuators.ino).

### Dependencies
- https://github.com/Johboh/MQTTRemote @^1.0.3
- https://github.com/bblanchon/ArduinoJson @^6.20.0