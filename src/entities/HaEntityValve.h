// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_VALVE_H__
#define __HA_ENTITY_VALVE_H__

#include "AttributeVariants.h"
#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Valve actuator (`valve.mqtt`) for water, gas, or generic valves, supporting discrete
 * open/close/stop control or numeric position (`0..100` by default).
 *
 * See https://www.home-assistant.io/integrations/valve.mqtt/
 *
 * @par Example
 * @code
 * HaEntityValve main_water(ha_bridge, "Main Water Valve", "main", {
 *     .device_class = homeassistantentities::Valve::Water{},
 *     .with_stop = true,
 * });
 *
 * main_water.onCommand([&](HaEntityValve::Action action) {
 *   if (action == HaEntityValve::Action::Open) {
 *     main_water.publish(HaEntityValve::State::Open);
 *   } else if (action == HaEntityValve::Action::Close) {
 *     main_water.publish(HaEntityValve::State::Closed);
 *   } else if (action == HaEntityValve::Action::Stop) {
 *     main_water.publish(HaEntityValve::State::Stopped);
 *   }
 * });
 *
 * // Once MQTT is connected:
 * main_water.publishConfiguration();
 * main_water.publish(false); // Closed
 * @endcode
 */
class HaEntityValve : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class (`"water"`, `"gas"`, or `homeassistantentities::Valve::Water{}` /
     * `Valve::Gas{}`). Leave empty for a generic valve.
     * See https://www.home-assistant.io/integrations/valve/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Set to `true` if the valve reports and accepts numeric position (`0..100` by default) rather than binary
     * open/closed states.
     */
    bool reports_position = false;

    /**
     * @brief Position value representing a fully closed valve (default `0`). Used when `reports_position` is `true`.
     */
    uint8_t position_closed = 0;

    /**
     * @brief Position value representing a fully open valve (default `100`). Used when `reports_position` is `true`.
     */
    uint8_t position_open = 100;

    /**
     * @brief If `true` (default), configures `payload_stop: "STOP"` so Home Assistant exposes a Stop action.
     */
    bool with_stop = true;

    /**
     * @brief If `true`, this valve only reports its state/position and cannot be controlled from Home Assistant.
     */
    bool read_only = false;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:pipe-valve"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the valve works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing JSON attributes.
     */
    bool with_attributes = false;
  };

  inline static Configuration _default = {.device_class = "",
                                          .reports_position = false,
                                          .position_closed = 0,
                                          .position_open = 100,
                                          .with_stop = true,
                                          .read_only = false,
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt,
                                          .with_attributes = false};

  enum class State {
    Open,
    Opening,
    Closed,
    Closing,
    Stopped,
    Unknown,
  };

  enum class Action {
    Open,
    Close,
    Stop,
    Unknown,
  };

  /**
   * @brief Construct a new HaEntityValve object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Utility"` + name `"Water Valve"`
   * -> `"Utility Water Valve"`). Can be empty if a device is set and this is the main (or only) entity of the device.
   * See https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several
   * valves on the same node ID. Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this valve entity.
   */
  HaEntityValve(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish binary open/closed state (`true` -> `"open"` or `position_open`, `false` -> `"closed"` or
   * `position_closed`) regardless of whether it has changed. Also see `update()`.
   *
   * @param open `true` if open, `false` if closed.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(bool open, const Attributes::Map &attributes = {});

  /**
   * @brief Publish binary open/closed state only if it (or `attributes`) has changed. Also see `publish()`.
   *
   * @param open `true` if open, `false` if closed.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(bool open, const Attributes::Map &attributes = {});

  /**
   * @brief Publish discrete `State` and/or numeric position regardless of whether they have changed. Also see
   * `update()`.
   *
   * @param state Discrete valve state (`Opening`, `Closing`, `Stopped`, `Open`, `Closed`), or `std::nullopt`.
   * @param position Numeric position between `position_closed` and `position_open` when `reports_position` is `true`,
   * or `std::nullopt`.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(std::optional<State> state, std::optional<uint8_t> position = std::nullopt,
               const Attributes::Map &attributes = {});

  /**
   * @brief Publish discrete `State` and/or numeric position only if changed. Also see `publish()`.
   *
   * @param state Discrete valve state (`Opening`, `Closing`, `Stopped`, `Open`, `Closed`), or `std::nullopt`.
   * @param position Numeric position between `position_closed` and `position_open` when `reports_position` is `true`,
   * or `std::nullopt`.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(std::optional<State> state, std::optional<uint8_t> position = std::nullopt,
              const Attributes::Map &attributes = {});

  /**
   * @brief Publish numeric valve position regardless of whether it has changed (`Configuration::reports_position` must
   * be `true`). Also see `updatePosition()`.
   *
   * @param position Position clamped between `position_closed` and `position_open`.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publishPosition(uint8_t position, const Attributes::Map &attributes = {});

  /**
   * @brief Publish numeric valve position only if it (or `attributes`) has changed (`Configuration::reports_position`
   * must be `true`). Also see `publishPosition()`.
   *
   * @param position Position clamped between `position_closed` and `position_open`.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void updatePosition(uint8_t position, const Attributes::Map &attributes = {});

  /**
   * @brief Set callback for receiving discrete `Open`, `Close`, and `Stop` commands from Home Assistant.
   * Not subscribed if `Configuration::read_only` is `true`.
   *
   * @param callback Callback invoked with the requested `Action`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(Action)> callback);

  /**
   * @brief Set callback for receiving numeric position commands when `Configuration::reports_position` is `true` and
   * `Configuration::read_only` is `false`.
   *
   * @param callback Callback invoked with the target position clamped between `position_closed` and `position_open`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onPosition(std::function<void(uint8_t)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  struct Callbacks {
    std::function<void(Action)> on_action;
    std::function<void(uint8_t)> on_position;
    bool subscribed = false;
  };

  uint8_t clampPosition(uint8_t position) const;
  std::optional<std::string> encodePayload(std::optional<State> state, std::optional<uint8_t> position) const;
  bool ensureSubscribed();

private:
  Configuration _configuration;
  std::shared_ptr<Callbacks> _callbacks;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_VALVE_H__
