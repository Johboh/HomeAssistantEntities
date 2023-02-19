#ifndef __HA_ENTITY_RETRIES_H__
#define __HA_ENTITY_RETRIES_H__

#include "HaBridge.h"
#include "HaEntity.h"
#include <Arduino.h>

/**
 * @brief Represent a retries sensor. BUT! At this point there is no Home Assistant configuration, just the value.
 */
class HaEntityRetries : public HaEntity {
public:
  /**
   * @brief Construct a new Ha Entity Retries object
   */
  HaEntityRetries(HaBridge &ha_bridge);

public:
  void publishConfiguration() override;
  void republishState() override;

  /**
   * @brief Publish the retries.
   *
   * @param retries number of retries for last received ESP-NOW message.
   */
  void publishRetries(uint8_t retries);

private:
  HaBridge &_ha_bridge;
};

#endif // __HA_ENTITY_RETRIES_H__