#ifndef __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__
#define __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a volatile organic compounds sensor, concentration in μg/m³ or parts in ppb/parts per billion.
 */
class HaEntityVolatileOrganicCompounds : public HaEntity {
public:
  enum class Unit {
    Concentration, // Concentration of volatile organic compounds in µg/m³
    Parts,         // Ratio of volatile organic compounds in ppb/parts per billion
  };

  struct Configuration {
    /**
     * @brief the unit of this volatile organic compound sensor. Either a concentration (μg/m³) or a ratio (ppb/parts
     * per billion).
     */
    Unit unit = Unit::Parts;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.unit = Unit::Parts, .force_update = false};

  /**
   * @brief Construct a new Ha Entity volatile organic compounds object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "volatile organic compounds", friendly name will be "Bathroom
   * volatile organic compounds". If no device, friendly name will be just "volatile organic compounds". If a device is
   * set, this name can be left empty if this entity is the one main entity (or only) entity of this device. See
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
  HaEntityVolatileOrganicCompounds(HaBridge &ha_bridge, std::string name,
                                   std::optional<std::string> child_object_id = std::nullopt,
                                   Configuration configuration = _default)
      : _ha_entity_sensor(HaEntitySensor(ha_bridge, name, child_object_id,
                                         HaEntitySensor::Configuration{
                                             .device_class = deviceClass(configuration),
                                             .unit_of_measurement = unitOfMeasurement(configuration),
                                             .force_update = configuration.force_update,
                                         })) {}

public:
  void publishConfiguration() override { _ha_entity_sensor.publishConfiguration(); }
  void republishState() override { _ha_entity_sensor.republishState(); }

  /**
   * @brief Publish the volatile organic compounds concentration or parts, depending on Unit selected in configuration.
   *
   * @param concentration concentration/parts in µg/m³ or ppb, depending on what was selected in the configuration.
   */
  void publishConcentration(double concentration) { _ha_entity_sensor.publishValue(concentration); }

private:
  const homeassistantentities::DeviceClass &deviceClass(const Configuration &configuration) const {
    switch (configuration.unit) {
    case Unit::Concentration:
      return _volatile_organic_compounds;
    case Unit::Parts:
      return _volatile_organic_compounds_parts;
    }
    return _volatile_organic_compounds_parts; // noop
  }

  std::optional<homeassistantentities::UnitType> unitOfMeasurement(const Configuration &configuration) const {
    switch (configuration.unit) {
    case Unit::Concentration:
      return homeassistantentities::Sensor::VolatileOrganicCompounds::Unit::ug_m3;
    case Unit::Parts:
      return homeassistantentities::Sensor::VolatileOrganicCompoundsParts::Unit::ppb;
    }
    return std::nullopt;
  }

private:
  const homeassistantentities::Sensor::VolatileOrganicCompounds _volatile_organic_compounds;
  const homeassistantentities::Sensor::VolatileOrganicCompoundsParts _volatile_organic_compounds_parts;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_VOLATILE_ORGANIC_COMPOUNDS_H__