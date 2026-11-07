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
 * @brief Represent a Button that can be set/pressed by Home Assistant (one way only).
 * See more at https://www.home-assistant.io/integrations/button.mqtt/
 */
class HaEntityButton : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class for this button (e.g. "identify", "restart", "update", or
     * homeassistantentities::Button::Restart{}).
     * See https://www.home-assistant.io/integrations/button/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Optional custom icon (e.g. "mdi:gesture-tap-button").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief If true, tells Home Assistant to publish the command message with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {
      .device_class = "", .icon = "", .entity_category = "", .retain = false, .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Button object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "button", friendly name will be "Bathroom button". If no
   * device, friendly name will be just "button". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several buttons for the same
   * node ID. Leave as empty string for no child object ID. Valid characters are [a-zA-Z0-9_-] (machine readable, not
   * human readable)
   * @param configuration optional configuration for this button.
   */
  HaEntityButton(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Set callback for receiving press commands from Home Assistant.
   */
  bool onCommand(std::function<void(void)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_BUTTON_H__
