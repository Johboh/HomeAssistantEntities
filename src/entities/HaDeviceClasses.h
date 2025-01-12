#pragma once

#include "HaUtilities.h"
#include <optional>
#include <string>

namespace homeassistantentities {

using UnitType = int;

class DeviceClass {
public:
  enum class SensorType {
    Sensor,       // A sensor with a numeric or string value
    BinarySensor, // A sensor with a boolean value (ON/OFF)
  };

public:
  virtual ~DeviceClass() = default;
  virtual SensorType sensorType() const = 0;
  virtual std::optional<std::string> deviceClass() const = 0;
  virtual std::optional<std::string> unitOfMeasurement(UnitType unit) const = 0;

  virtual std::string objectId() const {
    auto device_class_opt = deviceClass();
    if (device_class_opt) {
      auto device_class = trim(*device_class_opt);
      if (!device_class.empty()) {
        return device_class;
      }
    }
    return "unknown_device_class";
  }
};

/**
 * @brief Device classes and their units from https://www.home-assistant.io/integrations/sensor/#device-class
 */
namespace Sensor {

/**
 * @brief Air Quality Index (unitless).
 */
class Aqi : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "aqi";
};

/**
 * @brief Area in m², cm², etc.
 */
class Area : public DeviceClass {
public:
  enum Unit : UnitType { m2 = 1, cm2, km2, mm2, in2, ft2, yd2, mi2, ac, ha };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case m2:
      return "m2";
    case cm2:
      return "cm2";
    case km2:
      return "km2";
    case mm2:
      return "mm2";
    case in2:
      return "in2";
    case ft2:
      return "ft2";
    case yd2:
      return "yd2";
    case mi2:
      return "mi2";
    case ac:
      return "ac";
    case ha:
      return "ha";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "area";
};

// Repeat this structure for other namespaces using their specific units and device classes

/**
 * @brief Atmospheric pressure in cbar, bar, etc.
 */
class AtmosphericPressure : public DeviceClass {
public:
  enum Unit : UnitType { cbar = 1, bar, hPa, mmHg, inHg, kPa, mbar, Pa, psi };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case cbar:
      return "cbar";
    case bar:
      return "bar";
    case hPa:
      return "hPa";
    case mmHg:
      return "mmHg";
    case inHg:
      return "inHg";
    case kPa:
      return "kPa";
    case mbar:
      return "mbar";
    case Pa:
      return "Pa";
    case psi:
      return "psi";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "atmospheric_pressure";
};

/**
 * @brief Battery percentage in %.
 */
class Battery : public DeviceClass {
public:
  enum Unit : UnitType { Percent = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Percent:
      return "%";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "battery";
};

/**
 * @brief Blood glucose concentration in mg/dL, mmol/L.
 */
class BloodGlucoseConcentration : public DeviceClass {
public:
  enum Unit : UnitType { mg_dL = 1, mmol_L };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case mg_dL:
      return "mg/dL";
    case mmol_L:
      return "mmol/L";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "blood_glucose_concentration";
};

/**
 * @brief Carbon Dioxide in ppm.
 */
class CarbonDioxide : public DeviceClass {
public:
  enum Unit : UnitType { ppm = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ppm:
      return "ppm";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "carbon_dioxide";
};

// CarbonMonoxide
class CarbonMonoxide : public DeviceClass {
public:
  enum Unit : UnitType { ppm = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ppm:
      return "ppm";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "carbon_monoxide";
};

// Current
class Current : public DeviceClass {
public:
  enum Unit : UnitType { A = 1, mA };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case A:
      return "A";
    case mA:
      return "mA";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "current";
};

// DataRate
class DataRate : public DeviceClass {
public:
  enum Unit : UnitType { bits = 1, kbits, Mbits, Gbits, Bps, kBps, MBps, GBps, KiBps, MiBps, GiBps };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case bits:
      return "bit/s";
    case kbits:
      return "kbit/s";
    case Mbits:
      return "Mbit/s";
    case Gbits:
      return "Gbit/s";
    case Bps:
      return "B/s";
    case kBps:
      return "kB/s";
    case MBps:
      return "MB/s";
    case GBps:
      return "GB/s";
    case KiBps:
      return "KiB/s";
    case MiBps:
      return "MiB/s";
    case GiBps:
      return "GiB/s";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "data_rate";
};

// DataSize
class DataSize : public DeviceClass {
public:
  enum Unit : UnitType {
    bits = 1,
    kbits,
    Mbits,
    Gbits,
    B,
    kB,
    MB,
    GB,
    TB,
    PB,
    EB,
    ZB,
    YB,
    KiB,
    MiB,
    GiB,
    TiB,
    PiB,
    EiB,
    ZiB,
    YiB
  };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case bits:
      return "bit";
    case kbits:
      return "kbit";
    case Mbits:
      return "Mbit";
    case Gbits:
      return "Gbit";
    case B:
      return "B";
    case kB:
      return "kB";
    case MB:
      return "MB";
    case GB:
      return "GB";
    case TB:
      return "TB";
    case PB:
      return "PB";
    case EB:
      return "EB";
    case ZB:
      return "ZB";
    case YB:
      return "YB";
    case KiB:
      return "KiB";
    case MiB:
      return "MiB";
    case GiB:
      return "GiB";
    case TiB:
      return "TiB";
    case PiB:
      return "PiB";
    case EiB:
      return "EiB";
    case ZiB:
      return "ZiB";
    case YiB:
      return "YiB";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "data_size";
};

// Date
class Date : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "date";
};

// Distance
class Distance : public DeviceClass {
public:
  enum Unit : UnitType { km = 1, m, cm, mm, mi, nmi, yd, in };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case km:
      return "km";
    case m:
      return "m";
    case cm:
      return "cm";
    case mm:
      return "mm";
    case mi:
      return "mi";
    case nmi:
      return "nmi";
    case yd:
      return "yd";
    case in:
      return "in";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "distance";
};

// Duration
class Duration : public DeviceClass {
public:
  enum Unit : UnitType { d = 1, h, min, s, ms };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case d:
      return "d";
    case h:
      return "h";
    case min:
      return "min";
    case s:
      return "s";
    case ms:
      return "ms";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "duration";
};

// Energy
class Energy : public DeviceClass {
public:
  enum Unit : UnitType { J = 1, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case J:
      return "J";
    case kJ:
      return "kJ";
    case MJ:
      return "MJ";
    case GJ:
      return "GJ";
    case mWh:
      return "mWh";
    case Wh:
      return "Wh";
    case kWh:
      return "kWh";
    case MWh:
      return "MWh";
    case GWh:
      return "GWh";
    case TWh:
      return "TWh";
    case cal:
      return "cal";
    case kcal:
      return "kcal";
    case Mcal:
      return "Mcal";
    case Gcal:
      return "Gcal";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "energy";
};

// EnergyStorage
class EnergyStorage : public DeviceClass {
public:
  enum Unit : UnitType { J = 1, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case J:
      return "J";
    case kJ:
      return "kJ";
    case MJ:
      return "MJ";
    case GJ:
      return "GJ";
    case mWh:
      return "mWh";
    case Wh:
      return "Wh";
    case kWh:
      return "kWh";
    case MWh:
      return "MWh";
    case GWh:
      return "GWh";
    case TWh:
      return "TWh";
    case cal:
      return "cal";
    case kcal:
      return "kcal";
    case Mcal:
      return "Mcal";
    case Gcal:
      return "Gcal";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "energy_storage";
};

// Enum
class Enum : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "enum";
};

// Frequency
class Frequency : public DeviceClass {
public:
  enum Unit : UnitType { Hz = 1, kHz, MHz, GHz };

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Hz:
      return "Hz";
    case kHz:
      return "kHz";
    case MHz:
      return "MHz";
    case GHz:
      return "GHz";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "frequency";
};

// Gas
class Gas : public DeviceClass {
public:
  enum Unit : UnitType { m3 = 1, ft3, CCF };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case m3:
      return "m³";
    case ft3:
      return "ft³";
    case CCF:
      return "CCF";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "gas";
};

// Humidity
class Humidity : public DeviceClass {
public:
  enum Unit : UnitType { Percent = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Percent:
      return "%";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "humidity";
};

// Illuminance
class Illuminance : public DeviceClass {
public:
  enum Unit : UnitType { lx = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case lx:
      return "lx";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "illuminance";
};

// Irradiance
class Irradiance : public DeviceClass {
public:
  enum Unit : UnitType { W_m2 = 1, BTU_ft2_h };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case W_m2:
      return "W/m²";
    case BTU_ft2_h:
      return "BTU/(h⋅ft²)";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "irradiance";
};

// Moisture
class Moisture : public DeviceClass {
public:
  enum Unit : UnitType { Percent = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Percent:
      return "%";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "moisture";
};

// Monetary
class Monetary : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "monetary";
};

// NitrogenDioxide
class NitrogenDioxide : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "nitrogen_dioxide";
};

// NitrogenMonoxide
class NitrogenMonoxide : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "nitrogen_monoxide";
};

// NitrousOxide
class NitrousOxide : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "nitrous_oxide";
};

// Ozone
class Ozone : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "ozone";
};

// Ph
class Ph : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "ph";
};

// Pm1
class Pm1 : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "pm1";
};

// Pm25
class Pm25 : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "pm25";
};

// Pm10
class Pm10 : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "pm10";
};

// PowerFactor
class PowerFactor : public DeviceClass {
public:
  enum Unit : UnitType { None = 1, Percent };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case None:
      return std::nullopt;
    case Percent:
      return "%";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "power_factor";
};

// Power
class Power : public DeviceClass {
public:
  enum Unit : UnitType { mW = 1, W, kW, MW, GW, TW };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case mW:
      return "mW";
    case W:
      return "W";
    case kW:
      return "kW";
    case MW:
      return "MW";
    case GW:
      return "GW";
    case TW:
      return "TW";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "power";
};

// Precipitation
class Precipitation : public DeviceClass {
public:
  enum Unit : UnitType { cm = 1, in, mm };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case cm:
      return "cm";
    case in:
      return "in";
    case mm:
      return "mm";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "precipitation";
};

// PrecipitationIntensity
class PrecipitationIntensity : public DeviceClass {
public:
  enum Unit : UnitType { in_d = 1, in_h, mm_d, mm_h };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case in_d:
      return "in/d";
    case in_h:
      return "in/h";
    case mm_d:
      return "mm/d";
    case mm_h:
      return "mm/h";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "precipitation_intensity";
};

// Pressure
class Pressure : public DeviceClass {
public:
  enum Unit : UnitType { Pa = 1, kPa, hPa, bar, cbar, mbar, mmHg, inHg, psi };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Pa:
      return "Pa";
    case kPa:
      return "kPa";
    case hPa:
      return "hPa";
    case bar:
      return "bar";
    case cbar:
      return "cbar";
    case mbar:
      return "mbar";
    case mmHg:
      return "mmHg";
    case inHg:
      return "inHg";
    case psi:
      return "psi";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "pressure";
};

// ReactivePower
class ReactivePower : public DeviceClass {
public:
  enum Unit : UnitType { var = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case var:
      return "var";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "reactive_power";
};

// SignalStrength
class SignalStrength : public DeviceClass {
public:
  enum Unit : UnitType { dB = 1, dBm };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case dB:
      return "dB";
    case dBm:
      return "dBm";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "signal_strength";
};

// SoundPressure
class SoundPressure : public DeviceClass {
public:
  enum Unit : UnitType { dB = 1, dBA };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case dB:
      return "dB";
    case dBA:
      return "dBA";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "sound_pressure";
};

// Speed
class Speed : public DeviceClass {
public:
  enum Unit : UnitType { ft_s = 1, in_d, in_h, in_s, km_h, kn, m_s, mph, mm_d, mm_s };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ft_s:
      return "ft/s";
    case in_d:
      return "in/d";
    case in_h:
      return "in/h";
    case in_s:
      return "in/s";
    case km_h:
      return "km/h";
    case kn:
      return "kn";
    case m_s:
      return "m/s";
    case mph:
      return "mph";
    case mm_d:
      return "mm/d";
    case mm_s:
      return "mm/s";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "speed";
};

// SulphurDioxide
class SulphurDioxide : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "sulphur_dioxide";
};

// Temperature
class Temperature : public DeviceClass {
public:
  enum Unit : UnitType { C = 1, F, K };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case C:
      return "°C";
    case F:
      return "°F";
    case K:
      return "K";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "temperature";
};

// Timestamp
class Timestamp : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "timestamp";
};

// VolatileOrganicCompounds
class VolatileOrganicCompounds : public DeviceClass {
public:
  enum Unit : UnitType { ug_m3 = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ug_m3:
      return "µg/m³";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "volatile_organic_compounds";
};

// VolatileOrganicCompoundsParts
class VolatileOrganicCompoundsParts : public DeviceClass {
public:
  enum Unit : UnitType { ppm = 1, ppb };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case ppm:
      return "ppm";
    case ppb:
      return "ppb";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "volatile_organic_compounds_parts";
};

// Voltage
class Voltage : public DeviceClass {
public:
  enum Unit : UnitType { V = 1, mV, uV };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case V:
      return "V";
    case mV:
      return "mV";
    case uV:
      return "µV";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "voltage";
};

// Volume
class Volume : public DeviceClass {
public:
  enum Unit : UnitType { L = 1, mL, gal, fl_oz, m3, ft3, CCF };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case L:
      return "L";
    case mL:
      return "mL";
    case gal:
      return "gal";
    case fl_oz:
      return "fl. oz.";
    case m3:
      return "m³";
    case ft3:
      return "ft³";
    case CCF:
      return "CCF";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "volume";
};

// VolumeFlowRate
class VolumeFlowRate : public DeviceClass {
public:
  enum Unit : UnitType { m3_h = 1, ft3_min, L_min, gal_min, mL_s };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case m3_h:
      return "m³/h";
    case ft3_min:
      return "ft³/min";
    case L_min:
      return "L/min";
    case gal_min:
      return "gal/min";
    case mL_s:
      return "mL/s";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "volume_flow_rate";
};

// VolumeStorage
class VolumeStorage : public DeviceClass {
public:
  enum Unit : UnitType { L = 1, mL, gal, fl_oz, m3, ft3, CCF };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case L:
      return "L";
    case mL:
      return "mL";
    case gal:
      return "gal";
    case fl_oz:
      return "fl. oz.";
    case m3:
      return "m³";
    case ft3:
      return "ft³";
    case CCF:
      return "CCF";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "volume_storage";
};

// Water
class Water : public DeviceClass {
public:
  enum Unit : UnitType { L = 1, gal, m3, ft3, CCF };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case L:
      return "L";
    case gal:
      return "gal";
    case m3:
      return "m³";
    case ft3:
      return "ft³";
    case CCF:
      return "CCF";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "water";
};

// Weight
class Weight : public DeviceClass {
public:
  enum Unit : UnitType { kg = 1, g, mg, ug, oz, lb, st };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case kg:
      return "kg";
    case g:
      return "g";
    case mg:
      return "mg";
    case ug:
      return "µg";
    case oz:
      return "oz";
    case lb:
      return "lb";
    case st:
      return "st";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "weight";
};

// WindSpeed
class WindSpeed : public DeviceClass {
public:
  enum Unit : UnitType { Beaufort = 1, ft_s, km_h, kn, m_s, mph };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Beaufort:
      return "Beaufort";
    case ft_s:
      return "ft/s";
    case km_h:
      return "km/h";
    case kn:
      return "kn";
    case m_s:
      return "m/s";
    case mph:
      return "mph";
    default:
      return std::nullopt;
    }
  }

private:
  static constexpr const char *DEVICE_CLASS = "wind_speed";
};

/**
 * @brief These are unofficial or non existing device classes, used for convenience or for default.
 *
 */
namespace Undefined {

class Empty : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }
};

class Brightness : public DeviceClass {
public:
  enum Unit : UnitType { Percent = 1 };

  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; } // No brightness device class exists.

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case Percent:
      return "%";
    default:
      return std::nullopt;
    }
  }

  std::string objectId() const override { return "brightness"; }
};

/**
 * @brief Particles/Units/things per volume.
 */
class UnitConcentration : public DeviceClass {
public:
  enum Unit : UnitType { mL = 1, dL, L };

  SensorType sensorType() const override { return SensorType::Sensor; }

  // No unit concentration device class exists.
  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override {
    switch (unit) {
    case mL:
      return "/mL";
    case dL:
      return "/dL";
    case L:
      return "/L";
    default:
      return std::nullopt;
    }
  }

  std::string objectId() const override { return "unit_concentration"; }
};

class Json : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

  std::string objectId() const override { return "json"; }
};

class String : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::Sensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

  std::string objectId() const override { return "string"; }
};

}; // namespace Undefined

}; // namespace Sensor

/**
 * @brief Device classes and their units from https://www.home-assistant.io/integrations/binary_sensor/#device-class
 */
namespace BinarySensor {

/**
 * @brief on means low, off means normal
 */
class Battery : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "battery";
};

/**
 * @brief on means charging, off means not charging
 */
class BatteryCharging : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "battery_charging";
};

/**
 * @brief on means carbon monoxide detected, off means clear
 */
class CarbonMonoxide : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "carbon_monoxide";
};

/**
 * @brief on means cold, off means normal
 */
class Cold : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "cold";
};

/**
 * @brief on means connected, off means disconnected
 */
class Connectivity : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "connectivity";
};

/**
 * @brief on means open, off means closed
 */
class Door : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "door";
};

/**
 * @brief on means open, off means closed
 */
class GarageDoor : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "garage_door";
};

/**
 * @brief on means gas detected, off means clear
 */
class Gas : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "gas";
};

/**
 * @brief on means hot, off means normal
 */
class Heat : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "heat";
};

/**
 * @brief on means light detected, off means no light
 */
class Light : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "light";
};

/**
 * @brief on means open (unlocked), off means closed (locked)
 */
class Lock : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "lock";
};

/**
 * @brief on means moisture detected (wet), off means no moisture (dry)
 */
class Moisture : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "moisture";
};

/**
 * @brief on means motion detected, off means clear
 */
class Motion : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "motion";
};

/**
 * @brief on means moving, off means stopped
 */
class Moving : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "moving";
};

/**
 * @brief on means occupied (detected), off means clear
 */
class Occupancy : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "occupancy";
};

/**
 * @brief on means open, off means closed
 */
class Opening : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "opening";
};

/**
 * @brief on means device is plugged in, off means device is unplugged
 */
class Plug : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "plug";
};

/**
 * @brief on means power detected, off means no power
 */
class Power : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "power";
};

/**
 * @brief on means home, off means away
 */
class Presence : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "presence";
};

/**
 * @brief on means problem detected, off means OK
 */
class Problem : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "problem";
};

/**
 * @brief on means running, off means not running
 */
class Running : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "running";
};

/**
 * @brief on means unsafe, off means safe
 */
class Safety : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "safety";
};

/**
 * @brief on means smoke detected, off means clear
 */
class Smoke : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "smoke";
};

/**
 * @brief on means sound detected, off means clear
 */
class Sound : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "sound";
};

/**
 * @brief on means tampering detected, off means clear
 */
class Tamper : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "tamper";
};

/**
 * @brief on means update available, off means up-to-date
 */
class Update : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "update";
};

/**
 * @brief on means vibration detected, off means clear
 */
class Vibration : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "vibration";
};

/**
 * @brief on means open, off means closed
 */
class Window : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return DEVICE_CLASS; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

private:
  static constexpr const char *DEVICE_CLASS = "window";
};

/**
 * @brief These are unofficial or non existing device classes, used for convenience or for default.
 *
 */
namespace Undefined {

class Empty : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }
};

class Boolean : public DeviceClass {
public:
  SensorType sensorType() const override { return SensorType::BinarySensor; }

  std::optional<std::string> deviceClass() const override { return std::nullopt; }

  std::optional<std::string> unitOfMeasurement(UnitType unit) const override { return std::nullopt; }

  std::string objectId() const override { return "boolean"; }
};

}; // namespace Undefined

}; // namespace BinarySensor
}; // namespace homeassistantentities