// Copyright (c) Johan Böhlin
// SPDX-License-Identifier: MIT

#include "HaBridge.h"
#include <algorithm>
#include <set>
#include <utility>
#if __has_include(<mutex>) && defined(__GTHREADS)
#include <mutex>
#define HA_BRIDGE_HAS_MUTEX 1
#endif

using namespace homeassistantentities;

namespace {
constexpr const char *DEFAULT_ORIGIN_NAME = "HomeAssistantEntities";
constexpr const char *DEFAULT_ORIGIN_SUPPORT_URL = "https://github.com/Johboh/HomeAssistantEntities";

std::string combineObjectId(const std::string &object_id, const std::string &child_object_id) {
  std::string base = sanitizePath(trim(object_id));
  auto coid = trim(child_object_id);
  if (!coid.empty()) {
    if (!base.empty()) {
      base += "_" + sanitizePath(coid);
    } else {
      base = sanitizePath(coid);
    }
  }
  return base;
}

template <typename DocType> void writeOrigin(DocType &doc, const HaBridge::OriginInfo &origin) {
  auto name = trim(origin.name);
  if (name.empty()) {
    return;
  }
  JsonObjectType origin_obj = createJsonObject(doc, "origin");
  origin_obj["name"] = name;
  auto sw = trim(origin.sw_version);
  if (!sw.empty()) {
    origin_obj["sw_version"] = sw;
  }
  auto url = trim(origin.support_url);
  if (!url.empty()) {
    origin_obj["support_url"] = url;
  }
}

struct BirthEntry {
  std::function<void()> callback;
  std::string expected_payload;
};

struct BirthTopicGroup {
  IMQTTRemote *remote = nullptr;
  std::string topic;
  std::vector<std::weak_ptr<BirthEntry>> listeners;
};

struct BirthRegistry {
#ifdef HA_BRIDGE_HAS_MUTEX
  std::mutex mutex;
#endif
  std::vector<BirthTopicGroup> groups;
};

BirthRegistry &birthRegistry() {
  static BirthRegistry reg;
  return reg;
}

void dispatchBirthMessage(IMQTTRemote *remote, const std::string &topic, const std::string &msg) {
  std::vector<std::shared_ptr<BirthEntry>> active;
  {
    auto &reg = birthRegistry();
#ifdef HA_BRIDGE_HAS_MUTEX
    std::lock_guard<std::mutex> lock(reg.mutex);
#endif
    for (auto &grp : reg.groups) {
      if (grp.remote == remote && grp.topic == topic) {
        auto it = grp.listeners.begin();
        while (it != grp.listeners.end()) {
          if (auto sp = it->lock()) {
            active.push_back(std::move(sp));
            ++it;
          } else {
            it = grp.listeners.erase(it);
          }
        }
        break;
      }
    }
  }
  for (const auto &entry : active) {
    if (msg == entry->expected_payload && entry->callback) {
      entry->callback();
    }
  }
}
} // namespace

void HaBridge::populateDeviceJson(IJsonDocument &doc, const DeviceInfo &info) {
  if (!info.identifiers.empty()) {
    JsonArrayType ids = createJsonArray(doc, "identifiers");
    for (const auto &id : info.identifiers) {
      auto trimmed = trim(id);
      if (!trimmed.empty()) {
        addToJsonArray(ids, trimmed);
      }
    }
  }
  if (!info.connections.empty()) {
    JsonArrayType conns = createJsonArray(doc, "connections");
    for (const auto &conn : info.connections) {
      auto ctype = trim(conn.first);
      auto cid = trim(conn.second);
      if (!ctype.empty() && !cid.empty()) {
        IJsonDocument pair_doc;
        JsonArrayType pair_arr = createJsonArray(pair_doc, "p");
        addToJsonArray(pair_arr, ctype);
        addToJsonArray(pair_arr, cid);
        addToJsonArray(conns, pair_doc["p"]);
      }
    }
  }
  auto setIfNonEmpty = [&](const char *key, const std::string &val) {
    auto trimmed = trim(val);
    if (!trimmed.empty()) {
      doc[key] = trimmed;
    }
  };
  setIfNonEmpty("name", info.name);
  setIfNonEmpty("manufacturer", info.manufacturer);
  setIfNonEmpty("model", info.model);
  setIfNonEmpty("model_id", info.model_id);
  setIfNonEmpty("sw_version", info.sw_version);
  setIfNonEmpty("hw_version", info.hw_version);
  setIfNonEmpty("serial_number", info.serial_number);
  setIfNonEmpty("via_device", info.via_device);
  setIfNonEmpty("suggested_area", info.suggested_area);
  setIfNonEmpty("configuration_url", info.configuration_url);
}

HaBridge::HaBridge(IMQTTRemote &remote, std::string node_id, IJsonDocument &this_device_json_doc, bool verbose,
                   std::function<std::string(IMQTTRemote &)> availability_topic,
                   std::function<std::string(IMQTTRemote &, std::string &)> unique_id)
    : _verbose(verbose), _node_id(std::move(node_id)), _remote(remote), _this_device_json_doc(this_device_json_doc),
      _availability_topic(std::move(availability_topic)), _unique_id(std::move(unique_id)) {}

HaBridge HaBridge::childDevice(std::string child_node_id, std::string child_identifier, std::string child_name) {
  return childDevice(std::move(child_node_id), ChildDeviceInfo{{std::move(child_identifier)}, std::move(child_name)});
}

HaBridge HaBridge::childDevice(std::string child_node_id, ChildDeviceInfo child_device_info) {
  auto clean_child_node = sanitizePath(trim(child_node_id));
  auto parent_node = sanitizePath(trim(_node_id));
  std::string combined_node = parent_node.empty()
                                  ? clean_child_node
                                  : (clean_child_node.empty() ? parent_node : (parent_node + "_" + clean_child_node));

  HaBridge child(_remote, std::move(combined_node), _this_device_json_doc, _verbose, _availability_topic, _unique_id);
  child._child_node_id = std::move(clean_child_node);
  child._discovery_prefix = _discovery_prefix;
  child._payload_available = _payload_available;
  child._payload_not_available = _payload_not_available;
  child._origin = _origin;
  child.setChildDevice(std::move(child_device_info));
  return child;
}

void HaBridge::setChildDevice(ChildDeviceInfo child_device_info) {
  ChildDeviceInfo cleaned;
  for (const auto &id : child_device_info.identifiers) {
    auto t = trim(id);
    if (!t.empty() &&
        std::find(cleaned.identifiers.begin(), cleaned.identifiers.end(), t) == cleaned.identifiers.end()) {
      cleaned.identifiers.push_back(t);
    }
  }
  cleaned.name = trim(child_device_info.name);
  if (!cleaned.identifiers.empty()) {
    _child_device = std::move(cleaned);
  } else {
    _child_device.reset();
  }
}

void HaBridge::setOrigin(OriginInfo origin) {
  origin.name = trim(origin.name);
  origin.sw_version = trim(origin.sw_version);
  origin.support_url = trim(origin.support_url);
  if (!origin.name.empty()) {
    _origin = std::move(origin);
  } else {
    _origin.reset();
  }
}

void HaBridge::setDiscoveryPrefix(std::string discovery_prefix) {
  auto p = sanitizePath(trim(discovery_prefix));
  _discovery_prefix = p.empty() ? "homeassistant" : std::move(p);
}

void HaBridge::setAvailabilityPayloads(std::string payload_available, std::string payload_not_available) {
  _payload_available = std::move(payload_available);
  _payload_not_available = std::move(payload_not_available);
}

bool HaBridge::onHomeAssistantBirth(std::function<void()> callback, std::string birth_payload) {
  if (!callback) {
    _birth_registration.reset();
    return false;
  }
  std::string status_topic = _discovery_prefix + "/status";
  auto entry = std::make_shared<BirthEntry>(BirthEntry{std::move(callback), std::move(birth_payload)});
  _birth_registration = entry;

  IMQTTRemote *remote_ptr = &_remote;
  bool need_subscribe = false;
  {
    auto &reg = birthRegistry();
#ifdef HA_BRIDGE_HAS_MUTEX
    std::lock_guard<std::mutex> lock(reg.mutex);
#endif
    BirthTopicGroup *target = nullptr;
    for (auto &grp : reg.groups) {
      if (grp.remote == remote_ptr && grp.topic == status_topic) {
        target = &grp;
        break;
      }
    }
    if (!target) {
      reg.groups.push_back(BirthTopicGroup{remote_ptr, status_topic, {}});
      target = &reg.groups.back();
    }
    target->listeners.erase(std::remove_if(target->listeners.begin(), target->listeners.end(),
                                           [](const std::weak_ptr<BirthEntry> &w) { return w.expired(); }),
                            target->listeners.end());
    need_subscribe = target->listeners.empty();
    target->listeners.push_back(entry);
  }

  if (need_subscribe) {
    _remote.subscribe(status_topic, [remote_ptr](std::string topic, std::string msg) {
      dispatchBirthMessage(remote_ptr, topic, msg);
    });
  }
  return true;
}

std::string HaBridge::getAvailabilityTopic() const {
  return _availability_topic ? _availability_topic(_remote) : (sanitizePath(_remote.clientId()) + "/status");
}

std::string HaBridge::buildUniqueId(const std::string &object_id, const std::string &child_object_id) {
  std::string uid = _unique_id ? _unique_id(_remote, _node_id) : (_remote.clientId() + "_" + _node_id);
  auto coid = trim(child_object_id);
  if (!coid.empty()) {
    uid += "_" + sanitizePath(coid);
  }
  uid += "_" + sanitizePath(object_id);
  return uid;
}

std::vector<std::string> HaBridge::sanitizedChildIdentifiers(const std::vector<std::string> &parent_identifiers) const {
  std::vector<std::string> result;
  if (!_child_device) {
    return result;
  }
  for (const auto &id : _child_device->identifiers) {
    auto t = trim(id);
    if (!t.empty() && std::find(parent_identifiers.begin(), parent_identifiers.end(), t) == parent_identifiers.end() &&
        std::find(result.begin(), result.end(), t) == result.end()) {
      result.push_back(t);
    }
  }
  return result;
}

std::string HaBridge::getDiscoveryTopic(const std::string &component, const std::string &object_id,
                                        const std::string &child_object_id) const {
  auto coid = trim(child_object_id);
  std::string topic =
      _discovery_prefix + "/" + sanitizePath(component) + "/" + sanitizePath(_node_id) + "/" + sanitizePath(object_id);
  if (!coid.empty()) {
    topic += "_" + sanitizePath(coid);
  }
  topic += "/config";
  return topic;
}

std::string HaBridge::getDeviceDiscoveryTopic() const {
  return _discovery_prefix + "/device/" + sanitizePath(_node_id) + "/config";
}

void HaBridge::publishConfiguration(const std::string &component, const std::string &object_id,
                                    const std::string &child_object_id, const IJsonDocument &specific_doc) {
  IJsonDocument doc;

  // Home Assistant's device_automation (device trigger) schema forbids availability_topic and unique_id.
  if (component != "device_automation") {
    doc["availability_topic"] = getAvailabilityTopic();
    if (_payload_available) {
      doc["payload_available"] = *_payload_available;
    }
    if (_payload_not_available) {
      doc["payload_not_available"] = *_payload_not_available;
    }
    doc["unique_id"] = buildUniqueId(object_id, child_object_id);
  }

  // Set optional device keys (required by Home Assistant for device_automation and when child_device is used).
  bool has_device = false;
  std::vector<std::string> parent_identifiers;
  for (auto kv : IJsonIteratorBegin(_this_device_json_doc)) {
    // Strip child_device on device_automation (forbidden by Home Assistant).
    if (component == "device_automation" && std::string(kv.key().c_str()) == "child_device") {
      continue;
    }
    if (std::string(kv.key().c_str()) == "identifiers") {
      extractJsonStrings(kv.value(), parent_identifiers);
    }
    doc["device"][kv.key()] = kv.value();
    has_device = true;
  }
  if (component == "device_automation" && !has_device) {
    return;
  }

  // Inject validated child_device block when configured on this bridge (never on device_automation).
  if (has_device && component != "device_automation" && _child_device) {
    auto child_ids = sanitizedChildIdentifiers(parent_identifiers);
    if (!child_ids.empty()) {
      JsonObjectType cdev = createJsonObject(doc["device"], "child_device");
      JsonArrayType ids_arr = createJsonArray(cdev, "identifiers");
      for (const auto &cid : child_ids) {
        addToJsonArray(ids_arr, cid);
      }
      if (!_child_device->name.empty()) {
        cdev["name"] = _child_device->name;
      }
    }
  }

  if (_origin) {
    writeOrigin(doc, *_origin);
  }

  for (auto kv : IJsonConstIteratorBegin(specific_doc)) {
    doc[kv.key()] = kv.value();
  }

  auto message = toJsonString(doc);
  publishMessage(getDiscoveryTopic(component, object_id, child_object_id), message, true);
}

bool HaBridge::unpublishConfiguration(const std::string &component, const std::string &object_id,
                                      const std::string &child_object_id) {
  return publishMessage(getDiscoveryTopic(component, object_id, child_object_id), "", true);
}

bool HaBridge::publishDeviceConfiguration(const std::vector<std::reference_wrapper<HaEntity>> &entities) {
  if (entities.empty()) {
    return false;
  }

  IJsonDocument doc;

  // 1. Validate & copy parent device metadata (child_device is forbidden inside root "device" in device discovery).
  bool has_identifying_value = false;
  std::vector<std::string> parent_identifiers;
  for (auto kv : IJsonIteratorBegin(_this_device_json_doc)) {
    std::string key(kv.key().c_str());
    if (key == "child_device") {
      continue;
    }
    if (key == "identifiers") {
      extractJsonStrings(kv.value(), parent_identifiers);
      if (!parent_identifiers.empty()) {
        has_identifying_value = true;
      }
    } else if (key == "connections") {
      if (jsonHasNonEmptyArrayOrString(kv.value())) {
        has_identifying_value = true;
      }
    }
    doc["device"][kv.key()] = kv.value();
  }
  if (!has_identifying_value) {
    return false;
  }

  // 2. Origin is mandatory in Home Assistant device-based discovery.
  if (_origin && !trim(_origin->name).empty()) {
    writeOrigin(doc, *_origin);
  } else {
    writeOrigin(doc, OriginInfo{DEFAULT_ORIGIN_NAME, "", DEFAULT_ORIGIN_SUPPORT_URL});
  }

  // 3. Shared availability at root.
  const std::string root_availability = getAvailabilityTopic();
  doc["availability_topic"] = root_availability;
  if (_payload_available) {
    doc["payload_available"] = *_payload_available;
  }
  if (_payload_not_available) {
    doc["payload_not_available"] = *_payload_not_available;
  }

  // 4. Collect components for parent device and any child devices, enforcing unique component IDs & child IDs.
  struct PreparedComponent {
    std::string component_id;
    HaEntity::ComponentConfig cfg;
  };
  struct ChildGroup {
    std::vector<std::string> identifiers;
    std::string name;
    std::vector<PreparedComponent> components;
  };

  std::vector<PreparedComponent> root_components;
  std::vector<ChildGroup> child_groups;
  std::set<std::string> used_component_ids;
  std::set<std::string> used_child_identifiers;

  auto allocateUniqueComponentId = [&](const std::string &preferred, const std::string &component) {
    std::string candidate = preferred.empty() ? sanitizePath(component) : preferred;
    if (used_component_ids.insert(candidate).second) {
      return candidate;
    }
    std::string with_domain = sanitizePath(component) + "_" + candidate;
    if (used_component_ids.insert(with_domain).second) {
      return with_domain;
    }
    std::size_t suffix = 2;
    while (true) {
      std::string numbered = with_domain + "_" + std::to_string(suffix++);
      if (used_component_ids.insert(numbered).second) {
        return numbered;
      }
    }
  };

  for (const auto &entity_ref : entities) {
    HaEntity::ComponentConfig cfg;
    if (!entity_ref.get().buildComponentConfiguration(cfg) || cfg.component.empty()) {
      continue;
    }
    HaBridge *comp_bridge = cfg.bridge ? cfg.bridge : this;
    std::string base_id = combineObjectId(cfg.object_id, cfg.child_object_id);

    // Device triggers are forbidden on child devices by Home Assistant; attach them to root components.
    bool is_child = (cfg.component != "device_automation" && comp_bridge->_child_device.has_value());
    if (is_child) {
      auto child_ids = comp_bridge->sanitizedChildIdentifiers(parent_identifiers);
      if (child_ids.empty()) {
        is_child = false;
      } else {
        std::string prefix =
            !comp_bridge->_child_node_id.empty() ? comp_bridge->_child_node_id : sanitizePath(child_ids.front());
        std::string preferred_id = prefix.empty() ? base_id : (prefix + "_" + base_id);

        // Find existing group with identical identifiers, or create a new one if identifiers aren't duplicated.
        ChildGroup *target_group = nullptr;
        for (auto &grp : child_groups) {
          if (grp.identifiers == child_ids) {
            target_group = &grp;
            break;
          }
        }
        if (!target_group) {
          // Ensure none of child_ids was already claimed by a different ChildGroup.
          std::vector<std::string> fresh_ids;
          for (const auto &cid : child_ids) {
            if (used_child_identifiers.find(cid) == used_child_identifiers.end()) {
              fresh_ids.push_back(cid);
            }
          }
          if (fresh_ids.empty()) {
            continue;
          }
          for (const auto &cid : fresh_ids) {
            used_child_identifiers.insert(cid);
          }
          child_groups.push_back(ChildGroup{std::move(fresh_ids), comp_bridge->_child_device->name, {}});
          target_group = &child_groups.back();
        }
        std::string comp_id = allocateUniqueComponentId(preferred_id, cfg.component);
        target_group->components.push_back(PreparedComponent{std::move(comp_id), std::move(cfg)});
        continue;
      }
    }

    std::string comp_id = allocateUniqueComponentId(base_id, cfg.component);
    root_components.push_back(PreparedComponent{std::move(comp_id), std::move(cfg)});
  }

  if (root_components.empty() && child_groups.empty()) {
    return false;
  }

  auto populateComponentEntry = [&](auto &target_map, PreparedComponent &item) {
    HaBridge *comp_bridge = item.cfg.bridge ? item.cfg.bridge : this;
    JsonObjectType comp_obj = createJsonObject(target_map, item.component_id);
    comp_obj["platform"] = item.cfg.component;
    if (item.cfg.component != "device_automation") {
      comp_obj["unique_id"] = comp_bridge->buildUniqueId(item.cfg.object_id, item.cfg.child_object_id);
      std::string comp_avail = comp_bridge->getAvailabilityTopic();
      if (comp_avail != root_availability) {
        comp_obj["availability_topic"] = comp_avail;
      }
    }
    for (auto kv : IJsonConstIteratorBegin(item.cfg.doc)) {
      comp_obj[kv.key()] = kv.value();
    }
  };

  if (!root_components.empty()) {
    JsonObjectType cmps = createJsonObject(doc, "components");
    for (auto &item : root_components) {
      populateComponentEntry(cmps, item);
    }
  }

  if (!child_groups.empty()) {
    JsonArrayType cdevs = createJsonArray(doc, "child_devices");
    for (auto &grp : child_groups) {
      JsonObjectType cdev_obj = addObjectToJsonArray(cdevs);
      JsonArrayType ids_arr = createJsonArray(cdev_obj, "identifiers");
      for (const auto &cid : grp.identifiers) {
        addToJsonArray(ids_arr, cid);
      }
      if (!grp.name.empty()) {
        cdev_obj["name"] = grp.name;
      }
      JsonObjectType cdev_cmps = createJsonObject(cdev_obj, "components");
      for (auto &item : grp.components) {
        populateComponentEntry(cdev_cmps, item);
      }
    }
  }

  return publishMessage(getDeviceDiscoveryTopic(), toJsonString(doc), true);
}

bool HaBridge::unpublishDeviceConfiguration() { return publishMessage(getDeviceDiscoveryTopic(), "", true); }

bool HaBridge::migrateToDeviceConfiguration(const std::vector<std::reference_wrapper<HaEntity>> &entities) {
  if (entities.empty()) {
    return false;
  }

  IJsonDocument migrate_doc;
  migrate_doc["migrate_discovery"] = true;
  std::string migrate_payload = toJsonString(migrate_doc);

  std::vector<std::string> old_topics;
  old_topics.reserve(entities.size());
  for (const auto &entity_ref : entities) {
    HaEntity::ComponentConfig cfg;
    if (!entity_ref.get().buildComponentConfiguration(cfg) || cfg.component.empty()) {
      continue;
    }
    HaBridge *comp_bridge = cfg.bridge ? cfg.bridge : this;
    std::string topic = comp_bridge->getDiscoveryTopic(cfg.component, cfg.object_id, cfg.child_object_id);
    comp_bridge->publishMessage(topic, migrate_payload, true);
    old_topics.push_back(std::move(topic));
  }

  if (!publishDeviceConfiguration(entities)) {
    return false;
  }

  for (const auto &topic : old_topics) {
    publishMessage(topic, "", true);
  }
  return true;
}

bool HaBridge::publishMessage(const std::string &topic, const std::string &message, bool retain) {
  if (_verbose) {
    return _remote.publishMessageVerbose(topic, message, retain);
  } else {
    return _remote.publishMessage(topic, message, retain);
  }
}

std::string HaBridge::getTopic(TopicType topic_type, const std::string &component, const std::string &object_id,
                               const std::string &child_object_id) {
  auto str = sanitizePath(_node_id) + "/" + sanitizePath(component);

  auto oid = trim(object_id);
  if (!oid.empty()) {
    str += "/" + sanitizePath(oid);
  }

  auto coid = trim(child_object_id);
  if (!coid.empty()) {
    str += "/" + sanitizePath(coid);
  }
  str += "/" + sanitizePath(topicType(topic_type));
  return str;
}

std::string HaBridge::topicType(TopicType topic_type) const {
  switch (topic_type) {
  case TopicType::State:
    return "state";
  case TopicType::Command:
    return "command";
  case TopicType::Attributes:
    return "attributes";
  }

  return "unknown";
}
