// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_UPDATE_H__
#define __HA_ENTITY_UPDATE_H__

#include "AttributeVariants.h"
#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Firmware/Software Update entity in Home Assistant, reporting installed/available versions and
 * optionally receiving install commands (OTA update triggers).
 * See more at https://www.home-assistant.io/integrations/update.mqtt/
 */
class HaEntityUpdate : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Device class for this update entity (defaults to "firmware", or leave empty "" for generic software
     * update). See https://www.home-assistant.io/integrations/update/#device-class
     */
    std::string device_class = "firmware";

    /**
     * @brief If true, configures `command_topic` and `payload_install` so users can trigger updates from Home
     * Assistant. Set to false for read-only update notifications.
     */
    bool can_install = true;

    /**
     * @brief If true, tells Home Assistant to publish install command messages with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:package-up").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If true, configure `json_attributes_topic` for publishing extra JSON attributes.
     */
    bool with_attributes = false;
  };

  inline static Configuration _default = {.device_class = "firmware",
                                          .can_install = true,
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt,
                                          .with_attributes = false};

  struct State {
    /**
     * @brief Version currently installed on the device (required).
     */
    std::string installed_version;

    /**
     * @brief Latest version available (defaults to `installed_version` when empty, indicating up-to-date).
     */
    std::string latest_version = "";

    /**
     * @brief Optional title of the software/firmware update.
     */
    std::string title = "";

    /**
     * @brief Optional brief release summary (up to 255 characters in Home Assistant).
     */
    std::string release_summary = "";

    /**
     * @brief Optional URL to full release notes / changelog.
     */
    std::string release_url = "";

    /**
     * @brief Optional URL to an entity picture / icon for the update.
     */
    std::string entity_picture = "";

    /**
     * @brief Optional flag indicating whether an update installation is currently in progress.
     */
    std::optional<bool> in_progress = std::nullopt;

    /**
     * @brief Optional installation progress percentage (0-100).
     */
    std::optional<uint8_t> update_percentage = std::nullopt;
  };

  /**
   * @brief Construct a new Ha Entity Update object.
   */
  HaEntityUpdate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish full update state JSON to `state_topic` regardless of whether it changed.
   */
  void publish(const State &state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish full update state JSON to `state_topic` only if it changed.
   */
  void update(const State &state, const Attributes::Map &attributes = {});

  /**
   * @brief Convenience helper to publish installed and latest version strings.
   */
  void publish(const std::string &installed_version, const std::string &latest_version,
               const Attributes::Map &attributes = {});

  /**
   * @brief Convenience helper to update installed and latest version strings only if changed.
   */
  void update(const std::string &installed_version, const std::string &latest_version,
              const Attributes::Map &attributes = {});

  /**
   * @brief Set callback invoked when the user triggers "Install" in Home Assistant (`can_install` must be true).
   */
  bool onCommand(std::function<void(void)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  static std::optional<std::string> encodeState(const State &state);

private:
  bool _can_install;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_UPDATE_H__
