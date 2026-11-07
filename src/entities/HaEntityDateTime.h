// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_DATE_TIME_H__
#define __HA_ENTITY_DATE_TIME_H__

#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT DateTime entity (`datetime.mqtt`) that accepts and reports ISO 8601 date-times
 * (`YYYY-MM-DDTHH:MM:SS`).
 *
 * See https://www.home-assistant.io/integrations/datetime.mqtt/
 */
class HaEntityDateTime : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true` (default), configures `state_topic` so the entity can publish its current date-time state back
     * to Home Assistant.
     */
    bool with_state_topic = true;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Defines whether the date-time works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:calendar-clock"`).
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

  HaEntityDateTime(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                   Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the date-time string formatted as ISO 8601 (`YYYY-MM-DDTHH:MM:SS`). Also see `update()`.
   */
  void publish(const std::string &datetime_str);

  /**
   * @brief Publish date-time components (year, month, day, hour, minute, second). Also see `update()`.
   */
  void publish(int year, int month, int day, int hour, int minute, int second = 0);

  /**
   * @brief Publish the date-time only if it has changed. Also see `publish()`.
   */
  void update(const std::string &datetime_str);

  /**
   * @brief Publish the date-time only if it has changed. Also see `publish()`.
   */
  void update(int year, int month, int day, int hour, int minute, int second = 0);

  /**
   * @brief Set callback for receiving ISO 8601 date-time commands from Home Assistant.
   */
  bool onCommand(std::function<void(const std::string &datetime_str)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  Configuration _configuration;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_DATE_TIME_H__
