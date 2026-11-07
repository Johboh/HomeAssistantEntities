// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_ACTUATOR_H__
#define __HA_ENTITY_ACTUATOR_H__

#include "AttributeVariants.h"
#include "HaDeviceClasses.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief A generic composable actuator/control entity (the actuator counterpart to HaEntitySensor).
 *
 * Handles discovery configuration, state publishing/caching/republishing, JSON attributes, and command topic
 * subscriptions across Home Assistant MQTT actuator platforms (such as switch, button, lock, valve, number, select,
 * text, update, etc.). Specific actuator classes can compose HaEntityActuator to avoid boilerplate, or users can
 * instantiate HaEntityActuator directly with a DeviceClass from HaDeviceClasses.h.
 */
class HaEntityActuator : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The Device class to use (determines default component, object_id, and device_class).
     * See namespaces Button, Switch, Cover, Valve, Lock, Update, Number, Select, Text, Fan, Light in HaDeviceClasses.h.
     */
    const homeassistantentities::DeviceClass &device_class;

    /**
     * @brief Optional unit of measurement from the DeviceClass (e.g. when reusing a Sensor::* class for a "number"
     * actuator).
     */
    std::optional<homeassistantentities::UnitType> unit_of_measurement = std::nullopt;

    /**
     * @brief Optional custom unit of measurement string (used when unit_of_measurement is not set).
     */
    std::optional<std::string> custom_unit_of_measurement = std::nullopt;

    /**
     * @brief Optional override for the Home Assistant component platform (e.g. "number" when reusing a Sensor::*
     * DeviceClass). Defaults to device_class.component().
     */
    std::optional<std::string> component_override = std::nullopt;

    /**
     * @brief Optional override for the discovery object_id segment. Defaults to device_class.objectId().
     */
    std::optional<std::string> object_id_override = std::nullopt;

    /**
     * @brief Optional override for the device_class string in discovery JSON (set to "" to omit device_class, or a
     * non-empty string to override device_class.deviceClass()).
     */
    std::optional<std::string> device_class_override = std::nullopt;

    /**
     * @brief Optional sub-topic suffix placed between child_object_id and "/state" or "/command" (e.g. "onoff",
     * "text", or "" for `<node_id>/<component>/[<child>/]state`). When std::nullopt, topics use
     * `<node_id>/<component>/<object_id>/[<child>/]state` matching HaEntitySensor.
     */
    std::optional<std::string> sub_topic = std::nullopt;

    /**
     * @brief If true, configure and publish to `state_topic` (set to false for command-only actuators like buttons or
     * write-only text).
     */
    bool with_state_topic = true;

    /**
     * @brief If true, configure and subscribe to `command_topic` (set to false for read-only entities).
     */
    bool with_command_topic = true;

    /**
     * @brief If true, configure an attributes topic (`json_attributes_topic`) so attributes can be published with
     * state or separately.
     */
    bool with_attributes = false;

    /**
     * @brief If true, tells Home Assistant to publish command messages with MQTT retain set to true.
     */
    bool retain = false;

    /**
     * @brief If true, emit `"retain": false` when `retain` is false; if false, only emit `"retain": true` when `retain`
     * is true.
     */
    bool emit_retain_when_false = true;

    /**
     * @brief Flag that defines if the actuator works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:toggle-switch").
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Number of decimals which should be used in the Home Assistant frontend when displaying numeric values.
     */
    std::optional<uint8_t> suggested_display_precision = std::nullopt;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief Optional list of valid options (e.g. for `select` actuators).
     */
    std::vector<std::string> options = {};

    /**
     * @brief Optional hook to populate platform-specific fields in the component discovery JSON document.
     */
    std::function<void(IJsonDocument &)> extra_configuration = {};
  };

  /**
   * @brief Construct a new HaEntityActuator object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable entity name in Home Assistant (can be empty if main entity of a device).
   * @param child_object_id Optional machine-readable child identifier to distinguish multiple actuators of the same
   * type on the same node.
   * @param configuration Configuration for this actuator.
   */
  HaEntityActuator(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                   const Configuration &configuration);

  /**
   * @brief Convenience constructor without child_object_id.
   */
  HaEntityActuator(HaBridge &ha_bridge, std::string name, const Configuration &configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish numeric state to `state_topic` regardless of whether it changed. Non-finite values are ignored.
   */
  void publish(double value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish numeric state to `state_topic` only if it changed. Non-finite values are ignored.
   */
  void update(double value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish string/JSON state to `state_topic` regardless of whether it changed.
   */
  void publish(const std::string &value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish string/JSON state to `state_topic` only if it changed.
   */
  void update(const std::string &value, const Attributes::Map &attributes = {});

  /**
   * @brief Publish attributes only (`with_attributes` must be true).
   */
  void publishAttributes(const Attributes::Map &attributes);

  /**
   * @brief Publish attributes only if changed (`with_attributes` must be true).
   */
  void updateAttributes(const Attributes::Map &attributes);

  /**
   * @brief Subscribe to raw command payloads sent by Home Assistant on `command_topic`.
   */
  bool onCommand(std::function<void(const std::string &)> callback);

  /**
   * @brief Get the resolved state topic for this actuator.
   */
  std::string stateTopic() const;

  /**
   * @brief Get the resolved command topic for this actuator.
   */
  std::string commandTopic() const;

  /**
   * @brief Get the resolved JSON attributes topic for this actuator.
   */
  std::string attributesTopic() const;

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string resolveTopic(HaBridge::TopicType type) const;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _component;
  std::string _object_id;
  std::string _child_object_id;
  std::optional<std::string> _sub_topic;
  std::optional<std::string> _device_class;
  std::optional<std::string> _unit_of_measurement;
  std::optional<std::string> _icon;
  std::optional<std::string> _entity_category;
  std::optional<uint8_t> _suggested_display_precision;
  std::optional<bool> _enabled_by_default;
  std::vector<std::string> _options;
  std::function<void(IJsonDocument &)> _extra_configuration;
  bool _with_state_topic;
  bool _with_command_topic;
  bool _with_attributes;
  bool _retain;
  bool _emit_retain_when_false;
  bool _optimistic;

private:
  std::optional<std::string> _value;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_ACTUATOR_H__
