#pragma once

#include "gpio.h"
extern "C" {
#include "mik32_hal_gpio.h"
}

#include "logical_button.h"

namespace tamagotchi {
struct control_button {
protected:
  logical_button btn;
  GPIO_TypeDef *gpio;
  HAL_PinsTypeDef pin;

public:
  constexpr control_button(GPIO_TypeDef *gpio, HAL_PinsTypeDef pin)
      : btn(), gpio(gpio), pin(pin) {}

  constexpr ~control_button() = default;

  constexpr control_button(const control_button &) = default;

  constexpr control_button(control_button &&) = default;

  constexpr control_button &operator=(const control_button &) = default;

  constexpr control_button &operator=(control_button &&) = default;

  constexpr void init() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);
  }

  constexpr logical_button::button_event tick() {
    return btn.tick(HAL_GPIO_ReadPin(gpio, pin));
  }
};
} // namespace tamagotchi
