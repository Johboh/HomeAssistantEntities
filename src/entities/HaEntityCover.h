// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_COVER_H__
#define __HA_ENTITY_COVER_H__

#include "HaDeviceClasses.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Cover that can be controlled from Home Assistant. It goes two ways, as the cover can be
 * changed independently and will report back state to Home Assistant. See
 * https://www.home-assistant.io/integrations/cover.mqtt/
 */
class HaEntityCover : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class for this cover (e.g. "curtain", "shade", "garage", or
     * homeassistantentities::Cover::Shade{}). Leave empty for generic cover.
     * See https://www.home-assistant.io/integrations/cover/#device-class
     */
    std::string device_class = "";

    /**
     * If set to true, this device is only reporting its state and position, but cannot be controlled by Home Assistant.
     * Usually, Home Assistant can open and close a cover/curtain/awning by posting the position to the position topic.
     * If this is set to false, the position topic will be created and Home Assistant can post the position.
     */
    bool read_only = false;

    /**
     * From Home Assistant perspective:
     * MQTT cover expects position and tilt values to be in range of 0 to 100, where 0 indicates closed position and 100
     * indicates fully open position. If position min or max are set to a different range (e.g. 40 to 140), when sending
     * command to the device the range will be adjusted to the device range (position 0 will send a value of 40 to
     * device) and when position payload is received from the device it will be adjusted back to the 0 to 100 range
     * (device value of 40 will report cover position 0). min and max can also be used to reverse the direction of the
     * device, if min is set to 100 and max is set to 0 device operation will be inverted (e.g. when setting position to
     * 40, a value of 60 will be sent to device).
     *
     * Source: https://www.home-assistant.io/integrations/cover.mqtt/
     * Default: 0
     */
    uint8_t position_closed = 0;
    /**
     * see position_closed.
     * Default: 100
     */
    uint8_t position_open = 100;

    /**
     * @brief If true (default), configure position_topic (and set_position_topic when !read_only). Set to false for
     * covers that only report/accept discrete open/closed/stopped states.
     */
    bool with_position = true;

    /**
     * @brief If true (default), configure state_topic (and command_topic when !read_only). Set to false for covers that
     * only report/accept numeric position.
     */
    bool with_state_topic = true;

    /**
     * @brief If true, tells Home Assistant to publish command messages with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:curtains").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag that defines if cover works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If true, configure tilt_status_topic (and tilt_command_topic when !read_only) for slat/tilt control.
     */
    bool with_tilt = false;

    /**
     * @brief Minimum tilt value (default 0). Must be <= tilt_max.
     */
    uint8_t tilt_min = 0;

    /**
     * @brief Maximum tilt value (default 100).
     */
    uint8_t tilt_max = 100;
  };

  inline static Configuration _default = {.device_class = "",
                                          .read_only = false,
                                          .position_closed = 0,
                                          .position_open = 100,
                                          .with_position = true,
                                          .with_state_topic = true,
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt,
                                          .with_tilt = false,
                                          .tilt_min = 0,
                                          .tilt_max = 100};

  /**
   * @brief Construct a new Ha Entity Cover object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "cover", friendly name will be "Bathroom cover". If no
   * device, friendly name will be just "cover". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several covers of the same
   * entity type for the same node ID. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityCover(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  enum class State {
    Open,
    Opening,
    Closed,
    Closing,
    Stopped,
    Unknown,
  };

  /**
   * @brief Publish the cover values. This will publish to MQTT regardless if the values has changed. Also see
   * update().
   *
   * @param state the cover state. Or absent (std::nullopt) if state is unknown.
   * @param position of the cover, between Configuration::position_closed (default 0) and Configuration::position_open
   * (default 100). Or absent (std::nullopt) if position is unknown.
   */
  void publish(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  /**
   * @brief Publish the cover values, but only if the values has changed. Also see publish().
   *
   * @param state the cover state. Or absent (std::nullopt) if state is unknown.
   * @param position of the cover, between Configuration::position_closed (default 0) and Configuration::position_open
   * (default 100). Or absent (std::nullopt) if position is unknown.
   */
  void update(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  /**
   * @brief Publish the cover tilt value regardless of whether it changed (`with_tilt` must be true).
   */
  void publishTilt(uint8_t tilt);

  /**
   * @brief Publish the cover tilt value only if it changed (`with_tilt` must be true).
   */
  void updateTilt(uint8_t tilt);

  enum class Action {
    Open,
    Close,
    Stop,
    Unknown,
  };

  /**
   * @brief Set callback for receiving state commands (Open, Close, Stop) from Home Assistant.
   * Will not be called if read_only is true in Configuration.
   */
  bool onCommand(std::function<void(Action)> callback);

  /**
   * @brief Set callback for receiving position commands from Home Assistant, value between
   * Configuration::position_closed (default 0) and Configuration::position_open (default 100). Will not be called if
   * read_only is true in Configuration.
   */
  bool onPosition(std::function<void(uint8_t)> callback);

  /**
   * @brief Set callback for receiving tilt commands from Home Assistant (`with_tilt` must be true and !read_only).
   */
  bool onTilt(std::function<void(uint8_t)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  uint8_t clampPosition(uint8_t position) const;
  uint8_t clampTilt(uint8_t tilt) const;
  void publishState(std::optional<State> state);
  void publishPosition(std::optional<uint8_t> position);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<State> _state;
  std::optional<uint8_t> _position;
  std::optional<uint8_t> _tilt;
};

#endif // __HA_ENTITY_COVER_H__
