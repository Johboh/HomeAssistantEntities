// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_FAN_H__
#define __HA_ENTITY_FAN_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Fan (`fan.mqtt`) supporting on/off control and optional speed, airflow direction,
 * oscillation, and preset modes.
 *
 * See https://www.home-assistant.io/integrations/fan.mqtt/
 *
 * @par Example
 * @code
 * HaEntityFan ceiling_fan(ha_bridge, "Ceiling Fan", "ceiling", {
 *     .with_direction = true,
 *     .with_speed = true,
 *     .speed_range_min = 1,
 *     .speed_range_max = 3,
 *     .presets = {"eco", "boost"},
 * });
 *
 * ceiling_fan.onCommand([&](bool on) {
 *   setFanPower(on);
 *   ceiling_fan.publish(on);
 * });
 * ceiling_fan.onSpeed([&](uint32_t speed) {
 *   setFanSpeed(speed);
 *   ceiling_fan.publishSpeed(speed);
 * });
 * ceiling_fan.onPreset([&](std::string preset) {
 *   ceiling_fan.publishPreset(preset);
 * });
 *
 * // Once MQTT is connected:
 * ceiling_fan.publishConfiguration();
 * ceiling_fan.publish(false);
 * ceiling_fan.publishSpeed(0);
 * ceiling_fan.publishDirection(HaEntityFan::Direction::Forward);
 * @endcode
 */
class HaEntityFan : public HaEntity {
public:
  struct Configuration {

    /**
     * @brief If `true`, this fan supports changing airflow direction (`"forward"` / `"reverse"`).
     */
    bool with_direction = false;

    /**
     * @brief If `true`, this fan supports oscillation (sweeping back and forth).
     */
    bool with_oscillation = false;

    /**
     * @brief If `true`, this fan supports numeric speed control in addition to on/off control.
     */
    bool with_speed = false;

    /**
     * @brief When `with_speed` is `true`, the minimum active speed value (`speed_range_min - 1` represents `0%` / off).
     * Home Assistant computes `percentage_step` as `100 / (speed_range_max - speed_range_min + 1)`.
     */
    uint32_t speed_range_min = 1;

    /**
     * @brief When `with_speed` is `true`, the maximum speed value (representing `100%`).
     */
    uint32_t speed_range_max = 100;

    /**
     * @brief If non-empty, list of preset mode names supported by this fan (order is preserved in Home Assistant UI).
     */
    std::vector<std::string> presets = {};

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional custom icon (e.g. `"mdi:fan"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the fan works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_direction = false,
                                          .with_oscillation = false,
                                          .with_speed = false,
                                          .speed_range_min = 1,
                                          .speed_range_max = 100,
                                          .presets = {},
                                          .retain = false,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityFan object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Fan"` ->
   * `"Bathroom Fan"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several fans on the
   * same node ID. Pass `""` when there is only one fan on the node.
   * @param configuration Configuration for this fan.
   */
  HaEntityFan(HaBridge &ha_bridge, std::string name, std::string child_object_id,
              Configuration configuration = _default);

public:
  enum class Direction {
    Forward,
    Reverse,
  };

  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the airflow direction regardless of whether the value has changed (`Configuration::with_direction`
   * must be `true`). Also see `updateDirection()`.
   *
   * @param direction Direction string (`"forward"` or `"reverse"`).
   */
  void publishDirection(const std::string &direction);

  /**
   * @brief Publish the airflow direction regardless of whether the value has changed (`Configuration::with_direction`
   * must be `true`). Also see `updateDirection()`.
   *
   * @param direction Typed `Direction::Forward` or `Direction::Reverse`.
   */
  void publishDirection(Direction direction) {
    publishDirection(direction == Direction::Forward ? "forward" : "reverse");
  }

  /**
   * @brief Publish the airflow direction only if the value has changed (`Configuration::with_direction` must be
   * `true`). Also see `publishDirection()`.
   *
   * @param direction Direction string (`"forward"` or `"reverse"`).
   */
  void updateDirection(const std::string &direction);

  /**
   * @brief Publish the airflow direction only if the value has changed (`Configuration::with_direction` must be
   * `true`). Also see `publishDirection()`.
   *
   * @param direction Typed `Direction::Forward` or `Direction::Reverse`.
   */
  void updateDirection(Direction direction) {
    updateDirection(direction == Direction::Forward ? "forward" : "reverse");
  }

  /**
   * @brief Set callback for receiving direction commands (`"forward"` or `"reverse"`) from Home Assistant.
   * Requires `Configuration::with_direction` to be `true`.
   *
   * @param callback Callback invoked with the requested direction string.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onDirection(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the oscillation state regardless of whether the value has changed
   * (`Configuration::with_oscillation` must be `true`). Also see `updateOscillation()`.
   *
   * @param oscillation `true` if oscillating, `false` otherwise.
   */
  void publishOscillation(bool oscillation);

  /**
   * @brief Publish the oscillation state only if the value has changed (`Configuration::with_oscillation` must be
   * `true`). Also see `publishOscillation()`.
   *
   * @param oscillation `true` if oscillating, `false` otherwise.
   */
  void updateOscillation(bool oscillation);

  /**
   * @brief Set callback for receiving oscillation commands from Home Assistant.
   * Requires `Configuration::with_oscillation` to be `true`.
   *
   * @param callback Callback invoked with `true` to enable oscillation or `false` to disable.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onOscillation(std::function<void(bool)> callback);

  //--------------------------------------

  /**
   * @brief Publish the current speed regardless of whether the value has changed (`Configuration::with_speed` must be
   * `true`). Also see `updateSpeed()`.
   *
   * @param speed Speed value clamped between `speed_range_min - 1` (`0%` / off) and `speed_range_max` (`100%`).
   */
  void publishSpeed(uint32_t speed);

  /**
   * @brief Publish the current speed only if the value has changed (`Configuration::with_speed` must be `true`). Also
   * see `publishSpeed()`.
   *
   * @param speed Speed value clamped between `speed_range_min - 1` (`0%` / off) and `speed_range_max` (`100%`).
   */
  void updateSpeed(uint32_t speed);

  /**
   * @brief Set callback for receiving speed commands from Home Assistant.
   * Requires `Configuration::with_speed` to be `true`.
   *
   * @param callback Callback invoked with the requested speed between `speed_range_min - 1` (`0%`) and
   * `speed_range_max` (`100%`).
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onSpeed(std::function<void(uint32_t)> callback);

  //--------------------------------------

  /**
   * @brief Publish the active preset mode regardless of whether the value has changed (`Configuration::presets` must be
   * non-empty). Also see `updatePreset()`.
   *
   * @param preset Active preset name (should be one of `Configuration::presets` or `"None"`).
   */
  void publishPreset(const std::string &preset);

  /**
   * @brief Publish the active preset mode only if the value has changed (`Configuration::presets` must be non-empty).
   * Also see `publishPreset()`.
   *
   * @param preset Active preset name (should be one of `Configuration::presets` or `"None"`).
   */
  void updatePreset(const std::string &preset);

  /**
   * @brief Set callback for receiving preset mode commands from Home Assistant.
   * Requires `Configuration::presets` to be non-empty.
   *
   * @param callback Callback invoked with the requested preset name.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onPreset(std::function<void(std::string)> callback);

  //--------------------------------------

  /**
   * @brief Publish the fan on/off state regardless of whether the value has changed. Also see `update()`.
   *
   * @param on `true` if the fan is on, `false` if off.
   */
  void publish(bool on);

  /**
   * @brief Publish the fan on/off state only if the value has changed. Also see `publish()`.
   *
   * @param on `true` if the fan is on, `false` if off.
   */
  void update(bool on);

  /**
   * @brief Set callback for receiving on/off commands from Home Assistant.
   *
   * @param callback Callback invoked with `true` for `"ON"` or `false` for `"OFF"`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(bool)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  uint32_t clampSpeed(uint32_t speed) const;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _on;
  std::optional<uint32_t> _speed;
  std::optional<bool> _oscillation;
  std::optional<std::string> _preset;
  std::optional<std::string> _direction;
};

#endif // __HA_ENTITY_FAN_H__
