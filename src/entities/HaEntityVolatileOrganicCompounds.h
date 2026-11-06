// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__
#define __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a volatile organic compounds sensor, concentration in μg/m³ or parts in ppb/parts per billion.
 */
class HaEntityVolatileOrganicCompounds : public HaEntity {
public:
  enum class Unit {
    Concentration, // Concentration of volatile organic compounds in µg/m³
    Parts,         // Ratio of volatile organic compounds in ppb/parts per billion
  };

  struct Configuration {
    /**
     * @brief the unit of this volatile organic compound sensor. Either a concentration (μg/m³) or a ratio (ppb/parts
     * per billion).
     */
    Unit unit = Unit::Parts;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:molecule").
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

  inline static Configuration _default = {.unit = Unit::Parts,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity volatile organic compounds object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "volatile organic compounds", friendly name will be "Bathroom
   * volatile organic compounds". If no device, friendly name will be just "volatile organic compounds". If a device is
   * set, this name can be left empty if this entity is the one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home assistant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityVolatileOrganicCompounds(HaBridge &ha_bridge, std::string name,
                                   std::optional<std::string> child_object_id = std::nullopt,
                                   Configuration configuration = _default)
      : _ha_entity_sensor(makeSensor(ha_bridge, name, child_object_id, configuration)) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the volatile organic compounds concentration or parts, depending on Unit selected in configuration.
   * This will publish to MQTT regardless if the value has changed. Also see update().
   *
   * @param concentration concentration/parts in µg/m³ or ppb, depending on what was selected in the configuration.
   */
  void publish(double concentration) { _ha_entity_sensor.publish(concentration); }

  /**
   * @brief Publish the volatile organic compounds concentration or parts, depending on Unit selected in configuration,
   * but only if the value has changed. Also see publish().
   *
   * @param concentration concentration/parts in µg/m³ or ppb, depending on what was selected in the configuration.
   */
  void update(double concentration) { _ha_entity_sensor.update(concentration); }

private:
  static HaEntitySensor makeSensor(HaBridge &ha_bridge, const std::string &name,
                                   const std::optional<std::string> &child_object_id,
                                   const Configuration &configuration) {
    auto makeConfig = [&](const homeassistantentities::DeviceClass &dc, homeassistantentities::UnitType unit) {
      return HaEntitySensor::Configuration{
          .device_class = dc,
          .unit_of_measurement = unit,
          .icon = configuration.icon,
          .force_update = configuration.force_update,
          .entity_category = configuration.entity_category,
          .expire_after = configuration.expire_after,
          .suggested_display_precision = configuration.suggested_display_precision,
          .enabled_by_default = configuration.enabled_by_default,
      };
    };

    if (configuration.unit == Unit::Concentration) {
      return HaEntitySensor(ha_bridge, name, child_object_id,
                            makeConfig(homeassistantentities::Sensor::VolatileOrganicCompounds{},
                                       homeassistantentities::Sensor::VolatileOrganicCompounds::Unit::ug_m3));
    }
    return HaEntitySensor(ha_bridge, name, child_object_id,
                          makeConfig(homeassistantentities::Sensor::VolatileOrganicCompoundsParts{},
                                     homeassistantentities::Sensor::VolatileOrganicCompoundsParts::Unit::ppb));
  }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__
