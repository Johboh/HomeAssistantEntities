#ifndef __HA_ENTITY_NUMBER_H__
#define __HA_ENTITY_NUMBER_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <optional>
#include <string>

/**
 * @brief Represent a Number sensor/actuator.
 * See more at https://www.home-assistant.io/integrations/number.mqtt/
 */
class HaEntityNumber : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief The minimum value allowed.
     */
    float min_value = 1.0;

    /**
     * @brief The maximum value allowed.
     */
    float max_value = 100.0;

    /**
     * @brief the unit of measurement reported for this number. Make sure that the value you publish is of this unit.
     */
    std::string unit = "";

    /**
     * @brief the device class for this number. Should be any of
     * https://www.home-assistant.io/integrations/number/#device-class
     * Will eventually be replaced by a typed device class.
     */
    std::string device_class = "";

    /**
     * In Home Assistant, trigger events even if the sensor's state hasn't changed. Useful if you want
     * to have meaningful value graphs in history or want to create an automation that triggers on every incoming state
     * message (not only when the sensor’s new state is different to the current one).
     */
    bool force_update = false;

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;
  };

  inline static Configuration _default = {
      .min_value = 1.0, .max_value = 100.0, .unit = "", .device_class = "", .force_update = false, .retain = false};

  /**
   * @brief Construct a new Ha Entity Number object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "number", friendly name will be "Bathroom number". If no
   * device, friendly name will be just "number". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this number. Should be unique for each number for this node/device. Example:
   * "balcony_leds_speed". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityNumber(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration = _default);

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
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;

private:
  std::optional<float> _number;
};

#endif // __HA_ENTITY_NUMBER_H__