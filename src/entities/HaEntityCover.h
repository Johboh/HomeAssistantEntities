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
 * @brief Represents an MQTT Cover (`cover.mqtt`) such as blinds, curtains, shades, shutters, or garage doors.
 *
 * Supports bidirectional state (`Open`, `Opening`, `Closed`, `Closing`, `Stopped`), numeric position (`0..100` by
 * default), and optional slat tilt (`0..100` by default), or read-only reporting when `Configuration::read_only` is
 * `true`.
 * See https://www.home-assistant.io/integrations/cover.mqtt/
 *
 * @par Example
 * @code
 * HaEntityCover blind(ha_bridge, "Window Blind", "window", {
 *     .device_class = homeassistantentities::Cover::Blind{},
 *     .with_tilt = true,
 * });
 *
 * blind.onCommand([&](HaEntityCover::Action action) {
 *   if (action == HaEntityCover::Action::Open) {
 *     blind.publish(HaEntityCover::State::Open, 100);
 *   } else if (action == HaEntityCover::Action::Close) {
 *     blind.publish(HaEntityCover::State::Closed, 0);
 *   } else if (action == HaEntityCover::Action::Stop) {
 *     blind.publish(HaEntityCover::State::Stopped);
 *   }
 * });
 * blind.onPosition([&](uint8_t pos) {
 *   blind.publish(pos == 0 ? HaEntityCover::State::Closed : HaEntityCover::State::Open, pos);
 * });
 * blind.onTilt([&](uint8_t tilt) {
 *   blind.publishTilt(tilt);
 * });
 *
 * // Once MQTT is connected:
 * blind.publishConfiguration();
 * blind.publish(HaEntityCover::State::Closed, 0);
 * blind.publishTilt(50);
 * @endcode
 */
class HaEntityCover : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class for this cover (e.g. `"curtain"`, `"shade"`, `"garage"`, or
     * `homeassistantentities::Cover::Shade{}`). Leave empty for a generic cover.
     * See https://www.home-assistant.io/integrations/cover/#device-class
     */
    std::string device_class = "";

    /**
     * @brief If `true`, the cover only reports its state, position, and tilt to Home Assistant and cannot be
     * controlled (no command, set-position, or tilt-command topics are configured). When `false` (default), command
     * topics are configured for each enabled capability (`with_state_topic`, `with_position`, `with_tilt`).
     */
    bool read_only = false;

    /**
     * @brief Device position value representing a fully closed cover (default `0`).
     *
     * Home Assistant maps `[position_closed, position_open]` onto `0..100%`. Setting `position_closed` >
     * `position_open` (e.g. `100` and `0`) inverts the position direction in Home Assistant.
     * See https://www.home-assistant.io/integrations/cover.mqtt/
     */
    uint8_t position_closed = 0;

    /**
     * @brief Device position value representing a fully open cover (default `100`). See `position_closed`.
     */
    uint8_t position_open = 100;

    /**
     * @brief If `true` (default), configure `position_topic` (and `set_position_topic` when `!read_only`). Set to
     * `false` for covers that only report or accept discrete open/closed/stopped states.
     */
    bool with_position = true;

    /**
     * @brief If `true` (default), configure `state_topic` (and `command_topic` when `!read_only`). Set to `false` for
     * covers that only report or accept numeric position.
     */
    bool with_state_topic = true;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:curtains"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the cover works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If `true`, configure `tilt_status_topic` (and `tilt_command_topic` when `!read_only`) for slat/tilt
     * control.
     */
    bool with_tilt = false;

    /**
     * @brief Minimum tilt value (default `0`). Must be `<= tilt_max`.
     */
    uint8_t tilt_min = 0;

    /**
     * @brief Maximum tilt value (default `100`).
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
   * @brief Construct a new HaEntityCover object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Living Room"` + name `"Cover"`
   * -> `"Living Room Cover"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * covers on the same node ID. Leave empty (`""`) when not needed.
   * @param configuration Configuration for this cover.
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
   * @brief Publish the cover state and/or position regardless of whether the values have changed. Also see `update()`.
   *
   * @param state Cover state, or `std::nullopt` to leave state unpublished.
   * @param position Cover position between `Configuration::position_closed` (default `0`) and
   * `Configuration::position_open` (default `100`), or `std::nullopt` to leave position unpublished.
   */
  void publish(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  /**
   * @brief Publish the cover state and/or position only for values that have changed. Also see `publish()`.
   *
   * @param state Cover state, or `std::nullopt` to leave state unchanged.
   * @param position Cover position between `Configuration::position_closed` (default `0`) and
   * `Configuration::position_open` (default `100`), or `std::nullopt` to leave position unchanged.
   */
  void update(std::optional<State> state, std::optional<uint8_t> position = std::nullopt);

  /**
   * @brief Publish the cover tilt value regardless of whether it has changed (`Configuration::with_tilt` must be
   * `true`). Also see `updateTilt()`.
   *
   * @param tilt Tilt value clamped between `Configuration::tilt_min` and `Configuration::tilt_max`.
   */
  void publishTilt(uint8_t tilt);

  /**
   * @brief Publish the cover tilt value only if it has changed (`Configuration::with_tilt` must be `true`). Also see
   * `publishTilt()`.
   *
   * @param tilt Tilt value clamped between `Configuration::tilt_min` and `Configuration::tilt_max`.
   */
  void updateTilt(uint8_t tilt);

  enum class Action {
    Open,
    Close,
    Stop,
    Unknown,
  };

  /**
   * @brief Set callback for receiving discrete commands (`Open`, `Close`, `Stop`) from Home Assistant.
   * Not subscribed if `Configuration::read_only` is `true` or `Configuration::with_state_topic` is `false`.
   *
   * @param callback Callback invoked with the requested `Action`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(Action)> callback);

  /**
   * @brief Set callback for receiving numeric position commands from Home Assistant.
   * Not subscribed if `Configuration::read_only` is `true` or `Configuration::with_position` is `false`.
   *
   * @param callback Callback invoked with the target position clamped between `Configuration::position_closed` and
   * `Configuration::position_open`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onPosition(std::function<void(uint8_t)> callback);

  /**
   * @brief Set callback for receiving numeric tilt commands from Home Assistant.
   * Requires `Configuration::with_tilt` to be `true` and `Configuration::read_only` to be `false`.
   *
   * @param callback Callback invoked with the target tilt clamped between `Configuration::tilt_min` and
   * `Configuration::tilt_max`.
   * @return true if the subscription succeeded, false otherwise.
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
