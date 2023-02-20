#ifndef __HA_ENTITY_VOLTAGE_H__
#define __HA_ENTITY_VOLTAGE_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>

/**
 * @brief Represent a Voltage sensor (V).
 */
class HaEntityVoltage : public HaEntity {
public:
  enum class Unit {
    V,
    mV,
  };

public:
  /**
   * @brief Construct a new Ha Entity Voltage object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. Example: "Bathroom
   * battery voltage"
   * @param child_object_id optional child identifier for this entity in case there are several sensors of the same
   * entity type for the same node ID. Example: If you have a lock for the node ID "door", the home asisstant
   * configuration path will be "homeassistant/binary_sensor/door/lock/config". This works if you only have one lock on
   * your door, but if you have two locks, you want to add a child object ID to them. By setting the child_object_id to
   * say "upper", the configuration will be "homeassistant/binary_sensor/door/lock/upper/config". This also apply for
   * all state/command topics and so on. Leave as empty string for no child object ID.
   * @param force_update In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
   * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
   * message (not only when the sensor’s new state is different to the current one).
   * @param unit the unit of measurement reported for this sensor. Make sure that the value you publish is of this unit.
   */
  HaEntityVoltage(HaBridge &ha_bridge, String name, String child_object_id = "", bool force_update = false,
                  Unit unit = Unit::V);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the voltage.
   *
   * @param voltage voltage in mV or V, depending on what was selected at construction.
   */
  void publishVoltage(double voltage);

private:
  Unit _unit;
  String _name;
  bool _force_update;
  HaBridge &_ha_bridge;
  String _child_object_id;

private:
  std::optional<double> _voltage;
};

#endif // __HA_ENTITY_VOLTAGE_H__