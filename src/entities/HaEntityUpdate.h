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
 * @brief Represents an MQTT Update entity (`update.mqtt`) that reports installed and available firmware/software
 * versions (plus optional OTA progress) and optionally receives install commands from Home Assistant.
 *
 * See https://www.home-assistant.io/integrations/update.mqtt/
 *
 * @par Example
 * @code
 * HaEntityUpdate firmware(ha_bridge, "Firmware", "ota", {
 *     .device_class = "firmware",
 *     .can_install = true,
 * });
 *
 * firmware.onCommand([&]() {
 *   firmware.publish({.installed_version = "1.0.0", .latest_version = "1.1.0", .in_progress = true});
 *   startOtaUpdate();
 * });
 *
 * // Once MQTT is connected:
 * firmware.publishConfiguration();
 * firmware.publish("1.0.0", "1.1.0");
 * @endcode
 */
class HaEntityUpdate : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Device class for this update entity (defaults to `"firmware"`, or leave empty `""` for a generic software
     * update). See https://www.home-assistant.io/integrations/update/#device-class
     */
    std::string device_class = "firmware";

    /**
     * @brief If `true` (default), configures `command_topic` and `payload_install` so users can trigger updates from
     * Home Assistant. Set to `false` for read-only update notifications.
     */
    bool can_install = true;

    /**
     * @brief If `true`, tells Home Assistant to publish install command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:package-up"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing extra JSON attributes.
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
     * @brief Version currently installed on the device (required; must be non-empty).
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
     * @brief Optional installation progress percentage (`0..100`).
     */
    std::optional<uint8_t> update_percentage = std::nullopt;
  };

  /**
   * @brief Construct a new HaEntityUpdate object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Gateway"` + name `"Firmware"` ->
   * `"Gateway Firmware"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * update entities on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this update entity.
   */
  HaEntityUpdate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish full update `State` JSON to `state_topic` regardless of whether it has changed. Also see `update()`.
   *
   * @param state Update state structure (`installed_version` must be non-empty).
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(const State &state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish full update `State` JSON to `state_topic` only if it (or `attributes`) has changed. Also see
   * `publish()`.
   *
   * @param state Update state structure (`installed_version` must be non-empty).
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(const State &state, const Attributes::Map &attributes = {});

  /**
   * @brief Convenience helper to publish installed and latest version strings regardless of whether they have changed.
   *
   * @param installed_version Version currently installed on the device.
   * @param latest_version Latest available version.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(const std::string &installed_version, const std::string &latest_version,
               const Attributes::Map &attributes = {});

  /**
   * @brief Convenience helper to publish installed and latest version strings only if they (or `attributes`) have
   * changed.
   *
   * @param installed_version Version currently installed on the device.
   * @param latest_version Latest available version.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(const std::string &installed_version, const std::string &latest_version,
              const Attributes::Map &attributes = {});

  /**
   * @brief Set callback invoked when a user triggers "Install" in Home Assistant (`Configuration::can_install` must be
   * `true`).
   *
   * @param callback Callback invoked when the install command arrives.
   * @return true if the subscription succeeded, false otherwise.
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
