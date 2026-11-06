// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SOUND_H__
#define __HA_ENTITY_SOUND_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Sound binary sensor (if a sound is detected or not).
 */
class HaEntitySound : public HaEntity {
public:
  struct Configuration {
    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:ear-hearing").
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Optional number of seconds after which the sensor's state expires if not updated.
     */
    std::optional<uint32_t> expire_after = std::nullopt;

    /**
     * @brief Number of seconds after turning on before Home Assistant automatically resets back to off (useful for
     * sound detectors that only send ON events).
     */
    std::optional<uint32_t> off_delay = std::nullopt;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .off_delay = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Sound object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "sound", friendly name will be "Bathroom sound". If no
   * device, friendly name will be just "sound". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntitySound(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::BinarySensor::Sound{},
                              .state_class = std::nullopt,
                              .icon = configuration.icon,
                              .force_update = configuration.force_update,
                              .entity_category = configuration.entity_category,
                              .expire_after = configuration.expire_after,
                              .off_delay = configuration.off_delay,
                              .enabled_by_default = configuration.enabled_by_default,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the sound. This will publish to MQTT regardless if the value has changed. Also see
   * update().
   *
   * @param detected true if sound was detected, false if not.
   */
  void publish(bool detected) { _ha_entity_sensor.publish(detected ? "ON" : "OFF"); }

  /**
   * @brief Publish the sound, but only if the value has changed. Also see publish().
   *
   * @param detected true if sound was detected, false if not.
   */
  void update(bool detected) { _ha_entity_sensor.update(detected ? "ON" : "OFF"); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_SOUND_H__
