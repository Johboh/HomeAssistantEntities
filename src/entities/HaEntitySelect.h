#ifndef __HA_ENTITY_SELECT_H__
#define __HA_ENTITY_SELECT_H__

#include <Arduino.h>
#include <HaBridge.h>
#include <HaEntity.h>
#include <functional>
#include <set>

/**
 * @brief Represent a Select that can be set by Home Assistant or reported back to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/select.mqtt/
 */
class HaEntitySelect : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Select object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "select", friendly name will be "Bathroom select". If no
   * device, friendly name will be just "select". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this select. Should be unique for each select for this node/device. Example:
   * "balcony_leds_profile". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param options Set of options that can be selected. An empty set or a set with a single item is allowed.
   */
  HaEntitySelect(HaBridge &ha_bridge, String name, String object_id, std::set<String> options);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current selected option. Must be one of the options in the options list.
   *
   * @param option thte option selected.
   */
  void publishSelection(String option);

  /**
   * @brief Set callback for receving callbacks when there is a new option that should be set.
   */
  bool setOnSelected(std::function<void(String)> select_callback);

private:
  String _name;
  HaBridge &_ha_bridge;
  String _object_id;
  std::set<String> _options;
  unsigned long _total_string_length_of_options = 0;

private:
  std::optional<String> _selection;
};

#endif // __HA_ENTITY_SELECT_H__