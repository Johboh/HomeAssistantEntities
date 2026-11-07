// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_TIMESTAMP_H__
#define __HA_ENTITY_TIMESTAMP_H__

#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <ctime>
#include <optional>
#include <string>

/**
 * @brief Represents a Timestamp MQTT Sensor (`sensor.mqtt` with `device_class: "timestamp"`).
 *
 * Accepts either an ISO 8601 timestamp string (with timezone offset or `'Z'`) or a `const struct tm *`.
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityTimestamp last_boot(ha_bridge, "Last Boot", "boot", {
 *     .entity_category = "diagnostic",
 * });
 *
 * // Once MQTT is connected:
 * last_boot.publishConfiguration();
 * last_boot.publish("2026-05-06T15:43:00Z");
 * @endcode
 */
class HaEntityTimestamp : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true`, set up a JSON attributes topic (`json_attributes_topic`) so attributes can be published along
     * with the timestamp or separately.
     */
    bool with_attributes = false;

    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for triggering
     * automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:clock"`).
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
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_attributes = false,
                                          .force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityTimestamp object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Gateway"` + name `"Last Boot"`
   * -> `"Gateway Last Boot"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * timestamp sensors on the same node ID. For example, for node `"gateway"`, the default discovery topic is
   * `"homeassistant/sensor/gateway/timestamp/config"`; setting `child_object_id` to `"boot"` yields
   * `"homeassistant/sensor/gateway/timestamp_boot/config"`. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this timestamp sensor.
   */
  HaEntityTimestamp(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                    Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Timestamp{},
                              .state_class = std::nullopt,
                              .with_attributes = configuration.with_attributes,
                              .icon = configuration.icon,
                              .force_update = configuration.force_update,
                              .entity_category = configuration.entity_category,
                              .expire_after = configuration.expire_after,
                              .enabled_by_default = configuration.enabled_by_default,
                          }) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void unpublishConfiguration() override { _ha_entity_sensor.unpublishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Format and publish a `struct tm` timestamp regardless of whether the value has changed. Also see `update()`.
   *
   * @param time Pointer to the `struct tm` timestamp to format as ISO 8601 (`nullptr` is ignored).
   * @param attributes Optional JSON attributes to send with the timestamp (`Configuration::with_attributes` must be
   * `true`).
   */
  void publish(const struct tm *time, const Attributes::Map &attributes = {}) {
    auto formatted = formatTimestamp(time);
    if (formatted) {
      publish(*formatted, attributes);
    }
  }

  /**
   * @brief Publish an ISO 8601 timestamp string regardless of whether the value has changed. Also see `update()`.
   *
   * @param time ISO 8601 timestamp string with timezone (e.g. `"2026-05-06T15:43:00Z"` or
   * `"2026-05-06T17:43:00+0200"`).
   * @param attributes Optional JSON attributes to send with the timestamp (`Configuration::with_attributes` must be
   * `true`).
   */
  void publish(const std::string &time, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.publish(time, attributes);
  }

  /**
   * @brief Format and publish a `struct tm` timestamp only if the formatted timestamp (or `attributes`) has changed.
   * Also see `publish()`.
   *
   * @param time Pointer to the `struct tm` timestamp to format as ISO 8601 (`nullptr` is ignored).
   * @param attributes Optional JSON attributes to send with the timestamp (`Configuration::with_attributes` must be
   * `true`).
   */
  void update(const struct tm *time, const Attributes::Map &attributes = {}) {
    auto formatted = formatTimestamp(time);
    if (formatted) {
      _ha_entity_sensor.update(*formatted, attributes);
    }
  }

  /**
   * @brief Publish an ISO 8601 timestamp string only if the timestamp (or `attributes`) has changed. Also see
   * `publish()`.
   *
   * @param time ISO 8601 timestamp string with timezone (e.g. `"2026-05-06T15:43:00Z"` or
   * `"2026-05-06T17:43:00+0200"`).
   * @param attributes Optional JSON attributes to send with the timestamp (`Configuration::with_attributes` must be
   * `true`).
   */
  void update(const std::string &time, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.update(time, attributes);
  }

  /**
   * @brief Publish JSON attributes only (`Configuration::with_attributes` must be `true`), regardless of whether they
   * have changed. Also see `updateAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void publishAttributes(const Attributes::Map &attributes) { _ha_entity_sensor.publishAttributes(attributes); }

  /**
   * @brief Publish JSON attributes only if they have changed (`Configuration::with_attributes` must be `true`). Also
   * see `publishAttributes()`.
   *
   * @param attributes Attributes map to publish.
   */
  void updateAttributes(const Attributes::Map &attributes) { _ha_entity_sensor.updateAttributes(attributes); }

private:
  static std::optional<std::string> formatTimestamp(const struct tm *time) {
    if (!time) {
      return std::nullopt;
    }
    char buf[32];
    std::size_t len = std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S%z", time);
    if (len == 0) {
      return std::nullopt;
    }
    std::string formatted(buf, len);
    // On newlib (ESP-IDF/ESP8266), %z outputs an empty string if TZ is unset; append 'Z' for valid ISO 8601 UTC.
    if (len == 19) {
      formatted += 'Z';
    }
    return formatted;
  }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_TIMESTAMP_H__
