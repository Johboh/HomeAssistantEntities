// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_NUMBER_H__
#define __HA_ENTITY_NUMBER_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Number sensor/actuator.
 * See more at https://www.home-assistant.io/integrations/number.mqtt/
 */
class HaEntityNumber : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The minimum value allowed.
     */
    float min_value = 1.0f;

    /**
     * @brief The maximum value allowed.
     */
    float max_value = 100.0f;

    /**
     * @brief Step value. Smallest value 0.001.
     */
    float step = 1.0f;

    /**
     * @brief the unit of measurement reported for this number. Make sure that the value you publish is of this unit.
     */
    std::string unit = "";

    /**
     * @brief the device class for this number (e.g. "duration", "temperature", or a class from
     * homeassistantentities::Sensor::*). See https://www.home-assistant.io/integrations/number/#device-class
     */
    std::string device_class = "";

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;

    enum class Mode {
      Auto,
      Box,
      Slider,
    };

    /**
     * @brief Control how the number should be displayed in the Home Assistant UI ("auto", "box", or "slider").
     */
    Mode mode = Mode::Auto;

    /**
     * @brief Optional custom icon (e.g. "mdi:speedometer").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag that defines if number works in optimistic mode.
     */
    bool optimistic = false;

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

  inline static Configuration _default = {.min_value = 1.0f,
                                          .max_value = 100.0f,
                                          .step = 1.0f,
                                          .unit = "",
                                          .device_class = "",
                                          .retain = false,
                                          .mode = Configuration::Mode::Auto,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .suggested_display_precision = std::nullopt,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Number object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "number", friendly name will be "Bathroom number". If no
   * device, friendly name will be just "number". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this number. Should be unique for each number for this node/device. Example:
   * "balcony_leds_speed". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the number. This will publish to MQTT regardless if the value has changed. Also see
   * update().
   */
  void publish(float number);

  /**
   * @brief Publish the number, but only if the value has changed. Also see publish().
   */
  void update(float number);

  /**
   * @brief Set callback for receiving commands from Home Assistant.
   */
  bool onCommand(std::function<void(float)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  static Configuration sanitizeConfiguration(Configuration configuration);

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_NUMBER_H__
