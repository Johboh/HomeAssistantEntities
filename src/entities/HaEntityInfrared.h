// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_INFRARED_H__
#define __HA_ENTITY_INFRARED_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Infrared entity (`infrared.mqtt`).
 *
 * Supports infrared transmitters (emitters) for blasting IR signals and infrared receivers for capturing IR codes,
 * or combined transceivers.
 *
 * See https://www.home-assistant.io/integrations/infrared.mqtt/
 */
class HaEntityInfrared : public HaEntity {
public:
  enum class Type {
    Emitter,
    Receiver,
    Transceiver,
  };

  struct Configuration {
    /**
     * @brief Type of infrared device: Emitter (transmit), Receiver (receive), or Transceiver (both).
     */
    Type type = Type::Transceiver;

    /**
     * @brief Optional template for outgoing commands.
     */
    std::string command_template = "";

    /**
     * @brief Optional template for incoming states.
     */
    std::string value_template = "";

    /**
     * @brief Optional custom icon (e.g. `"mdi:remote"`).
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

  inline static Configuration _default = {.type = Type::Transceiver,
                                          .command_template = "",
                                          .value_template = "",
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityInfrared(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                   Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish a received infrared code or JSON timing payload to Home Assistant.
   */
  void publish(const std::string &code);

  /**
   * @brief Register a callback invoked when Home Assistant commands an infrared signal to be transmitted.
   */
  bool onCommand(std::function<void(const std::string &code)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

  std::optional<std::string> _last_code;
};

#endif // __HA_ENTITY_INFRARED_H__
