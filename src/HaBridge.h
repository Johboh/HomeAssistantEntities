#ifndef __HA_BRIDGE_H__
#define __HA_BRIDGE_H__

#include <IJson.h>
#include <IMQTTRemote.h>
#include <cstdint>
#include <string>

/**
 * @brief Bridge for MQTT and Home Assistant.
 * Used in composition with the actual HaEntity implementations. see also the HaEntity.h interface.
 * Each HaEntity need their own unique instance of HaBridge.
 */
class HaBridge {
public:
  /**
   * @brief Construct a new Ha Entity object with a device.
   *
   * @param remote the MQTT Interface / Remote to send messages and subscribe to topics.
   * @param node_id this is used as the root in the MQTT path for publishing values to a state topic, or for
   * subscribing to a state. This is also the path key used under the MQTT
   * "homeassistant/<component>/<node_id>" path for publishing the configuration. Example for a state topic for
   * a temperature for bathroom: "bathroom" which will lead to state topic "bathroom/sensor/temperature/state", and the
   * Home Assistant Configuration will be published under "homeassistant/sensor/bathroom/temperature/config" Valid
   * characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param json_this_device_doc Information about this device. The device is this hardware that have all the Home
   * Assistant entities. A device is not required. If there is a device, the name displayed in the UI will be the device
   * name combined with the entity name. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information. All these keys will be added to a "device" key in the Home Assistant configuration for each entity.
   * Only a flat layout structure is supported, no nesting. This is called from the setup function below before we setup
   * the remote. set to empty document if no device information should be set.
   * @param verbose True to do extra debug logging and printouts.
   */
  HaBridge(IMQTTRemote &remote, std::string node_id, IJsonDocument &this_device_json_doc, bool verbose = false);

public:
  /**
   * @brief Call to publish the configuration for the HaEntity.
   * This function will set the "availability_topic" and the "unique_id" field, as well as add the
   * "device". The caller should set any entity specific fields like "name", "state_topic", "device_class",
   * "unit_of_measurement" or whatever is needed in the Configuration for the specific entity.
   *
   * The component and the object_id will create a topic like this (node_id is from the constructor of the HaBridge):
   * homeassistant/<component>/<node_id>/<object_id>/config
   *
   * For more information, see https://www.home-assistant.io/integrations/mqtt/#mqtt-discovery
   *
   * @param component This is the first path after "homeassistant/" in the topic and defines the kind of component it
   * is. Examples are "sensor", "binary_sensor", "cover", "light", etc.
   * @param object_id this is the object identifier, which is what the sensor/acturator is measuring/actuating. Examples
   * "voltage", "brightness", "movement", "cover", "light" etc. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock_upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param specific_doc Any entity specific values. See brief documentation.
   */
  void publishConfiguration(std::string component, std::string object_id, std::string child_object_id,
                            const IJsonDocument &specific_doc);

  /**
   * @brief Publish a message.
   *
   * @param topic the topic to publish to.
   * @param message The message to send. This cannot be larger than the value set for max_message_size in the
   * constructor.
   * @param retain True to set this message as retained.
   * @returns true on success, or false on failure.
   */
  bool publishMessage(std::string topic, std::string message, bool retain = false);

  enum class TopicType {
    State,      // Usually when the entity post a state for the entity.
    Command,    // Usually where the entity listen for actions/states to set (i.e. when Home Assistant update the value)
    Attributes, // For attributes.
  };

  /**
   * @brief Get the topic to use for publishing state, subscribing to events and to use in the home assistant setup.
   *
   * @param topic_type The type of topic.
   * @param component the component type, as in "light", "sensor", "binary_sensor" etc. Must be any of Home Assistant
   * acceptable components, see https://www.home-assistant.io/integrations/mqtt/
   * @param object_id the entity object ID, as in "brightness", "voltage", etc. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the state topic would be
   * "door/binary_sensor/lock/state" This works if you only have one lock on your door, but if you have two locks, you
   * want to add a child object ID to them. By setting the child_object_id to say "upper", the state topic will be
   * ""door/binary_sensor/lock/upper/state". Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   */
  std::string getTopic(TopicType topic_type, std::string component, std::string object_id,
                       std::string child_object_id = "");

  /**
   * @brief Raw IMQTTRemote. Usually only needed for subscription. Otherwise use publishConfiguration() and
   * publishMessage defined here).
   */
  IMQTTRemote &remote() { return _remote; }

private:
  std::string topicType(TopicType topic_type);

private:
  bool _verbose;
  std::string _node_id;
  IMQTTRemote &_remote;
  IJsonDocument &_this_device_json_doc;
};

#endif // __HA_BRIDGE_H__