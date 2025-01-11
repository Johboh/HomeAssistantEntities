#include "HaBridge.h"
#include <HaUtilities.h>

using namespace homeassistantentities;

HaBridge::HaBridge(IMQTTRemote &remote, std::string node_id, IJsonDocument &this_device_json_doc, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(this_device_json_doc) {}

void HaBridge::publishConfiguration(std::string component, std::string object_id, std::string child_object_id,
                                    const IJsonDocument &specific_doc) {
  IJsonDocument doc;
  doc["availability_topic"] = santitizePath(_remote.clientId()) + "/status";

  std::string unique_id = _remote.clientId() + "_" + _node_id;

  auto coid = homeassistantentities::trim(child_object_id);
  if (!coid.empty()) {
    unique_id += "_" + coid;
  }
  unique_id += "_" + object_id;
  doc["unique_id"] = unique_id;

  // Set optional device keys.
  for (auto kv : IJsonIteratorBegin(_this_device_json_doc)) {
    doc["device"][kv.key()] = kv.value();
  }

  for (auto kv : IJsonConstIteratorBegin(specific_doc)) {
    doc[kv.key()] = kv.value();
  }

  auto message = toJsonString(doc);
  std::string topic =
      "homeassistant/" + santitizePath(component) + "/" + santitizePath(_node_id) + "/" + santitizePath(object_id);
  if (!coid.empty()) {
    topic += "_" + coid;
  }
  topic += "/config";
  publishMessage(topic, message, true);
}

bool HaBridge::publishMessage(std::string topic, std::string message, bool retain) {
  if (_verbose) {
    return _remote.publishMessageVerbose(topic, message, retain);
  } else {
    return _remote.publishMessage(topic, message, retain);
  }
}

std::string HaBridge::getTopic(TopicType topic_type, std::string component, std::string object_id,
                               std::string child_object_id) {
  auto str = santitizePath(_node_id) + "/" + santitizePath(component) + "/" + santitizePath(object_id);

  auto coid = trim(child_object_id);
  if (!coid.empty()) {
    str += "/" + santitizePath(coid);
  }
  str += "/" + santitizePath(topicType(topic_type));
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