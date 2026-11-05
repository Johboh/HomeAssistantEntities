// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_LOCK_H__
#define __HA_ENTITY_LOCK_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Lock binary sensor (if a lock is locked or unlocked).
 */
class HaEntityLock : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Lock object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "lock", friendly name will be "Bathroom lock". If no
   * device, friendly name will be just "lock". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   */
  HaEntityLock(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::BinarySensor::Lock{},
                              .state_class = std::nullopt,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the lock. This will publish to MQTT regardless if the value has changed. Also see
   * updateLock().
   *
   * @param locked true if lock is locked, or false if unlocked.
   */
  void publishLock(bool locked) { _ha_entity_sensor.publishValue(locked ? "OFF" : "ON"); } // locked == OFF

  /**
   * @brief Publish the lock, but only if the value has changed. Also see publishLock().
   *
   * @param locked true if lock is locked, or false if unlocked.
   */
  void updateLock(bool locked) { _ha_entity_sensor.updateValue(locked ? "OFF" : "ON"); } // locked == OFF

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_LOCK_H__
