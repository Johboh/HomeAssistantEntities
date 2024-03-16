#ifndef __I_MQTT_REMOTE_H__
#define __I_MQTT_REMOTE_H__

/**
 * Copy paste from https://github.com/Johboh/MQTTRemote/blob/main/includes/IMQTTRemote.h
 * This is to avoid a direct dependency on Johboh/MQTTRemote library, as the user can implement
 * any MQTT client they want (or use Johboh/MQTTRemote).
 */

#include <cstdint>
#include <functional>
#include <string>

/**
 * @brief Inteface to separate the concerns between the actual MQTT implementation that has a lifecycle, connection
 * handling, and consumers who only need the API itself.
 */
class IMQTTRemote {
public:
  // First parameter is topic, second one is the message.
  typedef std::function<void(std::string, std::string)> SubscriptionCallback;

  /**
   * @brief Publish a message.
   *
   * @param topic the topic to publish to.
   * @param message The message to send. This cannot be larger than the value set for max_message_size in the
   * constructor.
   * @param retain True to set this message as retained.
   * @param qos quality of service for published message (0 (default), 1 or 2)
   * @returns true on success, or false on failure.
   */
  virtual bool publishMessage(std::string topic, std::string message, bool retain = false, uint8_t qos = 0) = 0;

  /**
   * Same as publishMessage(), but will print the message and topic and the result in console.
   */
  virtual bool publishMessageVerbose(std::string topic, std::string message, bool retain = false, uint8_t qos = 0) = 0;

  /**
   * @brief Subscribe to a topic. The callback will be invoked on every new message.
   * There can only be one callback per topic. If trying to subscribe to an already subscribe topic, it will be ignored.
   * Don't do have operations in the callback or delays as this will block the MQTT callback.
   * If not connected, will subscribe to this topic once connected.
   *
   * @param message_callback a message callback with the topic and the message. The topic is repeated for convinience,
   * but it will always be for the subscribed topic.
   */
  virtual bool subscribe(std::string topic, SubscriptionCallback message_callback) = 0;

  /**
   * @brief Unsubscribe a topic.
   */
  virtual bool unsubscribe(std::string topic) = 0;

  /**
   * @brief returns if there is a connection to the MQTT server.
   */
  virtual bool connected() = 0;

  /**
   * @brief The client ID for this device. This is used for the last will / status
   * topic.Example, if this is "esp_now_router", then the status/last will topic will be "esp_now_router/status". This
   * has to be [a-zA-Z0-9_] only.
   */
  virtual std::string &clientId() = 0;
};

#endif // __I_MQTT_REMOTE_H__