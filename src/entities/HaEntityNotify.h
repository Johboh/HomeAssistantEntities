// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_NOTIFY_H__
#define __HA_ENTITY_NOTIFY_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Notify entity (`notify.mqtt`).
 *
 * Allows Home Assistant to send text notifications to the microcontroller (e.g. to display on an OLED/LCD screen or
 * announce through a speaker).
 *
 * See https://www.home-assistant.io/integrations/notify.mqtt/
 */
class HaEntityNotify : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional custom icon (e.g. `"mdi:bell"`).
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

  inline static Configuration _default = {.icon = "", .entity_category = "", .enabled_by_default = std::nullopt};

  HaEntityNotify(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override {}

  /**
   * @brief Set callback for receiving incoming notifications with message and title.
   */
  bool onNotify(std::function<void(const std::string &message, const std::string &title)> callback);

  /**
   * @brief Set callback for receiving incoming notification messages.
   */
  bool onNotify(std::function<void(const std::string &message)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_NOTIFY_H__
