#include "HaBridge.h"

DynamicJsonDocument EMPTY_DOCUMENT(1);

HaBridge::HaBridge(IMQTTRemote &remote, String node_id, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(EMPTY_DOCUMENT) {}

HaBridge::HaBridge(IMQTTRemote &remote, JsonDocument &this_device_json_doc, String node_id, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(this_device_json_doc) {}

void HaBridge::publishConfiguration(String component, String object_id, String child_object_id,
                                    const JsonDocument &specific_doc) {
  DynamicJsonDocument doc(1024);
  doc["availability_topic"] = _remote.clientId() + "/status";

  String unique_id = _remote.clientId() + "_" + _node_id;
  child_object_id.trim();
  if (child_object_id.length() > 0) {
    unique_id += "_" + child_object_id;
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

  String message;
  serializeJson(doc, message);
  String topic = "homeassistant/" + component + "/" + _node_id + "/" + object_id;
  if (child_object_id.length() > 0) {
    topic += "_" + child_object_id;
  }
  topic += "/config";
  publishMessage(topic, message, true);
}

bool HaBridge::publishMessage(String topic, String message, bool retain) {
  if (_verbose) {
    return _remote.publishMessageVerbose(topic, message, retain);
  } else {
    return _remote.publishMessage(topic, message, retain);
  }
}

String HaBridge::getTopic(TopicType topic_type, String component, String object_id, String child_object_id) {
  auto str = _node_id + "/" + component + "/" + object_id;
  child_object_id.trim();
  if (child_object_id.length() > 0) {
    str += "/" + child_object_id;
  }
  str += "/" + topicType(topic_type);
  return str;
}

String HaBridge::topicType(TopicType topic_type) {
  switch (topic_type) {
  case TopicType::State:
    return "state";
  case TopicType::Command:
    return "command";
  }
  Serial.println("HaBridge: Unknown topic type!");
  return "unknown";
}