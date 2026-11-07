// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_JSON_H__
#define __HA_ENTITY_JSON_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <HaEntitySensor.h>
#include <IJson.h>
#include <cstdint>
#include <optional>
#include <string>

// TODO(johboh): merge with HaEntityString?
/**
 * @brief Represents a raw JSON MQTT Sensor (`sensor.mqtt`) that serializes and publishes an `IJsonDocument` to its
 * state topic.
 *
 * Note: Home Assistant limits sensor `state` strings to 255 characters; for large structured payloads, consider
 * `HaEntityString` / `HaEntitySensor` with `with_attributes = true`.
 * See https://www.home-assistant.io/integrations/sensor.mqtt/
 *
 * @par Example
 * @code
 * HaEntityJson status_json(ha_bridge, "Diagnostics", "diag", {
 *     .entity_category = "diagnostic",
 * });
 *
 * // Once MQTT is connected:
 * status_json.publishConfiguration();
 * IJsonDocument doc;
 * doc["uptime"] = 120;
 * status_json.update(doc);
 * @endcode
 */
class HaEntityJson : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief In Home Assistant, trigger state events even if the sensor's state has not changed. Useful for triggering
     * automations on every incoming state message.
     */
    bool force_update = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:code-json"`).
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

  inline static Configuration _default = {.force_update = false,
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .expire_after = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityJson object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Gateway"` + name `"Diagnostics"`
   * -> `"Gateway Diagnostics"`). Can be empty if a device is set and this is the main (or only) entity of the device.
   * See https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several JSON
   * sensors on the same node ID. For example, for node `"gateway"`, the default discovery topic is
   * `"homeassistant/sensor/gateway/json/config"`; setting `child_object_id` to `"net"` yields
   * `"homeassistant/sensor/gateway/json_net/config"`. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this JSON sensor.
   */
  HaEntityJson(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
               Configuration configuration = _default)
      : _ha_entity_sensor(ha_bridge, name, child_object_id,
                          HaEntitySensor::Configuration{
                              .device_class = homeassistantentities::Sensor::Undefined::Json{},
                              .state_class = std::nullopt,
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
   * @brief Serialize and publish the JSON document regardless of whether the value has changed. Also see `update()`.
   *
   * @param json_doc JSON document to serialize and publish.
   */
  void publish(const IJsonDocument &json_doc) {
    auto message = toJsonString(json_doc);
    _ha_entity_sensor.publish(message);
  }

  /**
   * @brief Serialize and publish the JSON document only if the serialized payload has changed. Also see `publish()`.
   *
   * @param json_doc JSON document to serialize and publish.
   */
  void update(const IJsonDocument &json_doc) {
    auto message = toJsonString(json_doc);
    _ha_entity_sensor.update(message);
  }

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    return buildChildComponentConfiguration(_ha_entity_sensor, out);
  }

private:
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_JSON_H__
