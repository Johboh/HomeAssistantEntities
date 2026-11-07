// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_BUTTON_H__
#define __HA_ENTITY_BUTTON_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents a stateless MQTT Button (`button.mqtt`) that can be pressed from Home Assistant (command-only).
 *
 * See https://www.home-assistant.io/integrations/button.mqtt/
 *
 * @par Example
 * @code
 * HaEntityButton restart_btn(ha_bridge, "Restart", "restart", {
 *     .device_class = homeassistantentities::Button::Restart{},
 *     .entity_category = "config",
 * });
 *
 * restart_btn.onCommand([]() {
 *   triggerDeviceRestart();
 * });
 *
 * // Once MQTT is connected:
 * restart_btn.publishConfiguration();
 * @endcode
 */
class HaEntityButton : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class for this button (e.g. `"identify"`, `"restart"`, `"update"`, or
     * `homeassistantentities::Button::Restart{}`). Leave empty for a generic button.
     * See https://www.home-assistant.io/integrations/button/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Optional custom icon (e.g. `"mdi:gesture-tap-button"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief If true, tells Home Assistant to publish the command message with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {
      .device_class = "", .icon = "", .entity_category = "", .retain = false, .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityButton object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Restart"` ->
   * `"Bathroom Restart"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * buttons for the same node ID. Leave empty (`""`) when not needed.
   * @param configuration Optional configuration for this button.
   */
  HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Set callback for receiving press commands from Home Assistant.
   *
   * @param callback Callback invoked whenever the button is pressed in Home Assistant.
   * @return true if the MQTT command subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(void)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_BUTTON_H__
