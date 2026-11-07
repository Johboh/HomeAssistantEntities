// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SELECT_H__
#define __HA_ENTITY_SELECT_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Select entity (`select.mqtt`) allowing users to choose an option from a predefined list in
 * Home Assistant and reporting the currently active option.
 *
 * See https://www.home-assistant.io/integrations/select.mqtt/
 *
 * @par Example
 * @code
 * HaEntitySelect mode_select(ha_bridge, "Operating Mode", "operating_mode", {
 *     .options = {"auto", "eco", "boost"},
 * });
 *
 * mode_select.onCommand([&](std::string option) {
 *   applyOperatingMode(option);
 *   mode_select.publish(option);
 * });
 *
 * // Once MQTT is connected:
 * mode_select.publishConfiguration();
 * mode_select.publish("auto");
 * @endcode
 */
class HaEntitySelect : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief List of selectable option strings (order is preserved in the Home Assistant UI).
     */
    std::vector<std::string> options = {};

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:format-list-bulleted"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the select entity works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  /**
   * @brief Construct a new HaEntitySelect object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Balcony"` + name `"Profile"` ->
   * `"Balcony Profile"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param object_id Unique machine-readable object ID (`[a-zA-Z0-9_-]`) for this select on this node/device (e.g.
   * `"balcony_leds_profile"`).
   * @param configuration Configuration for this select entity.
   */
  HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the currently selected option regardless of whether the value has changed. Must be one of the
   * strings in `Configuration::options`. Also see `update()`.
   *
   * @param option Selected option string.
   */
  void publish(const std::string &option);

  /**
   * @brief Publish the currently selected option only if the value has changed. Must be one of the strings in
   * `Configuration::options`. Also see `publish()`.
   *
   * @param option Selected option string.
   */
  void update(const std::string &option);

  /**
   * @brief Set callback for receiving option selection commands from Home Assistant.
   *
   * @param callback Callback invoked with the newly selected option string.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(std::string)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_SELECT_H__
