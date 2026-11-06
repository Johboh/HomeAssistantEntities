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
 * @brief Represent a timestamp
 */
class HaEntityTimestamp : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief if true, setup an attribute topic attributes will be published to. With this set, attributes
     * can be published when the message is published, or using a separate call.
     */
    bool with_attributes = false;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:clock").
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
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
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
   * @brief Construct a new Ha Entity Timestamp object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "timestamp", friendly name will be "Bathroom timestamp". If
   * no device, friendly name will be just "timestamp". If a device is set, this name can be left empty if this entity
   * is the one main entity (or only) entity of this device. See
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
   * @brief Publish the timestamp. This will publish to MQTT regardless if the string has changed. Also see
   * update().
   *
   * @param time the timestamp to publish.
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void publish(const struct tm *time, const Attributes::Map &attributes = {}) {
    auto formatted = formatTimestamp(time);
    if (formatted) {
      publish(*formatted, attributes);
    }
  }

  /**
   * @brief Publish the timestamp. This will publish to MQTT regardless if the string has changed. Also see
   * update().
   *
   * @param time the timestamp to publish, ISO 8601 UTC
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void publish(const std::string &time, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.publish(time, attributes);
  }

  /**
   * @brief Publish the timestamp, but only if the timestamp has changed. Also see publish().
   *
   * @param time the timestamp to publish.
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void update(const struct tm *time, const Attributes::Map &attributes = {}) {
    auto formatted = formatTimestamp(time);
    if (formatted) {
      _ha_entity_sensor.update(*formatted, attributes);
    }
  }

  /**
   * @brief Publish the timestamp, but only if the timestamp has changed. Also see publish().
   *
   * @param time the timestamp to publish, ISO 8601 UTC
   * @param attributes optional attributes to send with the string. with_attributes in constructor must be set.
   */
  void update(const std::string &time, const Attributes::Map &attributes = {}) {
    _ha_entity_sensor.update(time, attributes);
  }

  /**
   * @brief Publish attributes only. with_attributes in constructor must be set.
   *
   * @param attributes attributes to publish.
   */
  void publishAttributes(const Attributes::Map &attributes) { _ha_entity_sensor.publishAttributes(attributes); }

  /**
   * @brief Publish attributes only, but only if the value has changed. Also see
   * publishAttributes(). with_attributes in constructor must be set.
   *
   * @param attributes attributes to publish.
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
