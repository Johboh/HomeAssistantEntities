// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_SCENE_H__
#define __HA_ENTITY_SCENE_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Scene entity (`scene.mqtt`).
 *
 * When activated from the Home Assistant frontend or automations, Home Assistant publishes a trigger message
 * to the command topic, which calls the registered `onActivate` callback.
 *
 * See https://www.home-assistant.io/integrations/scene.mqtt/
 */
class HaEntityScene : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Payload received when the scene is activated (default `"ON"`).
     */
    std::string payload_on = "ON";

    /**
     * @brief Optional custom icon (e.g. `"mdi:palette"`).
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
      .payload_on = "ON", .icon = "", .entity_category = "", .enabled_by_default = std::nullopt};

  HaEntityScene(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override {}

  /**
   * @brief Set callback invoked whenever this scene is activated in Home Assistant.
   */
  bool onActivate(std::function<void()> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_SCENE_H__
