// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_TAG_H__
#define __HA_ENTITY_TAG_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Tag scanner entity (e.g. NFC or RFID reader).
 *
 * When an RFID/NFC tag is scanned, publishes a tag scanned event payload to Home Assistant
 * (`homeassistant/tag/<device_id>/tag_scanned`).
 *
 * See https://www.home-assistant.io/integrations/tag/
 */
class HaEntityTag : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional custom topic prefix (defaults to `"homeassistant"`).
     */
    std::string tag_prefix = "homeassistant";
  };

  inline static Configuration _default = {.tag_prefix = "homeassistant"};

  HaEntityTag(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
              Configuration configuration = _default);

public:
  void publishConfiguration() override {}
  void unpublishConfiguration() override {}
  void republishState() override {}

  /**
   * @brief Publish a scanned tag event to Home Assistant.
   *
   * @param tag_id Unique identifier string of the scanned RFID/NFC tag.
   */
  void publishTag(const std::string &tag_id);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override {
    (void)out;
    return false;
  }

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_TAG_H__
