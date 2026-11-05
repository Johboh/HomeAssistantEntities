// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_H__
#define __HA_ENTITY_H__

#include <IJson.h>
#include <string>

class HaBridge;

/**
 * @brief Abstract for a HaEntity.
 *
 * An entity is the sensor, binary sensor, actuator etc that will show up in Home Assistant. If a sensor, Home Assistant
 * can show/use the sensor value. If an actuator, Home Assistant can send states to the entity.
 * An entity has a configuration published to the Home Assistant MQTT topic (directly under MQTT "homeassistant/") which
 * Home Assistant is subscribed to, and where it collect all its MQTT Sensors, Binary Sensors, Actuators and so on. A
 * configuration always have a setup phase to publish to the above mentioned topic, but also a specific method for
 * publishing value to the state topic, if any. Or to read from a state topic.
 *
 * A HaEntity is only an Home Assistant abstraction, and does not interact with any hardware layer. an HaEntity should
 * only provide a method to setup and publish any values, or callbacks one can subscribe to to receive values.
 */
class HaEntity {
public:
  virtual ~HaEntity() = default;

  /**
   * @brief Publish the Home Assistant configuration (single-component discovery).
   */
  virtual void publishConfiguration() = 0;

  /**
   * @brief Remove the Home Assistant configuration by publishing an empty retained payload to the discovery topic.
   */
  virtual void unpublishConfiguration() {}

  /**
   * @brief Republish any previous published state on any of the available state topics.
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