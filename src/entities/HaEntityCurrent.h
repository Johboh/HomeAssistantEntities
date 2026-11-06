// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_CURRENT_H__
#define __HA_ENTITY_CURRENT_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Current sensor (see homeassistantentities::Sensor::Current::Unit in HaDeviceClasses.h).
 */
class HaEntityCurrent : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::Current::Unit;

  struct Configuration {
    /**
     * @brief the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
     */
    Unit unit = Unit::A;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:current-ac").
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
     * @brief Number of decimals which should be used in the Home Assistant frontend when displaying the value.
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.unit = Unit::A,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Current object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "current", friendly name will be "Bathroom current". If no
   * device, friendly name will be just "current". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityCurrent(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                  Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Current{},
                              .unit_of_measurement = configuration.unit,
                              .icon = configuration.icon,
                              .force_update = configuration.force_update,
                              .entity_category = configuration.entity_category,
                              .expire_after = configuration.expire_after,
                              .suggested_display_precision = configuration.suggested_display_precision,
                              .enabled_by_default = configuration.enabled_by_default,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the current. This will publish to MQTT regardless if the value has changed. Also see
   * update().
   *
   * @param current current in the unit specified in the configuration.
   */
  void publish(double current) { _ha_entity_sensor.publish(current); }

  /**
   * @brief Publish the current, but only if the value has changed. Also see publish().
   *
   * @param current current in the unit specified in the configuration.
   */
  void update(double current) { _ha_entity_sensor.update(current); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_CURRENT_H__
