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
 * @brief Represents a Sound detection MQTT Binary Sensor (`binary_sensor.mqtt` with `device_class: "sound"`).
 *
 * Reports whether sound is detected (`true` -> `"ON"`) or clear (`false` -> `"OFF"`).
 * See https://www.home-assistant.io/integrations/binary_sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntitySound noise_sensor(ha_bridge, "Sound", std::nullopt, {
 *     .off_delay = 5,
 * });
 *
 * // Once MQTT is connected:
 * noise_sensor.publishConfiguration();
 * noise_sensor.update(false);
 * @endcode
 */
class HaEntitySound : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:ear-hearing"`).
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Optional number of seconds after which the sensor's state expires if not updated.
     */
    std::optional<uint32_t> expire_after = std::nullopt;

    /**
     * @brief Number of seconds after turning on before Home Assistant automatically resets the binary sensor back to
     * off (useful for sound detectors that only emit `"ON"` events).
     */
    std::optional<uint32_t> off_delay = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
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
   * @brief Construct a new HaEntitySound object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Nursery"` + name `"Sound"` ->
   * `"Nursery Sound"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several sound
   * sensors on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sound sensor.
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
   * @brief Publish the sound detection state regardless of whether the value has changed. Also see `update()`.
   *
   * @param detected `true` if sound was detected (`"ON"`), `false` otherwise (`"OFF"`).
   */
  void publish(bool detected) { _ha_entity_sensor.publish(detected ? "ON" : "OFF"); }

  /**
   * @brief Publish the sound detection state only if the value has changed. Also see `publish()`.
   *
   * @param detected `true` if sound was detected (`"ON"`), `false` otherwise (`"OFF"`).
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
