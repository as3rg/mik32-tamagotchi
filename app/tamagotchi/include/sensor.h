#pragma once
extern "C" {
#include "analog_reg.h"
}
#include <cstdint>
namespace tamagotchi {
struct sensor {
protected:
  ANALOG_REG_TypeDef *instance;
  uint8_t ch;

public:
  constexpr static uint32_t max_value = 4095;

  constexpr sensor(ANALOG_REG_TypeDef *instance, uint8_t ch)
      : instance(instance), ch(ch) {}

  constexpr ~sensor() = default;

  constexpr sensor(const sensor &) = default;

  constexpr sensor(sensor &&) = default;

  constexpr sensor &operator=(const sensor &) = default;

  constexpr sensor &operator=(sensor &&) = default;

  constexpr void init() { instance->ADC_CONTINUOUS = 1; }

  constexpr uint16_t get() { return instance->ADC_VALUE; }
};
} // namespace tamagotchi