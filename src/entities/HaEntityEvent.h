// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_EVENT_H__
#define __HA_ENTITY_EVENT_H__

#include "AttributeVariants.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

/**
 * @brief Represent an Event that can be sent to Home Assistant.
 * See more at https://www.home-assistant.io/integrations/event.mqtt/
 */
class HaEntityEvent : public HaEntity {
public:
  enum class DeviceClass {
    None,
    Button,
    Motion,
    Doorbell,
  };

  struct Configuration {
    /**
     * @brief A list of valid event_type strings.
     */
    std::vector<std::string> event_types = {};

    /**
     * @brief the device class for this event. None to not specify (default).
     */
    DeviceClass device_class = DeviceClass::None;

    /**
     * @brief Optional custom icon (e.g. "mdi:bell-ring").
     */
    std::string icon = "";

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::string entity_category = "";

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;
  };

  /**
   * @brief Construct a new Ha Entity Event object
   *
   * @param name this is the human readable name that will be used for the entity in Home Assistant. If a device is set
   * when creating the [HaBridge], the friendly named displayed in the UI will be the device name plus this name.
   * Example: if device name is "Bathroom" and entity name "event", friendly name will be "Bathroom event". If no
   * device, friendly name will be just "event". If a device is set, this name can be left empty if this entity is the
   * one main entity (or only) entity of this device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for more
   * information.
   * @param object_id object ID for this event. Should be unique for each event for this node/device. Example:
   * "balcony_leds_event". Valid characters are [a-zA-Z0-9_-] (machine readable, not human readable)
   * @param configuration the configuration for this entity.
   */
  HaEntityEvent(HaBridge &ha_bridge, std::string name, std::string object_id, Configuration configuration);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish an event. Must be one of the event types in the event_types list.
   *
   * @param event the event.
   * @param attributes optional attributes to send with the event.
   */
  void publishEvent(const std::string &event, const Attributes::Map &attributes = {});

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  std::string _name;
  HaBridge &_ha_bridge;
  std::string _object_id;
  Configuration _configuration;
};

#endif // __HA_ENTITY_EVENT_H__
