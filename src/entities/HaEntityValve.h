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
 * @brief Represent a Valve actuator (water/gas/generic) supporting binary open/close/stop or position control.
 * See more at https://www.home-assistant.io/integrations/valve.mqtt/
 */
class HaEntityValve : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief Optional device class ("water", "gas", or homeassistantentities::Valve::Water{} / Valve::Gas{}).
     * See https://www.home-assistant.io/integrations/valve/#device-class
     */
    std::string device_class = "";

    /**
     * @brief Set to true if the valve reports and accepts numeric position (0-100 by default) rather than binary
     * open/closed states.
     */
    bool reports_position = false;

    /**
     * @brief Position value representing a fully closed valve (default 0). Used when reports_position is true.
     */
    uint8_t position_closed = 0;

    /**
     * @brief Position value representing a fully open valve (default 100). Used when reports_position is true.
     */
    uint8_t position_open = 100;

    /**
     * @brief If true, configures `payload_stop: "STOP"` so Home Assistant exposes a Stop action.
     */
    bool with_stop = true;

    /**
     * @brief If true, this valve only reports its state/position and cannot be controlled from Home Assistant.
     */
    bool read_only = false;

    /**
     * @brief If true, tells Home Assistant to publish command messages with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. "mdi:pipe-valve").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag that defines if valve works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If true, configure `json_attributes_topic` for publishing JSON attributes.
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
   * @brief Construct a new Ha Entity Valve object.
   */
  HaEntityValve(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish binary open/closed state (`true` -> open / position_open, `false` -> closed / position_closed).
   */
  void publish(bool open, const Attributes::Map &attributes = {});

  /**
   * @brief Publish binary open/closed state only if changed.
   */
  void update(bool open, const Attributes::Map &attributes = {});

  /**
   * @brief Publish discrete state and/or numeric position.
   */
  void publish(std::optional<State> state, std::optional<uint8_t> position = std::nullopt,
               const Attributes::Map &attributes = {});

  /**
   * @brief Publish discrete state and/or numeric position only if changed.
   */
  void update(std::optional<State> state, std::optional<uint8_t> position = std::nullopt,
              const Attributes::Map &attributes = {});

  /**
   * @brief Publish numeric position (`reports_position` must be true).
   */
  void publishPosition(uint8_t position, const Attributes::Map &attributes = {});

  /**
   * @brief Publish numeric position only if changed (`reports_position` must be true).
   */
  void updatePosition(uint8_t position, const Attributes::Map &attributes = {});

  /**
   * @brief Set callback for receiving Open / Close / Stop commands from Home Assistant.
   */
  bool onCommand(std::function<void(Action)> callback);

  /**
   * @brief Set callback for receiving numeric position commands when `reports_position` is true.
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
