#ifndef __HA_ENTITY_SENSOR_H__
#define __HA_ENTITY_SENSOR_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief A generic sensor/binary sensor. Consider using any of the specific sensors first, like HaEntityTemperature,
 * HaEntityVoltage etc.
 */
class HaEntitySensor : public HaEntity {
public:
  enum class SensorType {
    Sensor,       // A sensor with a numeric or string value
    BinarySensor, // A sensor with a boolean value (ON/OFF)
  };

  struct Configuration {
    /**
     * @brief The type of sensor. See enum.
     */
    SensorType sensor_type = SensorType::Sensor;

    /**
     * @brief the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
     */
    std::optional<std::string> unit_of_measurement = std::nullopt;

    /**
     * @brief the device class to use for this sensor. See
     * https://www.home-assistant.io/integrations/sensor/#device-class for valid values.
     */
    std::optional<std::string> device_class = std::nullopt;

    /**
     * @brief The state class to use for this sensor. See
     * https://developers.home-assistant.io/docs/core/entity/sensor/#available-state-classes
     *
     */
    std::optional<std::string> state_class = "measurement";

    /**
     * @brief if true, setup an attribute topic attributes will be published to. With this set, attributes
     * can be published when the message is published, or using a separate call.
     */
    bool with_attributes = false;

    /**
     * @brief A custom icon for the sensor. Usually Home Assistant picks a good one. Example "mdi:brightness-percent"
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.sensor_type = SensorType::Sensor,
                                          .unit_of_measurement = std::nullopt,
                                          .device_class = std::nullopt,
                                          .state_class = "measurement",
                                          .with_attributes = false,
                                          .icon = std::nullopt,
                                          .force_update = false};

  /**
   * @brief Construct a new Ha Entity Sensor object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "voltage", friendly name will be "Bathroom voltage". If no
   * device, friendly name will be just "voltage". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable).
   * @param configuration the configuration for this entity.
   */
  HaEntitySensor(HaBridge &ha_bridge, std::string name, std::string child_object_id = "",
                 Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the value for this sensor.
   *
   * @param value value in unit you specified during object creation.
   * @param attributes optional attributes to send with the value. with_attributes in configuration must be set.
   */
  void publishValue(double value, Attributes::Map attributes = {});

  /**
   * @brief Publish the value for this sensor.
   *
   * @param value value in unit you specified during object creation.
   * @param attributes optional attributes to send with the value. with_attributes in configuration must be set.
   */
  void publishValue(std::string &value, Attributes::Map attributes = {});

  /**
   * @brief Publish attributes only. with_attributes in configuration must be set.
   *
   * @param attributes attributes to publish.
   */
  void publishAttributes(Attributes::Map attributes);

  /**
   * @brief Use with caution! In rare cases, there might be a need to override the object ID used for
   * a sensor. This must be called before any configuration or values are published.
   *
   */
  void overrideObjectId(std::string object_id) { _object_id = object_id; }

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  std::string _component;
  std::string _child_object_id;
  Configuration _configuration;

private:
  std::optional<std::string> _value;
  std::optional<Attributes::Map> _attributes;
};

#endif // __HA_ENTITY_SENSOR_H__