// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_H__
#define __HA_ENTITY_H__

#include <IJson.h>
#include <string>

class HaBridge;

/**
 * @brief Abstract base class for all Home Assistant MQTT entities.
 *
 * An entity represents a sensor, binary sensor, actuator, or event source exposed to Home Assistant over MQTT
 * discovery (https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery).
 * - For **sensors**, Home Assistant subscribes to the entity's state topic to display and record telemetry.
 * - For **actuators**, Home Assistant publishes commands to command topics and optionally tracks the entity's state.
 *
 * Each `HaEntity` provides typed methods to publish/update state values, register callbacks for incoming commands,
 * and publish discovery configuration to Home Assistant.
 *
 * ## Per-Entity Discovery vs. Unified Device Discovery
 * This library supports two Home Assistant discovery workflows:
 *
 * ### 1. Per-Entity Discovery (Classic, Default)
 * Each entity publishes its own independent MQTT discovery message by calling `entity.publishConfiguration()`:
 * - **Discovery Topic**: `<discovery_prefix>/<component>/<node_id>/<object_id>[_<child_id>]/config`
 * - **Pros**:
 *   - **Low Memory & Packet Size**: Each MQTT message is small (typically 300–800 bytes), making it ideal for
 * memory-constrained microcontrollers (such as ESP8266 or devices with small MQTT packet buffers like 512–1024 bytes).
 *   - **Independent Lifecycle**: Entities can be initialized, published, updated, or unpublished
 * (`entity.unpublishConfiguration()`) individually at runtime.
 *   - **Universal Compatibility**: Works on all Home Assistant releases supporting MQTT discovery.
 * - **Cons**:
 *   - $N$ separate retained MQTT discovery messages on the broker for $N$ entities.
 *   - Device metadata (`device` dictionary) is duplicated across every entity's discovery payload.
 *
 * ### 2. Unified Device-Based Discovery
 * All entities belonging to a device are bundled into a single atomic MQTT discovery payload by calling
 * `ha_bridge.publishDeviceConfiguration({entity1, entity2, ...})`:
 * - **Discovery Topic**: `<discovery_prefix>/device/<node_id>/config`
 * - **Pros**:
 *   - **Single Atomic Message**: One retained MQTT message registers or updates the device and all its entities at
 * once.
 *   - **Clean Broker State**: Only one discovery topic per physical device instead of $N$.
 *   - **Atomic Device Removal**: Calling `ha_bridge.unpublishDeviceConfiguration()` removes the entire device and all
 * its entities.
 *   - **Multi-Device / Child Device Support**: Seamlessly supports `child_devices` on multi-channel hardware (e.g.
 * smart strips).
 * - **Cons**:
 *   - **Larger Packet Size**: The JSON payload bundles all components together and can reach 2–8 KB. The MQTT client
 * buffer
 *     (`max_message_size` or `MQTT_MAX_PACKET_SIZE`) must be sized accordingly.
 *   - **Home Assistant Version**: Requires Home Assistant 2024.2 or newer.
 *   - **Migration Requirement**: Existing deployments using per-entity discovery must be migrated using
 *     `ha_bridge.migrateToDeviceConfiguration(entities)` to prevent duplicate entities in Home Assistant.
 *
 * @par Example 1: Per-Entity Discovery (No Device Bundle)
 * See also `examples/arduino/sensors/sensors.ino` and `examples/espidf/sensors/main/main.cpp`:
 * @code
 * HaBridge ha_bridge(mqtt_client, "living_room", device_doc);
 * HaEntityTemperature temperature(ha_bridge, "Temperature");
 * HaEntitySwitch relay(ha_bridge, "Relay", "main");
 *
 * relay.onCommand([&](bool on) {
 *   setRelayHardware(on);
 *   relay.publish(on);
 * });
 *
 * // Once MQTT connects (and inside onHomeAssistantBirth):
 * temperature.publishConfiguration();
 * relay.publishConfiguration();
 * temperature.publish(21.5);
 * relay.publish(false);
 * @endcode
 *
 * @par Example 2: Unified Device-Based Discovery
 * See also `examples/arduino/child_devices/child_devices.ino` and `examples/espidf/child_devices/main/main.cpp`:
 * @code
 * HaBridge ha_bridge(mqtt_client, "living_room", device_doc);
 * HaEntityTemperature temperature(ha_bridge, "Temperature");
 * HaEntitySwitch relay(ha_bridge, "Relay", "main");
 *
 * std::vector<std::reference_wrapper<HaEntity>> all_entities = {temperature, relay};
 *
 * // Once MQTT connects (and inside onHomeAssistantBirth):
 * ha_bridge.publishDeviceConfiguration(all_entities);
 * temperature.publish(21.5);
 * relay.publish(false);
 * @endcode
 *
 * @par Example 3: Safe Migration from Per-Entity to Device-Based Discovery
 * @code
 * // Upgrading existing deployment without creating duplicate entities or losing entity IDs:
 * ha_bridge.migrateToDeviceConfiguration(all_entities);
 * @endcode
 */
class HaEntity {
public:
  virtual ~HaEntity() = default;

  /**
   * @brief Publish the Home Assistant MQTT discovery configuration for this entity.
   */
  virtual void publishConfiguration() = 0;

  /**
   * @brief Remove the Home Assistant discovery configuration by publishing an empty retained payload to the discovery
   * topic.
   */
  virtual void unpublishConfiguration() {}

  /**
   * @brief Republish the most recently published state (and attributes, if any) to MQTT.
   */
  virtual void republishState() = 0;

protected:
  friend class HaBridge;

  /**
   * @brief Internal descriptor for an entity's MQTT discovery component payload (used for device-based discovery and
   * migration).
   */
  struct ComponentConfig {
    std::string component;
    std::string object_id;
    std::string child_object_id;
    IJsonDocument doc;
    HaBridge *bridge = nullptr;
  };

  /**
   * @brief Populate this entity's component discovery descriptor.
   * @return true if a valid component configuration was written to @p out.
   */
  virtual bool buildComponentConfiguration(ComponentConfig &out) {
    (void)out;
    return false;
  }

  /**
   * @brief Helper allowing wrapper entities to delegate `buildComponentConfiguration` to an inner `HaEntity` member.
   */
  static bool buildChildComponentConfiguration(HaEntity &child, ComponentConfig &out) {
    return child.buildComponentConfiguration(out);
  }
};

#endif // __HA_ENTITY_H__