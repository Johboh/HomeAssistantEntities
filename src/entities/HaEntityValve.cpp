// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityValve.h"
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

HaEntityValve::HaEntityValve(HaBridge &ha_bridge, std::string name, std::optional<std::string> child_object_id,
                             Configuration configuration)
    : _configuration(std::move(configuration)), _callbacks(std::make_shared<Callbacks>()),
      _ha_entity_actuator(ha_bridge, std::move(name), std::move(child_object_id),
                          HaEntityActuator::Configuration{
                              .device_class = homeassistantentities::Valve::Undefined::Empty{},
                              .device_class_override = _configuration.device_class,
                              .sub_topic = "",
                              .with_state_topic = true,
                              .with_command_topic = !_configuration.read_only,
                              .with_attributes = _configuration.with_attributes,
                              .retain = _configuration.retain,
                              .optimistic = _configuration.optimistic,
                              .icon = _configuration.icon,
                              .entity_category = _configuration.entity_category,
                              .enabled_by_default = _configuration.enabled_by_default,
                              .extra_configuration =
                                  [cfg = _configuration](IJsonDocument &doc) {
                                    if (cfg.reports_position) {
                                      doc["reports_position"] = true;
                                      doc["position_open"] = cfg.position_open;
                                      doc["position_closed"] = cfg.position_closed;
                                    } else if (!cfg.read_only) {
                                      doc["payload_open"] = "OPEN";
                                      doc["payload_close"] = "CLOSE";
                                    }
                                    if (!cfg.read_only && cfg.with_stop) {
                                      doc["payload_stop"] = "STOP";
                                    }
                                  },
                          }) {}

bool HaEntityValve::buildComponentConfiguration(ComponentConfig &out) {
  return buildChildComponentConfiguration(_ha_entity_actuator, out);
}

void HaEntityValve::publishConfiguration() { _ha_entity_actuator.publishConfiguration(); }

void HaEntityValve::unpublishConfiguration() { _ha_entity_actuator.unpublishConfiguration(); }

void HaEntityValve::republishState() { _ha_entity_actuator.republishState(); }

uint8_t HaEntityValve::clampPosition(uint8_t position) const {
  uint8_t lo = _configuration.position_closed;
  uint8_t hi = _configuration.position_open;
  if (lo > hi) {
    std::swap(lo, hi);
  }
  return std::clamp(position, lo, hi);
}

std::optional<std::string> HaEntityValve::encodePayload(std::optional<State> state,
                                                        std::optional<uint8_t> position) const {
  auto stateToStr = [](State s) -> std::optional<std::string> {
    switch (s) {
    case State::Open:
      return "open";
    case State::Opening:
      return "opening";
    case State::Closed:
      return "closed";
    case State::Closing:
      return "closing";
    case State::Stopped:
      return "stopped";
    case State::Unknown:
      return std::nullopt;
    }
    return std::nullopt;
  };

  if (_configuration.reports_position) {
    if (position && state && (*state == State::Opening || *state == State::Closing || *state == State::Stopped)) {
      IJsonDocument doc;
      doc["position"] = clampPosition(*position);
      doc["state"] = *stateToStr(*state);
      return toJsonString(doc);
    }
    if (position) {
      return std::to_string(clampPosition(*position));
    }
    if (state) {
      if (*state == State::Open) {
        return std::to_string(_configuration.position_open);
      }
      if (*state == State::Closed) {
        return std::to_string(_configuration.position_closed);
      }
      auto s = stateToStr(*state);
      if (s) {
        IJsonDocument doc;
        doc["state"] = *s;
        return toJsonString(doc);
      }
    }
    return std::nullopt;
  }

  if (state) {
    return stateToStr(*state);
  }
  return std::nullopt;
}

void HaEntityValve::publish(bool open, const Attributes::Map &attributes) {
  publish(open ? State::Open : State::Closed, std::nullopt, attributes);
}

void HaEntityValve::update(bool open, const Attributes::Map &attributes) {
  update(open ? State::Open : State::Closed, std::nullopt, attributes);
}

void HaEntityValve::publish(std::optional<State> state, std::optional<uint8_t> position,
                            const Attributes::Map &attributes) {
  auto payload = encodePayload(state, position);
  if (payload) {
    _ha_entity_actuator.publish(*payload, attributes);
  }
}

void HaEntityValve::update(std::optional<State> state, std::optional<uint8_t> position,
                           const Attributes::Map &attributes) {
  auto payload = encodePayload(state, position);
  if (payload) {
    _ha_entity_actuator.update(*payload, attributes);
  }
}

void HaEntityValve::publishPosition(uint8_t position, const Attributes::Map &attributes) {
  if (!_configuration.reports_position) {
    return;
  }
  _ha_entity_actuator.publish(std::to_string(clampPosition(position)), attributes);
}

void HaEntityValve::updatePosition(uint8_t position, const Attributes::Map &attributes) {
  if (!_configuration.reports_position) {
    return;
  }
  _ha_entity_actuator.update(std::to_string(clampPosition(position)), attributes);
}

bool HaEntityValve::ensureSubscribed() {
  if (_configuration.read_only) {
    return false;
  }
  if (_callbacks->subscribed) {
    return true;
  }
  uint8_t lo = _configuration.position_closed;
  uint8_t hi = _configuration.position_open;
  if (lo > hi) {
    std::swap(lo, hi);
  }
  bool reports_pos = _configuration.reports_position;
  bool with_stop = _configuration.with_stop;
  auto cbs = _callbacks;

  bool ok = _ha_entity_actuator.onCommand([cbs, lo, hi, reports_pos, with_stop](const std::string &message) {
    if (message == "OPEN") {
      if (cbs->on_action) {
        cbs->on_action(Action::Open);
      }
      return;
    }
    if (message == "CLOSE") {
      if (cbs->on_action) {
        cbs->on_action(Action::Close);
      }
      return;
    }
    if (with_stop && message == "STOP") {
      if (cbs->on_action) {
        cbs->on_action(Action::Stop);
      }
      return;
    }
    if (reports_pos) {
      char *end = nullptr;
      long pos = std::strtol(message.c_str(), &end, 10);
      if (end != message.c_str() && *end == '\0' && pos >= lo && pos <= hi) {
        if (cbs->on_position) {
          cbs->on_position(static_cast<uint8_t>(pos));
        } else if (cbs->on_action) {
          if (pos == hi) {
            cbs->on_action(Action::Open);
          } else if (pos == lo) {
            cbs->on_action(Action::Close);
          }
        }
      }
    }
  });
  _callbacks->subscribed = ok;
  return ok;
}

bool HaEntityValve::onCommand(std::function<void(Action)> callback) {
  if (_configuration.read_only || !callback) {
    return false;
  }
  _callbacks->on_action = std::move(callback);
  return ensureSubscribed();
}

bool HaEntityValve::onPosition(std::function<void(uint8_t)> callback) {
  if (_configuration.read_only || !_configuration.reports_position || !callback) {
    return false;
  }
  _callbacks->on_position = std::move(callback);
  return ensureSubscribed();
}
