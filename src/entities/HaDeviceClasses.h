#pragma once

#include <optional>
#include <string>

namespace homeassistantentities {

namespace Sensor {

/**
 * @brief Device classes and their units from https://www.home-assistant.io/integrations/sensor/#device-class
 */
namespace DeviceClass {

/**
 * @brief Apparent power in VA.
 */
namespace ApparentPower {
const std::string DEVICE_CLASS = "apparent_power";
enum class Unit { VA };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::VA:
    return "VA";
  }
  return std::nullopt;
}
}; // namespace ApparentPower

/**
 * @brief Air Quality Index (unitless).
 */
namespace Aqi {
const std::string DEVICE_CLASS = "aqi";

enum class Unit {};

inline std::optional<std::string> unit_of_measurement(const Unit &unit) { return std::nullopt; }
}; // namespace Aqi

/**
 * @brief Area in m², cm², etc.
 */
namespace Area {
const std::string DEVICE_CLASS = "area";

enum class Unit { m2, cm2, km2, mm2, in2, ft2, yd2, mi2, ac, ha };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::m2:
    return "m2";
  case Unit::cm2:
    return "cm2";
  case Unit::km2:
    return "km2";
  case Unit::mm2:
    return "mm2";
  case Unit::in2:
    return "in2";
  case Unit::ft2:
    return "ft2";
  case Unit::yd2:
    return "yd2";
  case Unit::mi2:
    return "mi2";
  case Unit::ac:
    return "ac";
  case Unit::ha:
    return "ha";
  }
  return std::nullopt;
}
}; // namespace Area

/**
 * @brief Atmospheric pressure in cbar, bar, etc.
 */
namespace AtmosphericPressure {
const std::string DEVICE_CLASS = "atmospheric_pressure";

enum class Unit { cbar, bar, hPa, mmHg, inHg, kPa, mbar, Pa, psi };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::cbar:
    return "cbar";
  case Unit::bar:
    return "bar";
  case Unit::hPa:
    return "hPa";
  case Unit::mmHg:
    return "mmHg";
  case Unit::inHg:
    return "inHg";
  case Unit::kPa:
    return "kPa";
  case Unit::mbar:
    return "mbar";
  case Unit::Pa:
    return "Pa";
  case Unit::psi:
    return "psi";
  }
  return std::nullopt;
}
}; // namespace AtmosphericPressure

/**
 * @brief Battery percentage in %.
 */
namespace Battery {
const std::string DEVICE_CLASS = "battery";

enum class Unit { Percent };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Percent:
    return "%";
  }
  return std::nullopt;
}
}; // namespace Battery

/**
 * @brief Blood glucose concentration in mg/dL, mmol/L.
 */
namespace BloodGlucoseConcentration {
const std::string DEVICE_CLASS = "blood_glucose_concentration";

enum class Unit { mg_dL, mmol_L };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::mg_dL:
    return "mg/dL";
  case Unit::mmol_L:
    return "mmol/L";
  }
  return std::nullopt;
}
}; // namespace BloodGlucoseConcentration

/**
 * @brief Carbon Dioxide in ppm.
 */
namespace CarbonDioxide {
const std::string DEVICE_CLASS = "carbon_dioxide";

enum class Unit { ppm };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ppm:
    return "ppm";
  }
  return std::nullopt;
}
}; // namespace CarbonDioxide

/**
 * @brief Carbon Monoxide in ppm.
 */
namespace CarbonMonoxide {
const std::string DEVICE_CLASS = "carbon_monoxide";

enum class Unit { ppm };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ppm:
    return "ppm";
  }
  return std::nullopt;
}
}; // namespace CarbonMonoxide

/**
 * @brief Current in A, mA.
 */
namespace Current {
const std::string DEVICE_CLASS = "current";

enum class Unit { A, mA };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::A:
    return "A";
  case Unit::mA:
    return "mA";
  }
  return std::nullopt;
}
}; // namespace Current

/**
 * @brief Data rate in bit/s, kbit/s, etc.
 */
namespace DataRate {
const std::string DEVICE_CLASS = "data_rate";

enum class Unit { bits, kbits, Mbits, Gbits, Bps, kBps, MBps, GBps, KiBps, MiBps, GiBps };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::bits:
    return "bit/s";
  case Unit::kbits:
    return "kbit/s";
  case Unit::Mbits:
    return "Mbit/s";
  case Unit::Gbits:
    return "Gbit/s";
  case Unit::Bps:
    return "B/s";
  case Unit::kBps:
    return "kB/s";
  case Unit::MBps:
    return "MB/s";
  case Unit::GBps:
    return "GB/s";
  case Unit::KiBps:
    return "KiB/s";
  case Unit::MiBps:
    return "MiB/s";
  case Unit::GiBps:
    return "GiB/s";
  }
  return std::nullopt;
}
}; // namespace DataRate

/**
 * @brief Data size in bit, kbit, etc.
 */
namespace DataSize {
const std::string DEVICE_CLASS = "data_size";

enum class Unit {
  bits,
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

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::bits:
    return "bit";
  case Unit::kbits:
    return "kbit";
  case Unit::Mbits:
    return "Mbit";
  case Unit::Gbits:
    return "Gbit";
  case Unit::B:
    return "B";
  case Unit::kB:
    return "kB";
  case Unit::MB:
    return "MB";
  case Unit::GB:
    return "GB";
  case Unit::TB:
    return "TB";
  case Unit::PB:
    return "PB";
  case Unit::EB:
    return "EB";
  case Unit::ZB:
    return "ZB";
  case Unit::YB:
    return "YB";
  case Unit::KiB:
    return "KiB";
  case Unit::MiB:
    return "MiB";
  case Unit::GiB:
    return "GiB";
  case Unit::TiB:
    return "TiB";
  case Unit::PiB:
    return "PiB";
  case Unit::EiB:
    return "EiB";
  case Unit::ZiB:
    return "ZiB";
  case Unit::YiB:
    return "YiB";
  }
  return std::nullopt;
}
}; // namespace DataSize

/**
 * @brief Date string (ISO 8601).
 */
namespace Date {
const std::string DEVICE_CLASS = "date";

enum class Unit { ISO8601 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  // Since the date is typically represented as a string, there is no unit to return.
  return std::nullopt;
}
}; // namespace Date

/**
 * @brief Distance in km, m, etc.
 */
namespace Distance {
const std::string DEVICE_CLASS = "distance";

enum class Unit { km, m, cm, mm, mi, nmi, yd, in };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::km:
    return "km";
  case Unit::m:
    return "m";
  case Unit::cm:
    return "cm";
  case Unit::mm:
    return "mm";
  case Unit::mi:
    return "mi";
  case Unit::nmi:
    return "nmi";
  case Unit::yd:
    return "yd";
  case Unit::in:
    return "in";
  }
  return std::nullopt;
}
}; // namespace Distance

/**
 * @brief Duration in d, h, etc.
 */
namespace Duration {
const std::string DEVICE_CLASS = "duration";

enum class Unit { d, h, min, s, ms };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::d:
    return "d";
  case Unit::h:
    return "h";
  case Unit::min:
    return "min";
  case Unit::s:
    return "s";
  case Unit::ms:
    return "ms";
  }
  return std::nullopt;
}
}; // namespace Duration

/**
 * @brief Energy in various units.
 */
namespace Energy {
const std::string DEVICE_CLASS = "energy";

enum class Unit { J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::J:
    return "J";
  case Unit::kJ:
    return "kJ";
  case Unit::MJ:
    return "MJ";
  case Unit::GJ:
    return "GJ";
  case Unit::mWh:
    return "mWh";
  case Unit::Wh:
    return "Wh";
  case Unit::kWh:
    return "kWh";
  case Unit::MWh:
    return "MWh";
  case Unit::GWh:
    return "GWh";
  case Unit::TWh:
    return "TWh";
  case Unit::cal:
    return "cal";
  case Unit::kcal:
    return "kcal";
  case Unit::Mcal:
    return "Mcal";
  case Unit::Gcal:
    return "Gcal";
  }
  return std::nullopt;
}
}; // namespace Energy

/**
 * @brief Energy storage in various units.
 */
namespace EnergyStorage {
const std::string DEVICE_CLASS = "energy_storage";

enum class Unit { J, kJ, MJ, GJ, mWh, Wh, kWh, MWh, GWh, TWh, cal, kcal, Mcal, Gcal };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::J:
    return "J";
  case Unit::kJ:
    return "kJ";
  case Unit::MJ:
    return "MJ";
  case Unit::GJ:
    return "GJ";
  case Unit::mWh:
    return "mWh";
  case Unit::Wh:
    return "Wh";
  case Unit::kWh:
    return "kWh";
  case Unit::MWh:
    return "MWh";
  case Unit::GWh:
    return "GWh";
  case Unit::TWh:
    return "TWh";
  case Unit::cal:
    return "cal";
  case Unit::kcal:
    return "kcal";
  case Unit::Mcal:
    return "Mcal";
  case Unit::Gcal:
    return "Gcal";
  }
  return std::nullopt;
}
}; // namespace EnergyStorage

/**
 * @brief Enum for a limited set of states.
 */
namespace Enum {
const std::string DEVICE_CLASS = "enum";

// This is typically application-specific, so no generic unit here.
inline std::optional<std::string> unit_of_measurement() { return std::nullopt; }
}; // namespace Enum

/**
 * @brief Frequency in Hz, kHz, etc.
 */
namespace Frequency {
const std::string DEVICE_CLASS = "frequency";

enum class Unit { Hz, kHz, MHz, GHz };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Hz:
    return "Hz";
  case Unit::kHz:
    return "kHz";
  case Unit::MHz:
    return "MHz";
  case Unit::GHz:
    return "GHz";
  }
  return std::nullopt;
}
}; // namespace Frequency

/**
 * @brief Gas volume in m³, ft³, or CCF.
 */
namespace Gas {
const std::string DEVICE_CLASS = "gas";

enum class Unit { m3, ft3, CCF };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::m3:
    return "m³";
  case Unit::ft3:
    return "ft³";
  case Unit::CCF:
    return "CCF";
  }
  return std::nullopt;
}
}; // namespace Gas

/**
 * @brief Humidity percentage in %.
 */
namespace Humidity {
const std::string DEVICE_CLASS = "humidity";

enum class Unit { Percent };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Percent:
    return "%";
  }
  return std::nullopt;
}
}; // namespace Humidity

/**
 * @brief Illuminance in lx.
 */
namespace Illuminance {
const std::string DEVICE_CLASS = "illuminance";

enum class Unit { lx };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::lx:
    return "lx";
  }
  return std::nullopt;
}
}; // namespace Illuminance

/**
 * @brief Irradiance in W/m² or BTU/(h⋅ft²).
 */
namespace Irradiance {
const std::string DEVICE_CLASS = "irradiance";

enum class Unit { W_m2, BTU_ft2_h };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::W_m2:
    return "W/m²";
  case Unit::BTU_ft2_h:
    return "BTU/(h⋅ft²)";
  }
  return std::nullopt;
}
}; // namespace Irradiance

/**
 * @brief Moisture percentage in %.
 */
namespace Moisture {
const std::string DEVICE_CLASS = "moisture";

enum class Unit { Percent };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Percent:
    return "%";
  }
  return std::nullopt;
}
}; // namespace Moisture

/**
 * @brief Monetary value (ISO 4217).
 */
namespace Monetary {
const std::string DEVICE_CLASS = "monetary";

// The currency code (ISO 4217) should be handled by the application context.
// No specific unit to return here, as it varies depending on the specific currency.
inline std::optional<std::string> unit_of_measurement() { return std::nullopt; }
}; // namespace Monetary

/**
 * @brief Nitrogen Dioxide concentration in µg/m³.
 */
namespace NitrogenDioxide {
const std::string DEVICE_CLASS = "nitrogen_dioxide";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace NitrogenDioxide

/**
 * @brief Nitrogen Monoxide concentration in µg/m³.
 */
namespace NitrogenMonoxide {
const std::string DEVICE_CLASS = "nitrogen_monoxide";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace NitrogenMonoxide

/**
 * @brief Nitrous Oxide concentration in µg/m³.
 */
namespace NitrousOxide {
const std::string DEVICE_CLASS = "nitrous_oxide";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace NitrousOxide

/**
 * @brief Ozone concentration in µg/m³.
 */
namespace Ozone {
const std::string DEVICE_CLASS = "ozone";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace Ozone

/**
 * @brief Potential hydrogen (pH) value of a water solution.
 */
namespace Ph {
const std::string DEVICE_CLASS = "ph";

// Typically, pH is a dimensionless unit and does not have a string representation for the unit.
inline std::optional<std::string> unit_of_measurement() { return std::nullopt; }
}; // namespace Ph

/**
 * @brief Particulate matter less than 1 micrometer in µg/m³.
 */
namespace Pm1 {
const std::string DEVICE_CLASS = "pm1";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace Pm1

/**
 * @brief Particulate matter less than 2.5 micrometers in µg/m³.
 */
namespace Pm25 {
const std::string DEVICE_CLASS = "pm25";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace Pm25

/**
 * @brief Particulate matter less than 10 micrometers in µg/m³.
 */
namespace Pm10 {
const std::string DEVICE_CLASS = "pm10";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace Pm10

/**
 * @brief Power factor (unitless), unit may be None or %.
 */
namespace PowerFactor {
const std::string DEVICE_CLASS = "power_factor";

enum class Unit { None, Percent };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::None:
    return std::nullopt;
  case Unit::Percent:
    return "%";
  }
  return std::nullopt;
}
}; // namespace PowerFactor

/**
 * @brief Power in mW, W, etc.
 */
namespace Power {
const std::string DEVICE_CLASS = "power";

enum class Unit { mW, W, kW, MW, GW, TW };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::mW:
    return "mW";
  case Unit::W:
    return "W";
  case Unit::kW:
    return "kW";
  case Unit::MW:
    return "MW";
  case Unit::GW:
    return "GW";
  case Unit::TW:
    return "TW";
  }
  return std::nullopt;
}
}; // namespace Power

/**
 * @brief Accumulated precipitation in cm, in or mm.
 */
namespace Precipitation {
const std::string DEVICE_CLASS = "precipitation";

enum class Unit { cm, in, mm };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::cm:
    return "cm";
  case Unit::in:
    return "in";
  case Unit::mm:
    return "mm";
  }
  return std::nullopt;
}
}; // namespace Precipitation

/**
 * @brief Precipitation intensity in in/d, in/h, etc.
 */
namespace PrecipitationIntensity {
const std::string DEVICE_CLASS = "precipitation_intensity";

enum class Unit { in_d, in_h, mm_d, mm_h };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::in_d:
    return "in/d";
  case Unit::in_h:
    return "in/h";
  case Unit::mm_d:
    return "mm/d";
  case Unit::mm_h:
    return "mm/h";
  }
  return std::nullopt;
}
}; // namespace PrecipitationIntensity

/**
 * @brief Pressure in various units.
 */
namespace Pressure {
const std::string DEVICE_CLASS = "pressure";

enum class Unit { Pa, kPa, hPa, bar, cbar, mbar, mmHg, inHg, psi };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Pa:
    return "Pa";
  case Unit::kPa:
    return "kPa";
  case Unit::hPa:
    return "hPa";
  case Unit::bar:
    return "bar";
  case Unit::cbar:
    return "cbar";
  case Unit::mbar:
    return "mbar";
  case Unit::mmHg:
    return "mmHg";
  case Unit::inHg:
    return "inHg";
  case Unit::psi:
    return "psi";
  }
  return std::nullopt;
}
}; // namespace Pressure

/**
 * @brief Reactive power in var.
 */
namespace ReactivePower {
const std::string DEVICE_CLASS = "reactive_power";

enum class Unit { var };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::var:
    return "var";
  }
  return std::nullopt;
}
}; // namespace ReactivePower

/**
 * @brief Signal strength in dB or dBm.
 */
namespace SignalStrength {
const std::string DEVICE_CLASS = "signal_strength";

enum class Unit { dB, dBm };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::dB:
    return "dB";
  case Unit::dBm:
    return "dBm";
  }
  return std::nullopt;
}
}; // namespace SignalStrength

/**
 * @brief Sound pressure in dB or dBA.
 */
namespace SoundPressure {
const std::string DEVICE_CLASS = "sound_pressure";

enum class Unit { dB, dBA };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::dB:
    return "dB";
  case Unit::dBA:
    return "dBA";
  }
  return std::nullopt;
}
}; // namespace SoundPressure

/**
 * @brief Generic speed in various units.
 */
namespace Speed {
const std::string DEVICE_CLASS = "speed";

enum class Unit { ft_s, in_d, in_h, in_s, km_h, kn, m_s, mph, mm_d, mm_s };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ft_s:
    return "ft/s";
  case Unit::in_d:
    return "in/d";
  case Unit::in_h:
    return "in/h";
  case Unit::in_s:
    return "in/s";
  case Unit::km_h:
    return "km/h";
  case Unit::kn:
    return "kn";
  case Unit::m_s:
    return "m/s";
  case Unit::mph:
    return "mph";
  case Unit::mm_d:
    return "mm/d";
  case Unit::mm_s:
    return "mm/s";
  }
  return std::nullopt;
}
}; // namespace Speed

/**
 * @brief Concentration of sulphur dioxide in µg/m³.
 */
namespace SulphurDioxide {
const std::string DEVICE_CLASS = "sulphur_dioxide";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace SulphurDioxide

/**
 * @brief Temperature in °C, °F, or K.
 */
namespace Temperature {
const std::string DEVICE_CLASS = "temperature";

enum class Unit { C, F, K };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::C:
    return "°C";
  case Unit::F:
    return "°F";
  case Unit::K:
    return "K";
  }
  return std::nullopt;
}
}; // namespace Temperature

/**
 * @brief Timestamp string (ISO 8601).
 */
namespace Timestamp {
const std::string DEVICE_CLASS = "timestamp";

// Timestamps are generally represented as ISO 8601 strings.
inline std::optional<std::string> unit_of_measurement() { return std::nullopt; }
}; // namespace Timestamp

/**
 * @brief Concentration of volatile organic compounds in µg/m³.
 */
namespace VolatileOrganicCompounds {
const std::string DEVICE_CLASS = "volatile_organic_compounds";

enum class Unit { ug_m3 };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ug_m3:
    return "µg/m³";
  }
  return std::nullopt;
}
}; // namespace VolatileOrganicCompounds

/**
 * @brief VOCs ratio in ppm or ppb.
 */
namespace VolatileOrganicCompoundsParts {
const std::string DEVICE_CLASS = "volatile_organic_compounds_parts";

enum class Unit { ppm, ppb };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::ppm:
    return "ppm";
  case Unit::ppb:
    return "ppb";
  }
  return std::nullopt;
}
}; // namespace VolatileOrganicCompoundsParts

/**
 * @brief Voltage in V, mV, µV.
 */
namespace Voltage {
const std::string DEVICE_CLASS = "voltage";

enum class Unit { V, mV, uV };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::V:
    return "V";
  case Unit::mV:
    return "mV";
  case Unit::uV:
    return "µV";
  }
  return std::nullopt;
}
}; // namespace Voltage

/**
 * @brief Generic volume in L, mL, etc.
 */
namespace Volume {
const std::string DEVICE_CLASS = "volume";

enum class Unit { L, mL, gal, fl_oz, m3, ft3, CCF };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::L:
    return "L";
  case Unit::mL:
    return "mL";
  case Unit::gal:
    return "gal";
  case Unit::fl_oz:
    return "fl. oz.";
  case Unit::m3:
    return "m³";
  case Unit::ft3:
    return "ft³";
  case Unit::CCF:
    return "CCF";
  }
  return std::nullopt;
}
}; // namespace Volume

/**
 * @brief Volume flow rate in m³/h, ft³/min, etc.
 */
namespace VolumeFlowRate {
const std::string DEVICE_CLASS = "volume_flow_rate";

enum class Unit { m3_h, ft3_min, L_min, gal_min, mL_s };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::m3_h:
    return "m³/h";
  case Unit::ft3_min:
    return "ft³/min";
  case Unit::L_min:
    return "L/min";
  case Unit::gal_min:
    return "gal/min";
  case Unit::mL_s:
    return "mL/s";
  }
  return std::nullopt;
}
}; // namespace VolumeFlowRate

/**
 * @brief Generic stored volume in L, mL, etc.
 */
namespace VolumeStorage {
const std::string DEVICE_CLASS = "volume_storage";

enum class Unit { L, mL, gal, fl_oz, m3, ft3, CCF };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::L:
    return "L";
  case Unit::mL:
    return "mL";
  case Unit::gal:
    return "gal";
  case Unit::fl_oz:
    return "fl. oz.";
  case Unit::m3:
    return "m³";
  case Unit::ft3:
    return "ft³";
  case Unit::CCF:
    return "CCF";
  }
  return std::nullopt;
}
}; // namespace VolumeStorage

/**
 * @brief Water consumption in L, gal, etc.
 */
namespace Water {
const std::string DEVICE_CLASS = "water";

enum class Unit { L, gal, m3, ft3, CCF };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::L:
    return "L";
  case Unit::gal:
    return "gal";
  case Unit::m3:
    return "m³";
  case Unit::ft3:
    return "ft³";
  case Unit::CCF:
    return "CCF";
  }
  return std::nullopt;
}
}; // namespace Water

/**
 * @brief Generic mass in kg, g, etc.
 */
namespace Weight {
const std::string DEVICE_CLASS = "weight";

enum class Unit { kg, g, mg, ug, oz, lb, st };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::kg:
    return "kg";
  case Unit::g:
    return "g";
  case Unit::mg:
    return "mg";
  case Unit::ug:
    return "µg";
  case Unit::oz:
    return "oz";
  case Unit::lb:
    return "lb";
  case Unit::st:
    return "st";
  }
  return std::nullopt;
}
}; // namespace Weight

/**
 * @brief Wind speed in Beaufort, ft/s, etc.
 */
namespace WindSpeed {
const std::string DEVICE_CLASS = "wind_speed";

enum class Unit { Beaufort, ft_s, km_h, kn, m_s, mph };

inline std::optional<std::string> unit_of_measurement(const Unit &unit) {
  switch (unit) {
  case Unit::Beaufort:
    return "Beaufort";
  case Unit::ft_s:
    return "ft/s";
  case Unit::km_h:
    return "km/h";
  case Unit::kn:
    return "kn";
  case Unit::m_s:
    return "m/s";
  case Unit::mph:
    return "mph";
  }
  return std::nullopt;
}
}; // namespace WindSpeed

}; // namespace DeviceClass
}; // namespace Sensor

namespace BinarySensor {

/**
 * @brief Device classes and their units from https://www.home-assistant.io/integrations/binary_sensor/#device-class
 */
namespace DeviceClass {

/**
 * @brief on means low, off means normal
 */
namespace Battery {
const std::string DEVICE_CLASS = "battery";
}; // namespace Battery

/**
 * @brief on means charging, off means not charging
 */
namespace BatteryCharging {
const std::string DEVICE_CLASS = "battery_charging";
}; // namespace BatteryCharging

/**
 * @brief on means carbon monoxide detected, off means clear
 */
namespace CarbonMonoxide {
const std::string DEVICE_CLASS = "carbon_monoxide";
}; // namespace CarbonMonoxide

/**
 * @brief on means cold, off means normal
 */
namespace Cold {
const std::string DEVICE_CLASS = "cold";
}; // namespace Cold

/**
 * @brief on means connected, off means disconnected
 */
namespace Connectivity {
const std::string DEVICE_CLASS = "connectivity";
}; // namespace Connectivity

/**
 * @brief on means open, off means closed
 */
namespace Door {
const std::string DEVICE_CLASS = "door";
}; // namespace Door

/**
 * @brief on means open, off means closed
 */
namespace GarageDoor {
const std::string DEVICE_CLASS = "garage_door";
}; // namespace GarageDoor

/**
 * @brief on means gas detected, off means clear
 */
namespace Gas {
const std::string DEVICE_CLASS = "gas";
}; // namespace Gas

/**
 * @brief on means hot, off means normal
 */
namespace Heat {
const std::string DEVICE_CLASS = "heat";
}; // namespace Heat

/**
 * @brief on means light detected, off means no light
 */
namespace Light {
const std::string DEVICE_CLASS = "light";
}; // namespace Light

/**
 * @brief on means open (unlocked), off means closed (locked)
 */
namespace Lock {
const std::string DEVICE_CLASS = "lock";
}; // namespace Lock

/**
 * @brief on means moisture detected (wet), off means no moisture (dry)
 */
namespace Moisture {
const std::string DEVICE_CLASS = "moisture";
}; // namespace Moisture

/**
 * @brief on means motion detected, off means clear
 */
namespace Motion {
const std::string DEVICE_CLASS = "motion";
}; // namespace Motion

/**
 * @brief on means moving, off means stopped
 */
namespace Moving {
const std::string DEVICE_CLASS = "moving";
}; // namespace Moving

/**
 * @brief on means occupied (detected), off means clear
 */
namespace Occupancy {
const std::string DEVICE_CLASS = "occupancy";
}; // namespace Occupancy

/**
 * @brief on means open, off means closed
 */
namespace Opening {
const std::string DEVICE_CLASS = "opening";
}; // namespace Opening

/**
 * @brief on means device is plugged in, off means device is unplugged
 */
namespace Plug {
const std::string DEVICE_CLASS = "plug";
}; // namespace Plug

/**
 * @brief on means power detected, off means no power
 */
namespace Power {
const std::string DEVICE_CLASS = "power";
}; // namespace Power

/**
 * @brief on means home, off means away
 */
namespace Presence {
const std::string DEVICE_CLASS = "presence";
}; // namespace Presence

/**
 * @brief on means problem detected, off means OK
 */
namespace Problem {
const std::string DEVICE_CLASS = "problem";
}; // namespace Problem

/**
 * @brief on means running, off means not running
 */
namespace Running {
const std::string DEVICE_CLASS = "running";
}; // namespace Running

/**
 * @brief on means unsafe, off means safe
 */
namespace Safety {
const std::string DEVICE_CLASS = "safety";
}; // namespace Safety

/**
 * @brief on means smoke detected, off means clear
 */
namespace Smoke {
const std::string DEVICE_CLASS = "smoke";
}; // namespace Smoke

/**
 * @brief on means sound detected, off means clear
 */
namespace Sound {
const std::string DEVICE_CLASS = "sound";
}; // namespace Sound

/**
 * @brief on means tampering detected, off means clear
 */
namespace Tamper {
const std::string DEVICE_CLASS = "tamper";
}; // namespace Tamper

/**
 * @brief on means update available, off means up-to-date
 */
namespace Update {
const std::string DEVICE_CLASS = "update";
}; // namespace Update

/**
 * @brief on means vibration detected, off means clear
 */
namespace Vibration {
const std::string DEVICE_CLASS = "vibration";
}; // namespace Vibration

/**
 * @brief on means open, off means closed
 */
namespace Window {
const std::string DEVICE_CLASS = "window";
}; // namespace Window

}; // namespace DeviceClass
}; // namespace BinarySensor

namespace Event {
namespace DeviceClass {

/**
 * @brief For remote control buttons.
 */
namespace Button {
const std::string DEVICE_CLASS = "button";
}; // namespace Button

/**
 * @brief Specifically for buttons that are used as a doorbell.
 */
namespace Doorbell {
const std::string DEVICE_CLASS = "doorbell";
}; // namespace Doorbell

/**
 * @brief For motion events detected by a motion sensor.
 */
namespace Motion {
const std::string DEVICE_CLASS = "motion";
}; // namespace Motion

}; // namespace DeviceClass
}; // namespace Event

}; // namespace homeassistantentities