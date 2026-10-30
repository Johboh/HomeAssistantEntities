#ifndef __HA_ENTITY_SELECT_H__
#define __HA_ENTITY_SELECT_H__

#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief Represent a Select that can be set by Home Assistant or reported back to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/select.mqtt/
 */
class HaEntitySelect : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief List of options that can be selected (order preserved in Home Assistant UI). An empty list or a list with
     * a single item is allowed.
     */
    std::vector<std::string> options = {};

    /**
     * @brief If true, this tells Home Assistant to publish the message on the command topic with retain set to true.
     */
    bool retain = false;
  };

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
   * @param configuration the configuration for this entity.
   */
  HaEntitySelect(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the current selected option. Must be one of the options in the options list. This will publish to
   * MQTT regardless if the value has changed. Also see updateSelection().
   *
   * @param option the option selected.
   */
  void publishSelection(const std::string &option);

  /**
   * @brief Publish the current selected option, but only if the value has changed. Also see publishSelection(). Must be
   * one of the options in the options list.
   *
   * @param option the option selected.
   */
  void updateSelection(const std::string &option);

  /**
   * @brief Set callback for receiving callbacks when there is a new option that should be set.
   */
  bool setOnSelected(std::function<void(std::string)> select_callback);

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;

private:
  std::optional<std::string> _selection;
};

#endif // __HA_ENTITY_SELECT_H__
