// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#ifndef __HA_ENTITY_LOCK_H__
#define __HA_ENTITY_LOCK_H__

#include "AttributeVariants.h"
#include "HaDeviceClasses.h"
#include "HaEntityActuator.h"
#include <HaBridge.h>
#include <HaEntity.h>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>

/**
 * @brief Represent a Lock actuator that reports lock states to Home Assistant and receives lock/unlock/open commands.
 * See more at https://www.home-assistant.io/integrations/lock.mqtt/
 */
class HaEntityLock : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If true, enables the unlatch/"open" command in Home Assistant (`payload_open: "OPEN"`).
     */
    bool with_open = false;

    /**
     * @brief If true, tells Home Assistant to publish command messages with retain set to true.
     */
    bool retain = false;

    /**
     * @brief Optional regular expression to validate PIN codes entered in Home Assistant UI/actions (e.g. "^\\d{4}$").
     */
    std::string code_format = "";

    /**
     * @brief Optional custom icon (e.g. "mdi:lock").
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category ("config" or "diagnostic").
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Flag that defines if lock works in optimistic mode.
     */
    bool optimistic = false;

    /**
     * @brief Flag which defines if the entity should be enabled when first added to Home Assistant. Defaults to true
     * in Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If true, configure an attributes topic (`json_attributes_topic`) for publishing attributes.
     */
    bool with_attributes = false;
  };

  inline static Configuration _default = {.with_open = false,
                                          .retain = false,
                                          .code_format = "",
                                          .icon = std::nullopt,
                                          .entity_category = std::nullopt,
                                          .optimistic = false,
                                          .enabled_by_default = std::nullopt,
                                          .with_attributes = false};

  enum class State {
    Locked,
    Unlocked,
    Locking,
    Unlocking,
    Jammed,
    Opening,
    Open,
  };

  enum class Action {
    Lock,
    Unlock,
    Open,
    Unknown,
  };

  /**
   * @brief Construct a new Ha Entity Lock object
   *
   * @param name Human-readable name used for the entity in Home Assistant.
   * @param child_object_id Optional child identifier in case there are several locks for the same node ID.
   * @param configuration Configuration for this lock entity.
   */
  HaEntityLock(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
               Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish whether the lock is locked (`true` -> `LOCKED`, `false` -> `UNLOCKED`).
   */
  void publish(bool locked, const Attributes::Map &attributes = {});

  /**
   * @brief Publish whether the lock is locked only if the state has changed.
   */
  void update(bool locked, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a detailed lock state (`Locked`, `Unlocked`, `Locking`, `Unlocking`, `Jammed`, `Opening`, `Open`).
   */
  void publish(State state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a detailed lock state only if it changed.
   */
  void update(State state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish JSON attributes (`with_attributes` must be true).
   */
  void publishAttributes(const Attributes::Map &attributes);

  /**
   * @brief Publish JSON attributes only if changed (`with_attributes` must be true).
   */
  void updateAttributes(const Attributes::Map &attributes);

  /**
   * @brief Set callback for receiving Lock / Unlock / Open commands from Home Assistant.
   */
  bool onCommand(std::function<void(Action)> callback);

protected:
  bool buildComponentConfiguration(ComponentConfig &out) override;

private:
  static const char *stateToPayload(State state);

private:
  bool _with_open;
  HaEntityActuator _ha_entity_actuator;
};

#endif // __HA_ENTITY_LOCK_H__
