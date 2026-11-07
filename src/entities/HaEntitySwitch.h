// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SWITCH_H__
#define __HA_ENTITY_SWITCH_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Switch that can be set by Home Assistant or reported back to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/switch.mqtt/
 */
class HaEntitySwitch : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional device class for this switch (e.g. "outlet", "switch", or
     * homeassistantentities::Switch::Outlet{}). See https://www.home-assistant.io/integrations/switch/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Optional custom icon (e.g. "mdi:power-socket-eu").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag that defines if switch works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.retain = false,
                                          .device_class = "",
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new Ha Entity Switch object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "switch", friendly name will be "Bathroom switch". If no
   * device, friendly name will be just "switch". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several switches for the same
   * node ID. Leave as empty string for no child object ID. Valid characters are [a-zA-Z0-9_-] (machine readable, not
   * human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntitySwitch(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the switch state. This will publish to MQTT regardless if the state has changed. Also see
   * update().
   *
   * @param on if switch is on or off.
   */
  void publish(bool on);

  /**
   * @brief Publish the switch state, but only if the state has changed. Also see publish().
   *
   * @param on if switch is on or off.
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving commands from Home Assistant.
   */
  bool onCommand(std::function<void(bool)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_SWITCH_H__
