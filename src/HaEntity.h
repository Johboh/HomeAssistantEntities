// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_H__
#define __HA_ENTITY_H__

#include <IJson.h>
#include <string>

class HaBridge;

/**
 * @brief Abstract base class for all Home Assistant MQTT entities.
 *
 * An entity represents a sensor, binary sensor, actuator, or event source exposed to Home Assistant over MQTT
 * discovery (https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery).
 * - For **sensors**, Home Assistant subscribes to the entity's state topic to display and record telemetry.
 * - For **actuators**, Home Assistant publishes commands to command topics and optionally tracks the entity's state.
 *
 * Each `HaEntity` publishes its discovery configuration under the Home Assistant discovery prefix (default
 * `homeassistant/`) and provides typed methods to publish/update state values or register callbacks for incoming
 * commands. `HaEntity` is purely an MQTT/Home Assistant protocol abstraction and does not interact directly with
 * hardware.
 *
 * @par Example
 * Concrete entities (such as `HaEntityTemperature` or `HaEntitySwitch`) inherit from `HaEntity` and can be managed
 * individually or grouped in a device discovery bundle via `HaBridge::publishDeviceConfiguration()`:
 * @code
 * HaBridge ha_bridge(mqtt_client, "living_room", device_doc);
 * HaEntityTemperature temperature(ha_bridge, "Temperature");
 * HaEntitySwitch relay(ha_bridge, "Relay", "main");
 *
 * relay.onCommand([&](bool on) {
 *   setRelayHardware(on);
 *   relay.publish(on);
 * });
 *
 * // Once MQTT is connected:
 * temperature.publishConfiguration();
 * relay.publishConfiguration();
 * temperature.publish(21.5);
 * relay.publish(false);
 * @endcode
 */
class HaEntity {
public:
  virtual ~HaEntity() = default;

  /**
   * @brief Publish the Home Assistant MQTT discovery configuration for this entity.
   */
  virtual void publishConfiguration() = 0;

  /**
   * @brief Remove the Home Assistant discovery configuration by publishing an empty retained payload to the discovery
   * topic.
   */
  virtual void unpublishConfiguration() {}

  /**
   * @brief Republish the most recently published state (and attributes, if any) to MQTT.
   */
  virtual void republishState() = 0;

protected:
  friend class HaBridge;

  /**
   * @brief Internal descriptor for an entity's MQTT discovery component payload (used for device-based discovery and
   * migration).
   */
  struct ComponentConfig {
    std::string component;
    std::string object_id;
    std::string child_object_id;
    IJsonDocument doc;
    HaBridge *bridge = nullptr;
  };

  /**
   * @brief Populate this entity's component discovery descriptor.
   * @return true if a valid component configuration was written to @p out.
   */
  virtual bool buildComponentConfiguration(ComponentConfig &out) {
    (void)out;
    return false;
  }

  /**
   * @brief Helper allowing wrapper entities to delegate `buildComponentConfiguration` to an inner `HaEntity` member.
   */
  static bool buildChildComponentConfiguration(HaEntity &child, ComponentConfig &out) {
    return child.buildComponentConfiguration(out);
  }
};

#endif // __HA_ENTITY_H__