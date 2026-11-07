// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_PARTICULATE_MATTER_H__
#define __HA_ENTITY_PARTICULATE_MATTER_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents a Particulate Matter (PM1 / PM2.5 / PM4 / PM10) mass concentration MQTT Sensor (`sensor.mqtt`, unit
 * `"µg/m³"`).
 *
 * Suitable for air-quality sensors such as PMS5003, SDS011, or SEN55.
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityParticulateMatter pm25(ha_bridge, "PM2.5", std::nullopt, {
 *     .size = HaEntityParticulateMatter::Size::pm25,
 *     .suggested_display_precision = 1,
 * });
 *
 * // Once MQTT is connected:
 * pm25.publishConfiguration();
 * pm25.update(8.4); // 8.4 µg/m³
 * @endcode
 */
class HaEntityParticulateMatter : public HaEntity {
public:
  enum class Size {
    pm1,  // Concentration of particulate matter less than 1 micrometer in µg/m³
    pm25, // Concentration of particulate matter less than 2.5 micrometers in µg/m³
    pm10, // Concentration of particulate matter less than 10 micrometers in µg/m³
    pm4,  // Concentration of particulate matter less than 4 micrometers in µg/m³
  };

  struct Configuration {
    /**
     * @brief Particulate matter size threshold (`pm1`, `pm25`, `pm4`, or `pm10`; default `Size::pm10`).
     */
    Size size = Size::pm10;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for graphing
     * repeated identical measurements or triggering automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:air-filter"`).
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
     * @brief Number of decimals to use in the Home Assistant frontend when displaying the value.
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.size = Size::pm10,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityParticulateMatter object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Air Monitor"` + name `"PM2.5"`
   * -> `"Air Monitor PM2.5"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * particulate matter sensors of the same `Size` on the same node ID. Note that each `Size` already uses a distinct
   * discovery object ID (`pm1`, `pm25`, `pm4`, `pm10`). Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this sensor.
   */
  HaEntityParticulateMatter(HaBridge &ha_bridge, std::string name,
                            std::optional<std::string> child_object_id = std::nullopt,
                            Configuration configuration = _default)
      : _ha_entity_sensor(makeSensor(ha_bridge, name, child_object_id, configuration)) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the particulate matter concentration regardless of whether the value has changed. Also see
   * `update()`.
   *
   * @param concentration Particulate matter concentration in `µg/m³`.
   */
  void publish(double concentration) { _ha_entity_sensor.publish(concentration); }

  /**
   * @brief Publish the particulate matter concentration only if the value has changed. Also see `publish()`.
   *
   * @param concentration Particulate matter concentration in `µg/m³`.
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

    switch (configuration.size) {
    case Size::pm1:
      return HaEntitySensor(
          ha_bridge, name, child_object_id,
          makeConfig(homeassistantentities::Sensor::Pm1{}, homeassistantentities::Sensor::Pm1::Unit::ug_m3));
    case Size::pm25:
      return HaEntitySensor(
          ha_bridge, name, child_object_id,
          makeConfig(homeassistantentities::Sensor::Pm25{}, homeassistantentities::Sensor::Pm25::Unit::ug_m3));
    case Size::pm4:
      return HaEntitySensor(
          ha_bridge, name, child_object_id,
          makeConfig(homeassistantentities::Sensor::Pm4{}, homeassistantentities::Sensor::Pm4::Unit::ug_m3));
    case Size::pm10:
      break;
    }
    return HaEntitySensor(
        ha_bridge, name, child_object_id,
        makeConfig(homeassistantentities::Sensor::Pm10{}, homeassistantentities::Sensor::Pm10::Unit::ug_m3));
  }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_PARTICULATE_MATTER_H__
