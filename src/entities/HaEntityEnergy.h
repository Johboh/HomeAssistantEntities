// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_ENERGY_H__
#define __HA_ENTITY_ENERGY_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent an Energy consumption/production sensor (default kWh, `total_increasing`) compatible with the Home
 * Assistant Energy Dashboard (see homeassistantentities::Sensor::Energy::Unit in HaDeviceClasses.h).
 */
class HaEntityEnergy : public HaEntity {
public:
  using Unit = homeassistantentities::Sensor::Energy::Unit;

  struct Configuration {
    /**
     * @brief Unit of measurement reported for this energy sensor (default `Unit::kWh`).
     */
    Unit unit = Unit::kWh;

    /**
     * @brief State class for this energy sensor (`"total_increasing"` by default, or `"total"`).
     */
    std::optional<std::string> state_class = "total_increasing";

    /**
     * @brief In Home Assistant, trigger events even if the sensor's state hasn't changed.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:lightning-bolt").
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

  inline static Configuration _default = {.unit = Unit::kWh,
                                          .state_class = "total_increasing",
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Energy object.
   */
  HaEntityEnergy(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Energy{},
                              .unit_of_measurement = configuration.unit,
                              .state_class = configuration.state_class,
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
   * @brief Publish energy in the configured unit regardless of whether it changed.
   */
  void publish(double energy) { _ha_entity_sensor.publish(energy); }

  /**
   * @brief Publish energy in the configured unit only if changed.
   */
  void update(double energy) { _ha_entity_sensor.update(energy); }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_ENERGY_H__
