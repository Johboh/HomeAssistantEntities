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
 * @brief Represents an MQTT Lock actuator (`lock.mqtt`) that reports lock states to Home Assistant and receives
 * `Lock`, `Unlock`, and optional `Open` (unlatch) commands.
 *
 * See https://www.home-assistant.io/integrations/lock.mqtt/
 *
 * @par Example
 * @code
 * HaEntityLock front_lock(ha_bridge, "Front Door Lock", "front", {
 *     .with_open = true,
 * });
 *
 * front_lock.onCommand([&](HaEntityLock::Action action) {
 *   if (action == HaEntityLock::Action::Lock) {
 *     front_lock.publish(HaEntityLock::State::Locked);
 *   } else if (action == HaEntityLock::Action::Unlock) {
 *     front_lock.publish(HaEntityLock::State::Unlocked);
 *   } else if (action == HaEntityLock::Action::Open) {
 *     front_lock.publish(HaEntityLock::State::Open);
 *   }
 * });
 *
 * // Once MQTT is connected:
 * front_lock.publishConfiguration();
 * front_lock.publish(true); // Publishes LOCKED (or pass HaEntityLock::State)
 * @endcode
 */
class HaEntityLock : public HaEntity {
public:
  struct Configuration {
    /**
     * @brief If `true`, enables the unlatch/`"OPEN"` command in Home Assistant (`payload_open: "OPEN"`).
     */
    bool with_open = false;

    /**
     * @brief If `true`, tells Home Assistant to publish command messages with the MQTT retain flag set.
     */
    bool retain = false;

    /**
     * @brief Optional regular expression to validate PIN codes entered in the Home Assistant UI or service calls (e.g.
     * `"^\\d{4}$"`).
     */
    std::string code_format = "";

    /**
     * @brief Optional custom icon (e.g. `"mdi:lock"`).
     */
    std::optional<std::string> icon = std::nullopt;

    /**
     * @brief Optional entity category (`"config"` or `"diagnostic"`).
     */
    std::optional<std::string> entity_category = std::nullopt;

    /**
     * @brief Defines whether the lock works in optimistic mode in Home Assistant.
     */
    bool optimistic = false;

    /**
     * @brief Defines whether the entity should be enabled when first added to Home Assistant. Defaults to `true` in
     * Home Assistant when omitted.
     */
    std::optional<bool> enabled_by_default = std::nullopt;

    /**
     * @brief If `true`, configure a JSON attributes topic (`json_attributes_topic`) for publishing attributes.
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
   * @brief Construct a new HaEntityLock object.
   *
   * @param ha_bridge Bridge for MQTT and Home Assistant.
   * @param name Human-readable name used for the entity in Home Assistant. If a device is set on `ha_bridge`, the
   * friendly name displayed in the UI is the device name plus this name (e.g. device `"Front Door"` + name `"Lock"` ->
   * `"Front Door Lock"`). Can be empty if a device is set and this is the main (or only) entity of the device. See
   * https://developers.home-assistant.io/docs/core/entity/#entity-naming for details.
   * @param child_object_id Optional machine-readable child identifier (`[a-zA-Z0-9_-]`) in case there are several locks
   * on the same node ID (e.g. `"upper"`). Pass `std::nullopt` or `""` when not needed.
   * @param configuration Configuration for this lock entity.
   */
  HaEntityLock(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id = std::nullopt,
               Configuration configuration = _default);

public:
  void publishConfiguration() override;
  void unpublishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish whether the lock is locked (`true` -> `"LOCKED"`, `false` -> `"UNLOCKED"`), regardless of whether
   * the state has changed. Also see `update()`.
   *
   * @param locked `true` if locked, `false` if unlocked.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(bool locked, const Attributes::Map &attributes = {});

  /**
   * @brief Publish whether the lock is locked (`true` -> `"LOCKED"`, `false` -> `"UNLOCKED"`), only if the state (or
   * attributes) has changed. Also see `publish()`.
   *
   * @param locked `true` if locked, `false` if unlocked.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(bool locked, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a detailed lock `State` (`Locked`, `Unlocked`, `Locking`, `Unlocking`, `Jammed`, `Opening`, `Open`)
   * regardless of whether the state has changed. Also see `update()`.
   *
   * @param state Detailed lock state to publish.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void publish(State state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish a detailed lock `State` only if the state (or attributes) has changed. Also see `publish()`.
   *
   * @param state Detailed lock state to publish.
   * @param attributes Optional JSON attributes (`Configuration::with_attributes` must be `true`).
   */
  void update(State state, const Attributes::Map &attributes = {});

  /**
   * @brief Publish JSON attributes only (`Configuration::with_attributes` must be `true`), regardless of whether they
   * have changed.
   *
   * @param attributes Attributes map to publish.
   */
  void publishAttributes(const Attributes::Map &attributes);

  /**
   * @brief Publish JSON attributes only if they have changed (`Configuration::with_attributes` must be `true`).
   *
   * @param attributes Attributes map to publish.
   */
  void updateAttributes(const Attributes::Map &attributes);

  /**
   * @brief Set callback for receiving `Lock`, `Unlock`, and `Open` commands from Home Assistant.
   *
   * @param callback Callback invoked with the requested `Action`.
   * @return true if the subscription succeeded, false otherwise.
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
