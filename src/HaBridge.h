// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_BRIDGE_H__
#define __HA_BRIDGE_H__

#include <HaEntity.h>
#include <HaUtilities.h>
#include <IJson.h>
#include <IMQTTRemote.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Bridge for MQTT and Home Assistant.
 * Used in composition with the actual HaEntity implementations. See also the HaEntity.h interface.
 * A single HaBridge instance is typically shared across all HaEntity instances on the same device/node.
 */
class HaBridge {
public:
  /**
   * @brief Origin metadata identifying the application/firmware generating MQTT discovery payloads.
   * Optional in single-component discovery; required by Home Assistant in device-based discovery
   * (where HaBridge provides a default if unset).
   */
  struct OriginInfo {
    std::string name;
    std::string sw_version = "";
    std::string support_url = "";
  };

  /**
   * @brief Metadata for a logical child device (sub-unit of a single physical product, such as an individual outlet on
   * a power strip or a relay channel).
   *
   * Per the Home Assistant MQTT specification, child devices only support `identifiers` (required, must differ from the
   * parent device's identifiers) and `name` (optional), and do not support `device_automation` triggers.
   * For separate physical devices routed through a gateway/hub, use `DeviceInfo::via_device` on a separate parent
   * `HaBridge` instead.
   */
  struct ChildDeviceInfo {
    std::vector<std::string> identifiers;
    std::string name = "";
  };

  /**
   * @brief Typed helper describing a Home Assistant MQTT `device` object, including `via_device` for gateway/hub
   * topologies and `[type, id]` connection pairs (e.g. `{"mac", "aa:bb:cc:dd:ee:ff"}`).
   * Use `HaBridge::populateDeviceJson(doc, info)` to populate an `IJsonDocument`, or see `HaDeviceInformation.h`
   * (`HaDeviceInformation::populateDeviceJson(...)` / `HaDeviceInformation::detectDeviceInfo(...)`) to auto-extract
   * firmware, chip/hardware revision, MAC, and local IP via ESP-IDF / Arduino APIs when available.
   */
  struct DeviceInfo {
    std::vector<std::string> identifiers = {};
    std::vector<std::pair<std::string, std::string>> connections = {};
    std::string name = "";
    std::string manufacturer = "";
    std::string model = "";
    std::string model_id = "";
    std::string sw_version = "";
    std::string hw_version = "";
    std::string serial_number = "";
    std::string via_device = "";
    std::string suggested_area = "";
    std::string configuration_url = "";
  };

  /**
   * @brief Populate an `IJsonDocument` with Home Assistant `device` metadata from a typed `DeviceInfo`.
   */
  static void populateDeviceJson(IJsonDocument &doc, const DeviceInfo &info);

  /**
   * @brief Construct a new HaBridge object for a device/node.
   *
   * @param remote MQTT interface / remote used to publish messages and subscribe to topics.
   * @param node_id Machine-readable root identifier (`[a-zA-Z0-9_-]`) used in MQTT state/command/attributes topics and
   * under `<discovery_prefix>/<component>/<node_id>/` for discovery payloads. For example, `"bathroom"` yields state
   * topic `"bathroom/sensor/temperature/state"` and discovery topic
   * `"homeassistant/sensor/bathroom/temperature/config"`.
   * @param this_device_json_doc JSON document describing this hardware device. Optional for single-component entities
   * (required for device triggers and device-based discovery). When populated, Home Assistant combines the device name
   * with each entity's name in the UI (see https://developers.home-assistant.io/docs/core/entity/#entity-naming) and
   * includes all keys under `"device"` in discovery payloads (including `"identifiers"`, `"connections"`, and
   * `"via_device"`). Pass an empty document to omit device metadata.
   * @param verbose `true` to enable extra debug logging.
   * @param availability_topic Custom availability topic builder. Defaults to `<mqtt_client_id>/status`.
   * @param unique_id Custom unique ID prefix builder. Each entity needs a stable unique ID in Home Assistant; defaults
   * to `<mqtt_client_id>_<node_id>`.
   */
  HaBridge(IMQTTRemote &remote, std::string node_id, IJsonDocument &this_device_json_doc, bool verbose = false,
           std::function<std::string(IMQTTRemote &remote)> availability_topic = {},
           std::function<std::string(IMQTTRemote &remote, std::string &node_id)> unique_id = {});

public:
  /**
   * @brief Create a child `HaBridge` representing a logical sub-device of this parent device (e.g. an individual outlet
   * on a smart power strip).
   *
   * Entities created with the returned bridge share the parent device's `this_device_json_doc` (even if populated later
   * in `setup()`), use `<node_id>_<child_node_id>` for distinct state/command/discovery topics and `unique_id`s, emit
   * `device.child_device` in single-component discovery, and group under `child_devices` in
   * `publishDeviceConfiguration(...)`.
   *
   * @param child_node_id Short machine-readable node suffix for topics/IDs (e.g. "outlet_1").
   * @param child_identifier Unique device identifier for this child device (must differ from parent `identifiers`).
   * @param child_name Optional human-readable name for the child device (e.g. "Outlet 1").
   */
  HaBridge childDevice(std::string child_node_id, std::string child_identifier, std::string child_name = "");

  /**
   * @brief Create a child `HaBridge` with multiple identifiers in `ChildDeviceInfo`.
   */
  HaBridge childDevice(std::string child_node_id, ChildDeviceInfo child_device_info);

  /**
   * @brief Configure or clear `child_device` metadata directly on this bridge.
   */
  void setChildDevice(ChildDeviceInfo child_device_info);
  void clearChildDevice() { _child_device.reset(); }
  const std::optional<ChildDeviceInfo> &childDeviceInfo() const { return _child_device; }

  /**
   * @brief Configure or clear `origin` metadata emitted in discovery payloads.
   */
  void setOrigin(OriginInfo origin);
  void clearOrigin() { _origin.reset(); }
  const std::optional<OriginInfo> &originInfo() const { return _origin; }

  /**
   * @brief Set the Home Assistant MQTT discovery prefix (defaults to "homeassistant").
   */
  void setDiscoveryPrefix(std::string discovery_prefix);
  const std::string &discoveryPrefix() const { return _discovery_prefix; }

  /**
   * @brief Customize the availability online/offline payloads (Home Assistant defaults to "online" and "offline").
   */
  void setAvailabilityPayloads(std::string payload_available, std::string payload_not_available);

  /**
   * @brief Subscribe to Home Assistant's birth message topic (`<discovery_prefix>/status`) and invoke `callback` when
   * `birth_payload` (default `"online"`) is received, e.g. to re-publish configurations and live states when Home
   * Assistant restarts while this device stays connected to the MQTT broker.
   *
   * Note: By default Home Assistant publishes its birth message with `retain: false`, so if Home Assistant is already
   * running when this device boots or reconnects to MQTT, no birth message is delivered. Register
   * `onHomeAssistantBirth` once during setup alongside publishing on MQTT connection so both device (re)connects and
   * Home Assistant restarts are handled. Multiple `HaBridge` instances sharing the same `IMQTTRemote` can each register
   * their own `onHomeAssistantBirth` callback.
   */
  bool onHomeAssistantBirth(std::function<void()> callback, std::string birth_payload = "online");

  /**
   * @brief Call to publish the configuration for the HaEntity (single-component discovery).
   * This function will set the "availability_topic" and the "unique_id" field (except for "device_automation"
   * components where Home Assistant forbids them), as well as add "device" (and "child_device" / "origin" if set).
   *
   * The component and the object_id will create a topic like this (node_id is from the constructor of the HaBridge):
   * `<discovery_prefix>/<component>/<node_id>/<object_id>/config`
   *
   * For more information, see https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery
   */
  void publishConfiguration(const std::string &component, const std::string &object_id,
                            const std::string &child_object_id, const IJsonDocument &specific_doc);

  /**
   * @brief Remove a single-component discovery configuration by publishing an empty retained payload (`""`).
   */
  bool unpublishConfiguration(const std::string &component, const std::string &object_id,
                              const std::string &child_object_id = "");

  /**
   * @brief Publish a single unified device-based discovery payload to `<discovery_prefix>/device/<node_id>/config`
   * containing `device`, `origin`, `availability_topic`, `components`, and (for any entities attached to a
   * `childDevice(...)` bridge) `child_devices`.
   *
   * Note: Home Assistant requires `this_device_json_doc` to have at least one entry in `identifiers` or `connections`.
   *
   * @param entities Entities belonging to this parent bridge and/or its `childDevice(...)` bridges.
   * @return true if the device discovery payload was valid and published.
   */
  bool publishDeviceConfiguration(const std::vector<std::reference_wrapper<HaEntity>> &entities);

  /**
   * @brief Remove a device-based discovery configuration by publishing an empty retained payload (`""`) to
   * `<discovery_prefix>/device/<node_id>/config`.
   */
  bool unpublishDeviceConfiguration();

  /**
   * @brief Migrate entities from single-component discovery to unified device-based discovery per the Home Assistant
   * migration flow:
   * 1. Publishes retained `{"migrate_discovery": true}` to each entity's single-component `/config` topic.
   * 2. Publishes the unified device-based discovery payload to `<discovery_prefix>/device/<node_id>/config`.
   * 3. Clears each old single-component `/config` topic by publishing an empty retained payload (`""`).
   */
  bool migrateToDeviceConfiguration(const std::vector<std::reference_wrapper<HaEntity>> &entities);

  /**
   * @brief Publish a message to MQTT.
   *
   * @param topic MQTT topic to publish to.
   * @param message Payload string to publish.
   * @param retain `true` to set the MQTT retain flag on the message.
   * @return true on success, false on failure.
   */
  bool publishMessage(const std::string &topic, const std::string &message, bool retain = false);

  enum class TopicType {
    State,      // Topic where the entity publishes its state.
    Command,    // Topic where the entity listens for commands/actions from Home Assistant.
    Attributes, // Topic where the entity publishes JSON attributes.
  };

  /**
   * @brief Get the topic to use for publishing state, subscribing to commands, and configuring Home Assistant
   * (including manual Home Assistant MQTT Subentry configuration).
   */
  std::string getTopic(TopicType topic_type, const std::string &component, const std::string &object_id,
                       const std::string &child_object_id = "");

  /**
   * @brief Get the single-component discovery `/config` topic for a component.
   */
  std::string getDiscoveryTopic(const std::string &component, const std::string &object_id,
                                const std::string &child_object_id = "") const;

  /**
   * @brief Get the unified device-based discovery `/config` topic (`<discovery_prefix>/device/<node_id>/config`).
   */
  std::string getDeviceDiscoveryTopic() const;

  /**
   * @brief Get the resolved availability topic for this bridge.
   */
  std::string getAvailabilityTopic() const;

  /**
   * @brief Get the node ID for this bridge.
   */
  const std::string &nodeId() const { return _node_id; }

  /**
   * @brief Access the underlying `IMQTTRemote` (typically only needed for custom topic subscriptions; prefer
   * `publishConfiguration()` and `publishMessage()` for publishing).
   */
  IMQTTRemote &remote() { return _remote; }

private:
  std::string topicType(TopicType topic_type) const;
  std::string buildUniqueId(const std::string &object_id, const std::string &child_object_id);
  std::vector<std::string> sanitizedChildIdentifiers(const std::vector<std::string> &parent_identifiers) const;

private:
  bool _verbose;
  std::string _node_id;
  std::string _child_node_id;
  std::string _discovery_prefix = "homeassistant";
  IMQTTRemote &_remote;
  IJsonDocument &_this_device_json_doc;
  std::function<std::string(IMQTTRemote &)> _availability_topic;
  std::function<std::string(IMQTTRemote &, std::string &)> _unique_id;
  std::optional<std::string> _payload_available;
  std::optional<std::string> _payload_not_available;
  std::optional<OriginInfo> _origin;
  std::optional<ChildDeviceInfo> _child_device;
  std::shared_ptr<void> _birth_registration;
};

#endif // __HA_BRIDGE_H__
