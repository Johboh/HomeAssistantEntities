// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_TEXT_H__
#define __HA_ENTITY_TEXT_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents a controllable MQTT Text input entity (`text.mqtt`) that accepts free-form text from Home Assistant
 * and optionally reports its current text state (`Configuration::with_state_topic = true`).
 *
 * For a read-only string sensor, use `HaEntityString` instead.
 * See https://www.home-assistant.io/integrations/text.mqtt/
 *
 * @par Example
 * @code
 * HaEntityText display_msg(ha_bridge, "Display Message", "display_msg", {
 *     .max_text_length = 64,
 *     .with_state_topic = true,
 * });
 *
 * display_msg.onCommand([&](std::string text) {
 *   showOnDisplay(text);
 *   display_msg.publish(text);
 * });
 *
 * // Once MQTT is connected:
 * display_msg.publishConfiguration();
 * display_msg.publish("Ready");
 * @endcode
 */
class HaEntityText : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Minimum length (in characters) of text being set or received (default `0`).
     */
    uint8_t min_text_length = 0;

    /**
     * @brief Maximum length (in characters) of text being set or received (default and Home Assistant maximum is
     * `255`).
     */
    uint8_t max_text_length = 255;

    /**
     * @brief If `true`, configures `state_topic` so the entity can publish its current text state back to Home
     * Assistant. If `false` (default), the text entity is write-only.
     */
    bool with_state_topic = false;

    /**
     * @brief If `true`, sets `mode: "password"` so Home Assistant masks the text input in the UI.
     */
    bool is_password = false;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional regular expression pattern that text set or received must match in Home Assistant.
     */
    std::string pattern = "";

    /**
     * @brief Optional custom icon (e.g. `"mdi:form-textbox"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the text entity works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.min_text_length = 0,
                                          .max_text_length = 255,
                                          .with_state_topic = false,
                                          .is_password = false,
                                          .retain = false,
                                          .pattern = "",
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityText object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Hallway"` + name `"Message"` ->
   * `"Hallway Message"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several text
   * entities on the same node ID. Leave empty (`""`) when not needed.
   * @param configuration Configuration for this text entity.
   */
  HaEntityText(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
               Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current text regardless of whether it has changed (`Configuration::with_state_topic` must be
   * `true`). Also see `update()`.
   *
   * @param str Text string to publish.
   */
  void publish(const std::string &str);

  /**
   * @brief Publish the current text only if it has changed (`Configuration::with_state_topic` must be `true`). Also see
   * `publish()`.
   *
   * @param str Text string to publish.
   */
  void update(const std::string &str);

  /**
   * @brief Set callback for receiving text commands from Home Assistant.
   *
   * @param callback Callback invoked with the submitted text string.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(std::string)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  static Configuration sanitizeConfiguration(Configuration configuration);

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_TEXT_H__
