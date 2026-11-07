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
 * @brief Represents an MQTT Switch (`switch.mqtt`) supporting bidirectional on/off control and state reporting.
 *
 * See https://www.home-assistant.io/integrations/switch.mqtt/
 *
 * @par Example
 * @code
 * HaEntitySwitch pump(ha_bridge, "Water Pump", "pump", {
 *     .device_class = homeassistantentities::Switch::Switch{},
 * });
 *
 * pump.onCommand([&](bool on) {
 *   setPumpRelay(on);
 *   pump.publish(on);
 * });
 *
 * // Once MQTT is connected:
 * pump.publishConfiguration();
 * pump.publish(false);
 * @endcode
 */
class HaEntitySwitch : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional device class for this switch (e.g. `"outlet"`, `"switch"`, or
     * `homeassistantentities::Switch::Outlet{}`). Leave empty for a generic switch.
     * See https://www.home-assistant.io/integrations/switch/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Optional custom icon (e.g. `"mdi:power-socket-eu"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the switch works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
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
   * @brief Construct a new HaEntitySwitch object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Switch"` ->
   * `"Bathroom Switch"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * switches on the same node ID. Leave empty (`""`) when not needed.
   * @param configuration Configuration for this switch.
   */
  HaEntitySwitch(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the switch on/off state regardless of whether the state has changed. Also see `update()`.
   *
   * @param on `true` if the switch is on (`"ON"`), `false` if off (`"OFF"`).
   */
  void publish(bool on);

  /**
   * @brief Publish the switch on/off state only if the state has changed. Also see `publish()`.
   *
   * @param on `true` if the switch is on (`"ON"`), `false` if off (`"OFF"`).
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving on/off commands from Home Assistant.
   *
   * @param callback Callback invoked with `true` for `"ON"` or `false` for `"OFF"`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(bool)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_SWITCH_H__
