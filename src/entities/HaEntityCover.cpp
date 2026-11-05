// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaEntityCover.h"
#include <HaUtilities.h>
#include <IJson.h>
#include <algorithm>
#include <cstdlib>
#include <utility>

// NOTE! We have swapped object ID and child object ID to get a nicer state/command topic path.
#define COMPONENT "cover"
#define OBJECT_ID "curtain"
#define OBJECT_ID_STATE "state"
#define OBJECT_ID_POSITION "position"

using namespace homeassistantentities;

HaEntityCover::HaEntityCover(HaBridge &ha_bridge, std::string name, std::string child_object_id,
                             Configuration configuration)
    : _name(homeassistantentities::trim(name)), _ha_bridge(ha_bridge), _child_object_id(std::move(child_object_id)),
      _configuration(std::move(configuration)) {
  if (!_configuration.with_state_topic && !_configuration.with_position) {
    _configuration.with_state_topic = true;
  }
}

void HaEntityCover::publishConfiguration() {
  IJsonDocument doc;

  if (!_name.empty()) {
    doc["name"] = _name;
  } else {
    doc["name"] = nullptr;
  }

  auto trimmed_device_class = trim(_configuration.device_class);
  if (!trimmed_device_class.empty()) {
    doc["device_class"] = trimmed_device_class;
  }

  if (!_configuration.read_only) {
    doc["retain"] = _configuration.retain;
  }

  if (_configuration.with_state_topic) {
    doc["state_topic"] = _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE);
    if (!_configuration.read_only) {
      doc["command_topic"] =
          _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_STATE);
    }
  }

  if (_configuration.with_position) {
    doc["position_topic"] =
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_POSITION);
    if (!_configuration.read_only) {
      doc["set_position_topic"] =
          _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_POSITION);
    }

    doc["position_open"] = _configuration.position_open;
    doc["position_closed"] = _configuration.position_closed;
  }

  _ha_bridge.publishConfiguration(COMPONENT, OBJECT_ID, _child_object_id, doc);
}

void HaEntityCover::republishState() { publish(_state, _position); }

void HaEntityCover::publish(std::optional<State> state, std::optional<uint8_t> position) {
  publishState(state);
  publishPosition(position);
}

uint8_t HaEntityCover::clampPosition(uint8_t position) const {
  uint8_t lo = _configuration.position_closed;
  uint8_t hi = _configuration.position_open;
  if (lo > hi) {
    std::swap(lo, hi);
  }
  return std::clamp(position, lo, hi);
}

void HaEntityCover::publishState(std::optional<State> state) {
  if (!_configuration.with_state_topic) {
    return;
  }
  if (state) {
    std::string str;
    switch (*state) {
    case State::Open:
      str = "open";
      break;
    case State::Opening:
      str = "opening";
      break;
    case State::Closed:
      str = "closed";
      break;
    case State::Closing:
      str = "closing";
      break;
    case State::Stopped:
      str = "stopped";
      break;
    case State::Unknown:
      break;
    }
    if (!str.empty()) {
      _ha_bridge.publishMessage(
          _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_STATE), str);
      _state = state;
    }
  }
}

void HaEntityCover::publishPosition(std::optional<uint8_t> position) {
  if (!_configuration.with_position) {
    return;
  }
  if (position) {
    uint8_t clamped = clampPosition(*position);
    _ha_bridge.publishMessage(
        _ha_bridge.getTopic(HaBridge::TopicType::State, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
        std::to_string(clamped));
    _position = clamped;
  }
}

void HaEntityCover::update(std::optional<State> state, std::optional<uint8_t> position) {
  if (_configuration.with_state_topic && state && state != State::Unknown && state != _state) {
    publishState(state);
  }
  if (_configuration.with_position && position) {
    uint8_t clamped = clampPosition(*position);
    if (!_position || *_position != clamped) {
      publishPosition(clamped);
    }
  }
}

bool HaEntityCover::setOnState(std::function<void(Action)> state_callback) {
  if (_configuration.read_only || !_configuration.with_state_topic || !state_callback) {
    return false;
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_STATE),
      [state_callback](std::string, std::string message) {
        Action state = Action::Unknown;
        if (message == "OPEN") {
          state = Action::Open;
        } else if (message == "CLOSE") {
          state = Action::Close;
        } else if (message == "STOP") {
          state = Action::Stop;
        }

        if (state != Action::Unknown) {
          state_callback(state);
        }
      });
}

bool HaEntityCover::setOnPosition(std::function<void(uint8_t)> position_callback) {
  if (_configuration.read_only || !_configuration.with_position || !position_callback) {
    return false;
  }
  uint8_t lo = _configuration.position_closed;
  uint8_t hi = _configuration.position_open;
  if (lo > hi) {
    std::swap(lo, hi);
  }
  return _ha_bridge.remote().subscribe(
      _ha_bridge.getTopic(HaBridge::TopicType::Command, COMPONENT, _child_object_id, OBJECT_ID_POSITION),
      [position_callback, lo, hi](std::string, std::string message) {
        char *end = nullptr;
        long pos = std::strtol(message.c_str(), &end, 10);
        if (end != message.c_str() && *end == '\0' && pos >= lo && pos <= hi) {
          position_callback(static_cast<uint8_t>(pos));
        }
      });
}
