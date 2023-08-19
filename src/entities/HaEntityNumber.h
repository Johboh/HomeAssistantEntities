#ifndef __HA_ENTITY_LOCK_H__
#define __HA_ENTITY_LOCK_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>

/**
 * @brief Represent a Number sensor/actuator.
 * See more at https://www.home-assistant.io/integrations/number.mqtt/
 */
class HaEntityNumber : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Number object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "number", friendly name wil lbe "Bathroom number". If no
   * device, friendly name will be just "number". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this number. Should be unique for each number for this node/device. Example:
   * "balcony_leds_speed". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param min_value minimum value allowed.
   * @param max_value maximum value allowed.
   */
  HaEntityNumber(HaBridge &ha_bridge, String name, String object_id, float min_value = 1.0, float max_value = 100.0);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the number.
   */
  void publishNumber(float number);

  /**
   * @brief Set callback for receving callbacks when there is a new number.
   */
  bool setOnNumber(std::function<void(float)> callback);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _object_id;
  float _min_value;
  float _max_value;

private:
  std::optional<float> _number;
};

#endif // __HA_ENTITY_LOCK_H__