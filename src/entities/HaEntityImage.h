// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_IMAGE_H__
#define __HA_ENTITY_IMAGE_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Image entity (`image.mqtt`).
 *
 * Exposes an image in Home Assistant either by streaming raw image bytes (`image_topic`) or publishing an image URL
 * (`url_topic`).
 *
 * See https://www.home-assistant.io/integrations/image.mqtt/
 */
class HaEntityImage : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Content type (MIME type) of the image (default `"image/jpeg"`).
     */
    std::string content_type = "image/jpeg";

    /**
     * @brief Whether the image is supplied via URL rather than binary payload.
     */
    bool url_mode = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:image"`).
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

  inline static Configuration _default = {.content_type = "image/jpeg",
                                          .url_mode = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .enabled_by_default = std::nullopt};

  HaEntityImage(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish an image URL when `Configuration::url_mode` is `true`.
   */
  void publishUrl(const std::string &url);

  /**
   * @brief Publish binary image bytes when `Configuration::url_mode` is `false`.
   */
  void publishBytes(const uint8_t *data, size_t length);

  /**
   * @brief Publish binary image bytes string when `Configuration::url_mode` is `false`.
   */
  void publishBytes(const std::string &data);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

  std::optional<std::string> _url;
};

#endif // __HA_ENTITY_IMAGE_H__
