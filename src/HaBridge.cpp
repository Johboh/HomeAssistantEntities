#include "HaBridge.h"
#include <utility>

using namespace homeassistantentities;

HaBridge::HaBridge(IMQTTRemote &remote, std::string node_id, IJsonDocument &this_device_json_doc, bool verbose,
                   std::function<std::string(IMQTTRemote &)> availability_topic,
                   std::function<std::string(IMQTTRemote &, std::string &)> unique_id)
    : _verbose(verbose), _node_id(std::move(node_id)), _remote(remote), _this_device_json_doc(this_device_json_doc),
      _availability_topic(std::move(availability_topic)), _unique_id(std::move(unique_id)) {}

void HaBridge::publishConfiguration(const std::string &component, const std::string &object_id,
                                    const std::string &child_object_id, const IJsonDocument &specific_doc) {
  IJsonDocument doc;
  auto coid = homeassistantentities::trim(child_object_id);

  // Home Assistant's device_automation (device trigger) schema forbids availability_topic and unique_id.
  if (component != "device_automation") {
    doc["availability_topic"] =
        _availability_topic ? _availability_topic(_remote) : (sanitizePath(_remote.clientId()) + "/status");

    std::string unique_id = _unique_id ? _unique_id(_remote, _node_id) : (_remote.clientId() + "_" + _node_id);
    if (!coid.empty()) {
      unique_id += "_" + sanitizePath(coid);
    }
    unique_id += "_" + sanitizePath(object_id);
    doc["unique_id"] = unique_id;
  }

  // Set optional device keys.
  for (auto kv : IJsonIteratorBegin(_this_device_json_doc)) {
    doc["device"][kv.key()] = kv.value();
  }

  for (auto kv : IJsonConstIteratorBegin(specific_doc)) {
    doc[kv.key()] = kv.value();
  }

  auto message = toJsonString(doc);
  std::string topic =
      "homeassistant/" + sanitizePath(component) + "/" + sanitizePath(_node_id) + "/" + sanitizePath(object_id);
  if (!coid.empty()) {
    topic += "_" + sanitizePath(coid);
  }
  topic += "/config";
  publishMessage(topic, message, true);
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

std::string HaBridge::topicType(TopicType topic_type) {
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