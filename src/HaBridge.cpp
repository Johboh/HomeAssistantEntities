#include "HaBridge.h"
#include <HaUtilities.h>

DynamicJsonDocument EMPTY_DOCUMENT(1);

HaBridge::HaBridge(IMQTTRemote &remote, std::string node_id, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(EMPTY_DOCUMENT) {}

HaBridge::HaBridge(IMQTTRemote &remote, JsonDocument &this_device_json_doc, std::string node_id, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(this_device_json_doc) {}

void HaBridge::publishConfiguration(std::string component, std::string object_id, std::string child_object_id,
                                    const JsonDocument &specific_doc) {
  DynamicJsonDocument doc(1024);
  doc["availability_topic"] = _remote.clientId() + "/status";

  std::string unique_id = _remote.clientId() + "_" + _node_id;

  auto coid = homeassistantentities::trim(child_object_id);
  if (coid.length() > 0) {
    unique_id += "_" + coid;
  }
  unique_id += "_" + object_id;
  doc["unique_id"] = unique_id;

  // Set optional device keys.
  if (!_this_device_json_doc.isNull()) {
    auto device = doc.createNestedObject("device");
    for (JsonPair kvp : _this_device_json_doc.as<JsonObject>()) {
      device[kvp.key()] = kvp.value();
    }
  }

  // Set specific keys.
  for (JsonPairConst kvp : specific_doc.as<JsonObjectConst>()) {
    doc[kvp.key()] = kvp.value();
  }

  std::string message;
  serializeJson(doc, message);
  std::string topic = "homeassistant/" + component + "/" + _node_id + "/" + object_id;
  if (coid.length() > 0) {
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
  auto str = _node_id + "/" + component + "/" + object_id;

  auto coid = homeassistantentities::trim(child_object_id);
  if (coid.length() > 0) {
    str += "/" + coid;
  }
  str += "/" + topicType(topic_type);
  return str;
}

std::string HaBridge::topicType(TopicType topic_type) {
  switch (topic_type) {
  case TopicType::State:
    return "state";
  case TopicType::Command:
    return "command";
  }
  Serial.println("HaBridge: Unknown topic type!");
  return "unknown";
}