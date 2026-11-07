// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_EVENT_H__
#define __HA_ENTITY_EVENT_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>

/* clang-format off */
/**
 * @brief Represents an MQTT Event entity (`event.mqtt`) that emits discrete typed events to Home Assistant.
 *
 * See https://www.home-assistant.io/integrations/event.mqtt/
 *
 * @section event_vs_trigger Event Entity vs. Device Trigger
 * While both `HaEntityEvent` and `HaEntityDeviceTrigger` handle momentary occurrences (such as button presses,
 * doorbell rings, or motion pulses), they serve distinct purposes in Home Assistant:
 *
 * | Feature | HaEntityEvent (`event.mqtt`) | HaEntityDeviceTrigger (`device_trigger.mqtt`) |
 * | :--- | :--- | :--- |
 * | **Entity Registry** | **Yes** — registered as a full entity (`event.<name>`) | **No** — no entity ID exists in Home Assistant |
 * | **UI & Dashboards** | Visible on Lovelace dashboards with last event & timestamp | Not visible on dashboards or in state tables |
 * | **History & Logbook** | Recorded in Home Assistant history and logbook entries | Not recorded in history or logbook |
 * | **State & Attributes** | State is the timestamp of last event (ISO 8601); holds `event_type` + custom attributes | Stateless (no state or attributes stored) |
 * | **Payloads** | Non-retained JSON containing `event_type` and optional extra metadata | Simple string or empty match payload on a trigger topic |
 * | **Automation Usage** | Trigger via standard state triggers (`platform: state`) or attribute triggers on `event_type` | Trigger via Device Automations (`platform: device`) in the UI automation builder |
 * | **Device Dependency** | Optional (recommended) | **Mandatory** (`ha_bridge` must have device info configured) |
 * | **Multiple Event Types**| Single entity supports multiple types (e.g. `single_press`, `double_press`, `long_press`) | Requires a separate `HaEntityDeviceTrigger` instance per type/subtype combination |
 *
 * @section when_to_use_event_entity When to use HaEntityEvent
 * - **Dashboards and UI**: You want users to see when the event occurred and what event type fired on their cards.
 * - **History and Auditing**: You want occurrences logged in Home Assistant's Logbook and History timeline (e.g. doorbell presses or RFID scans).
 * - **Rich Metadata**: You need to send contextual data alongside the event (e.g. `duration_ms`, `battery_level`, or `user_id`).
 * - **Consolidated Controls**: You want a single entity representing a multi-function button or controller rather than multiple disparate triggers.
 * - **Modern HA Integrations**: Event entities are Home Assistant's modern standard (HA 2023.7+) for transient, stateless signals.
 *
 * @section when_to_use_device_trigger_instead When to use HaEntityDeviceTrigger instead
 * - Choose `HaEntityDeviceTrigger` when you only want simple, stateless automation hooks in Home Assistant's UI automation editor
 *   under "Device -> Trigger" dropdowns, without creating entity IDs or cluttering the user's dashboard and logbook.
 *
 * @par Example
 * @code
 * HaEntityEvent doorbell(ha_bridge, "Doorbell", "doorbell_event", {
 *     .event_types = {"single_press", "double_press", "long_press"},
 *     .device_class = HaEntityEvent::DeviceClass::Doorbell,
 * });
 *
 * // Once MQTT is connected:
 * doorbell.publishConfiguration();
 *
 * // When the doorbell button is pressed:
 * doorbell.publish("single_press", {{"duration_ms", 120U}});
 * @endcode
 */
/* clang-format on */
class HaEntityEvent : public HaEntity {
public:
  enum class DeviceClass {
    None,
    Button,
    Motion,
    Doorbell,
  };

  struct Configuration {
    /**
     * @brief List of valid `event_type` strings supported by this entity (must be non-empty).
     */
    std::vector<std::string> event_types = {};

    /**
     * @brief Device class for this event (`None` to omit, `Button`, `Motion`, or `Doorbell`).
     */
    DeviceClass device_class = DeviceClass::None;

    /**
     * @brief Optional custom icon (e.g. `"mdi:bell-ring"`).
     */
    std::string icon = "";

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::string entity_category = "";

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  /**
   * @brief Construct a new HaEntityEvent object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Front Door"` + name `"Doorbell"`
   * -> `"Front Door Doorbell"`). Can be empty if a device is set and this is the main (or only) entity of the device.
   * See https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param object_id Unique machine-readable object ID (`[a-zA-Z0-9_-]`) for this event on this node/device (e.g.
   * `"doorbell_event"`).
   * @param configuration Configuration for this event entity (`event_types` must be non-empty).
   */
  HaEntityEvent(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish an event occurrence to Home Assistant (non-retained JSON payload containing `event_type` and any
   * extra attributes).
   *
   * @param event Event type string (must be one of the entries in `Configuration::event_types`).
   * @param attributes Optional extra event attributes merged into the event JSON payload.
   */
  void publish(const std::string &event, const Attributes::Map &attributes = {});

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_EVENT_H__
