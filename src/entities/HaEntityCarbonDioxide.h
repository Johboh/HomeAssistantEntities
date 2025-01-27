#ifndef __HA_ENTITY_CARBON_DIOXIDE_H__
#define __HA_ENTITY_CARBON_DIOXIDE_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Carbon Dioxide sensor (e.g. SGP30) (see
 * homeassistantentities::Sensor::CarbonDioxide:Unit  in HaDeviceClasses.h).
 */
class HaEntityCarbonDioxide : public HaEntity {
public:
  struct Configuration {
    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.force_update = false};

  /**
   * @brief Construct a new Ha Entity Carbon Dioxide object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "carbon dioxide", friendly name will be "Bathroom carbon
   * dioxide". If no device, friendly name will be just "carbon dioxide". If a device is set, this name can be left
   * empty if this entity is the one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID. Valid characters
   * are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityCarbonDioxide(HaBridge &ha_bridge, std::string name,
                        std::optional<std::string> child_object_id = std::nullopt,
                        Configuration configuration = _default)
      : _ha_entity_sensor(
            HaEntitySensor(ha_bridge, name, child_object_id,
                           HaEntitySensor::Configuration{
                               .device_class = _carbon_dioxide,
                               .unit_of_measurement = homeassistantentities::Sensor::CarbonDioxide::Unit::ppm,
                               .force_update = configuration.force_update,
                           })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the carbon dioxide concentration.
   *
   * @param ppm the carbon dioxide concentration in ppm/parts per million.
   */
  void publishConcentration(double concentration) { _ha_entity_sensor.publishValue(concentration); }

private:
  const homeassistantentities::Sensor::CarbonDioxide _carbon_dioxide;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_CARBON_DIOXIDE_H__