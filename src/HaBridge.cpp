#include "HaBridge.h"
#include <HaUtilities.h>

HaBridge::HaBridge(IMQTTRemote &remote, std::string node_id, nlohmann::json &this_device_json_doc, bool verbose)
    : _verbose(verbose), _node_id(node_id), _remote(remote), _this_device_json_doc(this_device_json_doc) {}

void HaBridge::publishConfiguration(std::string component, std::string object_id, std::string child_object_id,
                                    const nlohmann::json &specific_doc) {
  nlohmann::json doc;
  doc["availability_topic"] = _remote.clientId() + "/status";

  std::string unique_id = _remote.clientId() + "_" + _node_id;

  auto coid = homeassistantentities::trim(child_object_id);
  if (coid.length() > 0) {
    unique_id += "_" + coid;
  }
  unique_id += "_" + object_id;
  doc["unique_id"] = unique_id;

  // Set optional device keys.
  for (nlohmann::json::iterator it = _this_device_json_doc.begin(); it != _this_device_json_doc.end(); ++it) {
    doc["device"][it.key()] = it.value();
  }

  // Set specific keys.
  for (nlohmann::json::const_iterator it = specific_doc.begin(); it != specific_doc.end(); ++it) {
    doc[it.key()] = it.value();
  }

  auto message = doc.dump();
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
  case TopicType::Attributes:
    return "attributes";
  }

  return "unknown";
}