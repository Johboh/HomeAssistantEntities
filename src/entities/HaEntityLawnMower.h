// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_LAWN_MOWER_H__
#define __HA_ENTITY_LAWN_MOWER_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Lawn Mower entity (`lawn_mower.mqtt`).
 *
 * Supports activity state reporting (`mowing`, `docked`, `paused`, `error`) and commands (`start_mowing`, `pause`,
 * `dock`).
 *
 * See https://www.home-assistant.io/integrations/lawn_mower.mqtt/
 */
class HaEntityLawnMower : public HaEntity {
public:
  enum class Activity {
    Mowing,
    Docked,
    Paused,
    Error,
  };

  enum class Action {
    StartMowing,
    Pause,
    Dock,
  };

  static std::string activityToString(Activity activity);
  static std::optional<Activity> stringToActivity(const std::string &str);

  struct Configuration {
    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = false;

    /**
     * @brief If `true`, publishes messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:robot-mower"`).
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

  inline static Configuration _default = {
      .with_attributes = false, .retain = false, .icon = "", .entity_category = "", .enabled_by_default = std::nullopt};

  HaEntityLawnMower(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                    Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the activity state using typed `Activity`. Also see `update()`.
   */
  void publish(Activity activity);

  /**
   * @brief Publish the activity state string. Also see `update()`.
   */
  void publish(const std::string &activity);

  /**
   * @brief Publish the activity state only if it has changed. Also see `publish()`.
   */
  void update(Activity activity);

  /**
   * @brief Publish the activity state only if it has changed. Also see `publish()`.
   */
  void update(const std::string &activity);

  /**
   * @brief Set callback for receiving mower action commands (`StartMowing`, `Pause`, `Dock`).
   */
  bool onCommand(std::function<void(Action action)> callback);

  /**
   * @brief Publish custom JSON attributes (`json_attributes_topic`).
   */
  void publishAttributes(const Attributes::Map &attributes);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

  std::optional<std::string> _activity;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_LAWN_MOWER_H__
