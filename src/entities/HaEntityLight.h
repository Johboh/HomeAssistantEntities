// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_LIGHT_H__
#define __HA_ENTITY_LIGHT_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represents an MQTT Light (`light.mqtt`) supporting bidirectional on/off control and optional brightness, color
 * temperature (mireds or Kelvin), RGB color, and lighting effects.
 *
 * See https://www.home-assistant.io/integrations/light.mqtt/
 *
 * @par Example
 * @code
 * HaEntityLight strip(ha_bridge, "LED Strip", "strip", {
 *     .with_brightness = true,
 *     .with_color_temperature = HaEntityLight::Configuration::ColorTemperature::Kelvin,
 *     .with_rgb_color = true,
 *     .effects = {"solid", "rainbow"},
 * });
 *
 * strip.onCommand([&](bool on) {
 *   setStripPower(on);
 *   strip.publish(on);
 * });
 * strip.onBrightness([&](uint8_t brightness) {
 *   setStripBrightness(brightness);
 *   strip.publishBrightness(brightness);
 * });
 * strip.onRgb([&](HaEntityLight::RGB rgb) {
 *   setStripRgb(rgb.r, rgb.g, rgb.b);
 *   strip.publishRgb(rgb);
 * });
 *
 * // Once MQTT is connected:
 * strip.publishConfiguration();
 * strip.publish(false);
 * strip.publishBrightness(255);
 * @endcode
 */
class HaEntityLight : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true`, the light supports brightness control (`0..255`).
     */
    bool with_brightness = false;

    enum class ColorTemperature {
      None,
      Mireds, // Color temperature in mireds (Home Assistant default range: 153..500 unless overridden).
      Kelvin, // Color temperature in Kelvin (Home Assistant default range: 2000..6535 unless overridden).
    };

    /**
     * @brief If set to `Mireds` or `Kelvin`, the light supports color temperature control.
     */
    ColorTemperature with_color_temperature = ColorTemperature::None;

    /**
     * @brief If `true`, the light supports RGB color control (`0..255` per channel).
     */
    bool with_rgb_color = false;

    /**
     * @brief If non-empty, list of lighting effect names supported by this light (order is preserved in Home Assistant
     * UI).
     */
    std::vector<std::string> effects = {};

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional minimum color temperature (in mireds when `ColorTemperature::Mireds`, or Kelvin when
     * `ColorTemperature::Kelvin`). Must be `> 0` if set.
     */
    std::optional<uint16_t> min_color_temperature = std::nullopt;

    /**
     * @brief Optional maximum color temperature (in mireds when `ColorTemperature::Mireds`, or Kelvin when
     * `ColorTemperature::Kelvin`). Must be `> 0` if set.
     */
    std::optional<uint16_t> max_color_temperature = std::nullopt;

    /**
     * @brief Optional custom icon (e.g. `"mdi:led-strip"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the light works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  inline static Configuration _default = {.with_brightness = false,
                                          .with_color_temperature = Configuration::ColorTemperature::None,
                                          .with_rgb_color = false,
                                          .effects = {},
                                          .retain = false,
                                          .min_color_temperature = std::nullopt,
                                          .max_color_temperature = std::nullopt,
                                          .icon = "",
                                          .entity_category = "",
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt};

  /**
   * @brief Construct a new HaEntityLight object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Bathroom"` + name `"Light"` ->
   * `"Bathroom Light"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several lights on the
   * same node ID. Pass `""` when there is only one light on the node.
   * @param configuration Capabilities and options supported by this light.
   */
  HaEntityLight(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current on/off state regardless of whether the value has changed. Also see `update()`.
   *
   * @param on `true` if the light is on, `false` if off.
   */
  void publish(bool on);

  /**
   * @brief Publish the current on/off state only if the value has changed. Also see `publish()`.
   *
   * @param on `true` if the light is on, `false` if off.
   */
  void update(bool on);

  /**
   * @brief Publish the current brightness regardless of whether the value has changed. Also see `updateBrightness()`.
   *
   * @param brightness Brightness value (`0..255`). Only published if `Configuration::with_brightness` is `true`.
   */
  void publishBrightness(uint8_t brightness);

  /**
   * @brief Publish the current brightness only if the value has changed. Also see `publishBrightness()`.
   *
   * @param brightness Brightness value (`0..255`). Only published if `Configuration::with_brightness` is `true`.
   */
  void updateBrightness(uint8_t brightness);

  /**
   * @brief Publish the current color temperature regardless of whether the value has changed. Also see
   * `updateColorTemperature()`.
   *
   * @param temperature Color temperature in mireds or Kelvin (clamped to configured bounds) according to
   * `Configuration::with_color_temperature`. Only published if color temperature is enabled in `Configuration`.
   */
  void publishColorTemperature(uint16_t temperature);

  /**
   * @brief Publish the current color temperature only if the value has changed. Also see `publishColorTemperature()`.
   *
   * @param temperature Color temperature in mireds or Kelvin (clamped to configured bounds) according to
   * `Configuration::with_color_temperature`. Only published if color temperature is enabled in `Configuration`.
   */
  void updateColorTemperature(uint16_t temperature);

  /**
   * @brief Publish the currently active effect regardless of whether the value has changed. Also see `updateEffect()`.
   *
   * @param effect Active effect name (should be one of `Configuration::effects`). Only published if `effects` is
   * non-empty in `Configuration`.
   */
  void publishEffect(const std::string &effect);

  /**
   * @brief Publish the currently active effect only if the value has changed. Also see `publishEffect()`.
   *
   * @param effect Active effect name (should be one of `Configuration::effects`). Only published if `effects` is
   * non-empty in `Configuration`.
   */
  void updateEffect(const std::string &effect);

  struct RGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    bool operator==(const RGB &other) const { return (r == other.r) && (g == other.g) && (b == other.b); }

    bool operator!=(const RGB &other) const { return !(*this == other); }
  };

  /**
   * @brief Publish the current RGB color regardless of whether the value has changed. Also see `updateRgb()`.
   *
   * @param r Red channel (`0..255`).
   * @param g Green channel (`0..255`).
   * @param b Blue channel (`0..255`).
   */
  void publishRgb(uint8_t r, uint8_t g, uint8_t b) { publishRgb({r, g, b}); }

  /**
   * @brief Publish the current RGB color only if the value has changed. Also see `publishRgb()`.
   *
   * @param r Red channel (`0..255`).
   * @param g Green channel (`0..255`).
   * @param b Blue channel (`0..255`).
   */
  void updateRgb(uint8_t r, uint8_t g, uint8_t b) { updateRgb({r, g, b}); }

  /**
   * @brief Publish the current RGB color regardless of whether the value has changed. Also see `updateRgb()`.
   *
   * @param rgb RGB color value. Only published if `Configuration::with_rgb_color` is `true`.
   */
  void publishRgb(RGB rgb);

  /**
   * @brief Publish the current RGB color only if the value has changed. Also see `publishRgb()`.
   *
   * @param rgb RGB color value. Only published if `Configuration::with_rgb_color` is `true`.
   */
  void updateRgb(RGB rgb);

  /**
   * @brief Set callback for receiving on/off commands from Home Assistant.
   *
   * @param on_state_callback Callback invoked with `true` for `"ON"` or `false` for `"OFF"`.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onCommand(std::function<void(bool)> on_state_callback);

  /**
   * @brief Set callback for receiving brightness commands (`0..255`) from Home Assistant.
   * Only subscribed if `Configuration::with_brightness` is `true`.
   *
   * @param brightness_callback Callback invoked with the requested brightness (`0..255`).
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onBrightness(std::function<void(uint8_t)> brightness_callback);

  /**
   * @brief Set callback for receiving color temperature commands from Home Assistant.
   * Only subscribed if `Configuration::with_color_temperature` is not `ColorTemperature::None`.
   *
   * @param color_temperature_callback Callback invoked with the requested color temperature (in mireds or Kelvin
   * according to `Configuration::with_color_temperature`).
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onColorTemperature(std::function<void(uint16_t)> color_temperature_callback);

  /**
   * @brief Set callback for receiving effect commands from Home Assistant.
   * Only subscribed if `Configuration::effects` is non-empty.
   *
   * @param effect_callback Callback invoked with the requested effect name.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onEffect(std::function<void(std::string)> effect_callback);

  /**
   * @brief Set callback for receiving RGB color commands from Home Assistant.
   * Only subscribed if `Configuration::with_rgb_color` is `true`.
   *
   * @param rgb_callback Callback invoked with the requested `RGB` value.
   * @return true if the subscription succeeded, false otherwise.
   */
  bool onRgb(std::function<void(RGB)> rgb_callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  uint16_t clampColorTemperature(uint16_t temperature) const;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<bool> _on;
  std::optional<RGB> _rgb;
  std::optional<std::string> _effect;
  std::optional<uint8_t> _brightness;
  std::optional<uint16_t> _color_temperature;
};

#endif // __HA_ENTITY_LIGHT_H__
