// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_DEVICE_TRIGGER_H__
#define __HA_ENTITY_DEVICE_TRIGGER_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <optional>
#include <string>

/**
 * @brief Represents an MQTT Device Trigger (`device_trigger.mqtt`) for firing stateless automation triggers in Home
 * Assistant.
 *
 * Note: Home Assistant requires device information (`identifiers` or `connections`) to be populated on the `HaBridge`
 * when using device triggers.
 * See https://www.home-assistant.io/integrations/device_trigger.mqtt/
 *
 * @par Example
 * @code
 * // Note: `ha_bridge` must be constructed with a populated device JSON document.
 * HaEntityDeviceTrigger short_press(ha_bridge, "btn1_short_press", {
 *     .type = "button_short_press",
 *     .subtype = "button_1",
 * });
 *
 * // Once MQTT is connected:
 * short_press.publishConfiguration();
 *
 * // When physical button 1 is pressed:
 * short_press.publish();
 * @endcode
 */
class HaEntityDeviceTrigger : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The type of the trigger (e.g. `"button_short_press"`).
     *
     * Standard types supported by the Home Assistant frontend: `"button_short_press"`, `"button_short_release"`,
     * `"button_long_press"`, `"button_long_release"`, `"button_double_press"`, `"button_triple_press"`,
     * `"button_quadruple_press"`, `"button_quintuple_press"`.
     * Custom values are also accepted and rendered as `<subtype> <type>` in the UI.
     */
    std::string type;

    /**
     * @brief The subtype of the trigger (e.g. `"button_1"`).
     *
     * Standard subtypes supported by the Home Assistant frontend: `"turn_on"`, `"turn_off"`, `"button_1"`,
     * `"button_2"`, `"button_3"`, `"button_4"`, `"button_5"`, `"button_6"`.
     * Custom values are also accepted (e.g. `"left_button"`).
     *
     * By default, `subtype` is also used as the MQTT payload unless `payload` is explicitly set.
     */
    std::string subtype;

    /**
     * @brief Optional custom payload to match and publish for the trigger event.
     * If unset (`std::nullopt`), `subtype` is used as the payload. If set to an empty string (`""`), no payload filter
     * is configured in Home Assistant (any payload on the trigger topic fires the trigger).
     */
    std::optional<std::string> payload = std::nullopt;
  };

  /**
   * @brief Construct a new HaEntityDeviceTrigger object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant (must have device information configured).
   * @param object_id Unique machine-readable object ID (`[a-zA-Z0-9_-]`) for this device trigger on this node/device
   * (e.g. `"btn1_short_press"`).
   * @param configuration Configuration for this device trigger.
   */
  HaEntityDeviceTrigger(HaBridge &ha_bridge, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the trigger event to Home Assistant (non-retained).
   */
  void publish();

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_DEVICE_TRIGGER_H__