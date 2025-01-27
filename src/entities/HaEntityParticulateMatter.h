#ifndef __HA_ENTITY_PARTICULATE_MATTER_H__
#define __HA_ENTITY_PARTICULATE_MATTER_H__

#include "HaDeviceClasses.h"
#include "HaEntitySensor.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a patriculate matter sensor (e.g. PMS5003), μg/m³.
 */
class HaEntityParticulateMatter : public HaEntity {
public:
  enum class Size {
    pm1,  // Concentration of particulate matter less than 1 micrometer in µg/m³
    pm25, // Concentration of particulate matter less than 2.5 micrometers in µg/m³
    pm10, // Concentration of particulate matter less than 10 micrometers in µg/m³
  };

  struct Configuration {
    /**
     * @brief particulate matter size.
     */
    Size size = Size::pm10;

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;
  };

  inline static Configuration _default = {.size = Size::pm10, .force_update = false};

  /**
   * @brief Construct a new Ha Entity Patriculate Matter object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "particles", friendly name will be "Bathroom particles".
   * If no device, friendly name will be just "particles". If a device is set, this name can be left empty if this
   * entity is the one main entity (or only) entity of this device. See
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
  HaEntityParticulateMatter(HaBridge &ha_bridge, std::string name,
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
   * @brief Publish the particle concentration.
   *
   * @param concentration concentration of particles.
   */
  void publishConcentration(double concentration) { _ha_entity_sensor.publishValue(concentration); }

private:
  const homeassistantentities::DeviceClass &deviceClass(const Configuration &configuration) const {
    switch (configuration.size) {
    case Size::pm1:
      return _pm1;
    case Size::pm25:
      return _pm25;
    case Size::pm10:
      return _pm10;
    }
    return _pm10; // noop
  }

  const std::optional<homeassistantentities::UnitType> unitOfMeasurement(const Configuration &configuration) const {
    switch (configuration.size) {
    case Size::pm1:
      return homeassistantentities::Sensor::Pm1::Unit::ug_m3;
    case Size::pm25:
      return homeassistantentities::Sensor::Pm25::Unit::ug_m3;
    case Size::pm10:
      return homeassistantentities::Sensor::Pm10::Unit::ug_m3;
    }
    return std::nullopt;
  }

private:
  const homeassistantentities::Sensor::Pm1 _pm1;
  const homeassistantentities::Sensor::Pm25 _pm25;
  const homeassistantentities::Sensor::Pm10 _pm10;
  HaEntitySensor _ha_entity_sensor;
};

#endif // __HA_ENTITY_PARTICULATE_MATTER_H__