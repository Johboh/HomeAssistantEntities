// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_DEVICE_TRIGGER_H__
#define __HA_ENTITY_DEVICE_TRIGGER_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <optional>
#include <string>

/* clang-format off */
/**
 * @brief Represents an MQTT Device Trigger (`device_trigger.mqtt`) for firing stateless automation triggers in Home
 * Assistant.
 *
 * Note: Home Assistant requires device information (`identifiers` or `connections`) to be populated on the `HaBridge`
 * when using device triggers.
 * See https://www.home-assistant.io/integrations/device_trigger.mqtt/
 *
 * @section trigger_vs_event Device Trigger vs. Event Entity
 * Both `HaEntityDeviceTrigger` and `HaEntityEvent` can respond to momentary occurrences (such as button presses,
 * RF remotes, or sensor triggers), but they differ fundamentally in how Home Assistant exposes them:
 *
 * | Feature | HaEntityDeviceTrigger (`device_trigger.mqtt`) | HaEntityEvent (`event.mqtt`) |
 * | :--- | :--- | :--- |
 * | **Entity Registry** | **No** — does not create an `entity_id` | **Yes** — registered as `event.<name>` |
 * | **UI & Dashboards** | Not visible on dashboards or in entity lists | Visible on Lovelace dashboards with last event & timestamp |
 * | **History & Logbook** | Not recorded in history or logbook | Recorded in Home Assistant history and logbook |
 * | **State & Attributes** | Stateless (fires instantaneous trigger only) | State is ISO timestamp of last event + holds `event_type` & custom attributes |
 * | **Payloads** | Simple string matching `subtype` or empty payload | Non-retained JSON with `event_type` and arbitrary attributes |
 * | **Automation Usage** | Exposed directly in Home Assistant UI automation editor under "Device -> Trigger" | Triggered via standard entity state/attribute automations (`platform: state`) |
 * | **Device Dependency** | **Mandatory** (`ha_bridge` must have device info configured) | Optional (recommended) |
 * | **Multiple Actions** | Separate `HaEntityDeviceTrigger` instance required for each type/subtype | Single entity handles an array of `event_types` |
 *
 * @section when_to_use_device_trigger When to use HaEntityDeviceTrigger
 * - **Device Automation UI**: You want triggers to show up directly in Home Assistant's visual automation editor
 *   under Device Triggers (e.g., "When [Device Name] Button 1 short press...").
 * - **No Entity Clutter**: You specifically do not want extra entities in the entity registry, dashboard states,
 *   or logbook for high-frequency or purely automated actions.
 * - **Simplicity**: You only need to trigger automations without transmitting contextual event attributes or storing timestamps.
 *
 * @section when_to_use_event_entity_instead When to use HaEntityEvent instead
 * - Choose `HaEntityEvent` if you want occurrences displayed on dashboards, recorded in the logbook and history,
 *   need to carry extra attributes (such as duration, tag ID, or payload data), or want a single entity to handle
 *   multiple related event types (`single_press`, `double_press`, `long_press`).
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
/* clang-format on */
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