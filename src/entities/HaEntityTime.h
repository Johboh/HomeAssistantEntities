// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_TIME_H__
#define __HA_ENTITY_TIME_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Time entity (`time.mqtt`) that accepts and reports times in `HH:MM:SS` format.
 *
 * See https://www.home-assistant.io/integrations/time.mqtt/
 */
class HaEntityTime : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true` (default), configures `state_topic` so the entity can publish its current time state back to
     * Home Assistant.
     */
    bool with_state_topic = true;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the time works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:clock"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_state_topic = true,
                                          .retain = false,
                                          .optimistic = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityTime(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
               Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the time string formatted as `HH:MM:SS`. Also see `update()`.
   */
  void publish(const std::string &time_str);

  /**
   * @brief Publish the time specifying hour (0..23), minute (0..59), and second (0..59). Also see `update()`.
   */
  void publish(int hour, int minute, int second = 0);

  /**
   * @brief Publish the time only if it has changed. Also see `publish()`.
   */
  void update(const std::string &time_str);

  /**
   * @brief Publish the time only if it has changed. Also see `publish()`.
   */
  void update(int hour, int minute, int second = 0);

  /**
   * @brief Set callback for receiving time string commands (`"HH:MM:SS"`) from Home Assistant.
   */
  bool onCommand(std::function<void(const std::string &time_str)> callback);

  /**
   * @brief Set callback for receiving time commands with parsed hour, minute, second.
   */
  bool onCommand(std::function<void(int hour, int minute, int second)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_TIME_H__
