// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_DATE_H__
#define __HA_ENTITY_DATE_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Date entity (`date.mqtt`) that accepts and reports dates in `YYYY-MM-DD` format.
 *
 * See https://www.home-assistant.io/integrations/date.mqtt/
 */
class HaEntityDate : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true` (default), configures `state_topic` so the entity can publish its current date state back to
     * Home Assistant.
     */
    bool with_state_topic = true;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the date works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:calendar"`).
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

  HaEntityDate(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
               Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the date string formatted as `YYYY-MM-DD`. Also see `update()`.
   */
  void publish(const std::string &date_str);

  /**
   * @brief Publish the date specifying year, month (1..12), and day (1..31). Also see `update()`.
   */
  void publish(int year, int month, int day);

  /**
   * @brief Publish the date only if it has changed. Also see `publish()`.
   */
  void update(const std::string &date_str);

  /**
   * @brief Publish the date only if it has changed. Also see `publish()`.
   */
  void update(int year, int month, int day);

  /**
   * @brief Set callback for receiving date string commands (`"YYYY-MM-DD"`) from Home Assistant.
   */
  bool onCommand(std::function<void(const std::string &date_str)> callback);

  /**
   * @brief Set callback for receiving date commands with parsed year, month, day.
   */
  bool onCommand(std::function<void(int year, int month, int day)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_DATE_H__
