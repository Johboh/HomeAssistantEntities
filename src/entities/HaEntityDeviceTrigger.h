#ifndef __HA_ENTITY_DEVICE_TRIGGER_H__
#define __HA_ENTITY_DEVICE_TRIGGER_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <string>

/**
 * @brief Represent a Device Trigger that can be sent to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/device_trigger.mqtt/
 */
class HaEntityDeviceTrigger : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The type of the trigger, e.g. button_short_press
     *
     * Entries supported by the frontend: button_short_press, button_short_release,
     * button_long_press, button_long_release, button_double_press,
     * button_triple_press, button_quadruple_press, button_quintuple_press.
     * If set to an unsupported value, will render as subtype type, e.g.
     * button_1 spammed with type set to spammed and subtype set to button_1
     */
    std::string type;

    /**
     * @brief The subtype of the trigger, e.g. button_1
     *
     * Entries supported by the frontend: turn_on, turn_off, button_1,
     * button_2, button_3, button_4, button_5, button_6.
     * If set to an unsupported value, will render as subtype type,
     * e.g. left_button pressed with type set to button_short_press and subtype set to left_button
     *
     * This type is also used as the payload when publishing the trigger event.
     */
    std::string subtype;
  };

  /**
   * @brief Construct a new Ha Entity Device Trigger object
   *
   * @param object_id object ID for this device trigger. Should be unique for each device trigger for this node/device. Example:
   * "buzzer_device_trigger". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityDeviceTrigger(HaBridge &ha_bridge, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish a trigger event.
   */
  void publishTrigger();

private:
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_DEVICE_TRIGGER_H__