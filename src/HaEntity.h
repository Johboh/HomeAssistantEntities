#ifndef __HA_ENTITY_H__
#define __HA_ENTITY_H__

/**
 * @brief Abstract for a HaEntity.
 *
 * An entity is the sensor, binary sensor, actuator etc that will show up in Home Assistant. If a sensor, Home Assistant
 * can show/use the sensor value. If an actuator, Home Assistant can send states to the entity.
 * An entity has a configuration published to the Home Assistant MQTT topic (directly under MQTT "homeassistant/") which
 * Home Assistant is subscribed to, and where it collect all its MQTT Sensors, Binary Sensors, Actuators and so on. A
 * configuration always have a setup phase to publish to the above mentioned topic, but also a specific method for
 * publishing value to the state topic, if any. Or to read from a state topic.
 *
 * A HaEntity is only an Home Assistant abstraction, and does not interact with any hardware layer. an HaEntity should
 * only provide a method to setup and publish any values, or callbacks one can subscribe to to receive values.
 */
class HaEntity {
public:
  /**
   * @brief Publish the Home Assistant configuration.
   */
  virtual void publishConfiguration() = 0;

  /**
   * @brief Republish any previous published state on any of the available state topics.
   */
  virtual void republishState() = 0;
};

#endif // __HA_ENTITY_H__