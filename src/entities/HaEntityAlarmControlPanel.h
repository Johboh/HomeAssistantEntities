// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_ALARM_CONTROL_PANEL_H__
#define __HA_ENTITY_ALARM_CONTROL_PANEL_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Alarm Control Panel entity (`alarm_control_panel.mqtt`).
 *
 * Supports arming (home, away, night, vacation, custom bypass), disarming, pending, arming, and triggered states,
 * with optional PIN/code requirements.
 *
 * See https://www.home-assistant.io/integrations/alarm_control_panel.mqtt/
 */
class HaEntityAlarmControlPanel : public HaEntity {
public:
  enum class State {
    Disarmed,
    ArmedHome,
    ArmedAway,
    ArmedNight,
    ArmedVacation,
    ArmedCustomBypass,
    Pending,
    Arming,
    Disarming,
    Triggered,
  };

  enum class Command {
    ArmHome,
    ArmAway,
    ArmNight,
    ArmVacation,
    ArmCustomBypass,
    Disarm,
    Unknown,
  };

  static std::string stateToString(State state);
  static std::optional<State> stringToState(const std::string &str);
  static std::string commandToString(Command command);
  static Command stringToCommand(const std::string &str);

  struct Configuration {
    /**
     * @brief Optional code / PIN required to arm or disarm. Leave empty if none.
     */
    std::string code = "";

    /**
     * @brief If `true`, a code is required to arm the alarm.
     */
    bool code_arm_required = false;

    /**
     * @brief If `true`, a code is required to disarm the alarm.
     */
    bool code_disarm_required = false;

    /**
     * @brief Format of the code: `"number"` (default) or `"text"`.
     */
    std::string code_format = "number";

    /**
     * @brief List of supported arming features: `"arm_home"`, `"arm_away"`, `"arm_night"`, `"arm_vacation"`,
     * `"arm_custom_bypass"`.
     */
    std::vector<std::string> supported_features = {"arm_home", "arm_away", "arm_night", "arm_vacation",
                                                   "arm_custom_bypass"};

    /**
     * @brief If `true`, configure `json_attributes_topic` for publishing custom JSON attributes.
     */
    bool with_attributes = false;

    /**
     * @brief If `true`, publishes messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:shield-lock"`).
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
      .code = "",
      .code_arm_required = false,
      .code_disarm_required = false,
      .code_format = "number",
      .supported_features = {"arm_home", "arm_away", "arm_night", "arm_vacation", "arm_custom_bypass"},
      .with_attributes = false,
      .retain = false,
      .icon = "",
      .entity_category = "",
      .enabled_by_default = std::nullopt};

  HaEntityAlarmControlPanel(HaBridge &ha_bridge, std::string name,
                            std::optional<std::string> child_object_id = std::nullopt,
                            Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the alarm state using typed `State`. Also see `update()`.
   */
  void publish(State state);

  /**
   * @brief Publish the alarm state string. Also see `update()`.
   */
  void publish(const std::string &state);

  /**
   * @brief Publish the alarm state only if it has changed. Also see `publish()`.
   */
  void update(State state);

  /**
   * @brief Publish the alarm state only if it has changed. Also see `publish()`.
   */
  void update(const std::string &state);

  /**
   * @brief Set callback for receiving arm/disarm commands with entered code/PIN.
   */
  bool onCommand(std::function<void(Command command, const std::string &code)> callback);

  /**
   * @brief Set callback for receiving arm/disarm commands (ignoring code).
   */
  bool onCommand(std::function<void(Command command)> callback);

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

  std::optional<std::string> _state;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_ALARM_CONTROL_PANEL_H__
