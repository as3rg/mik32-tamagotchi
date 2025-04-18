#ifndef DHT11_DIRVER_H_
#define DHT11_DRIVER_H_

#include <cstdlib>

#define inline_always [[gnu::always_inline]] inline

extern "C" {
#include "mik32_hal_gpio.h"
}

struct dht11 {
public:
  struct packet {
    uint8_t humidity_h;
    uint8_t humidity_l;
    uint8_t temp_h;
    uint8_t temp_l;
    uint8_t checksum;
  };

  dht11(GPIO_TypeDef *gpio, HAL_PinsTypeDef pin) : gpio(gpio), pin(pin) {}

  __attribute__((section(".ram_text"))) packet receive();

private:
  GPIO_TypeDef *gpio;
  HAL_PinsTypeDef pin;

  inline_always bool get_lvl();
  inline_always void wait_high();
  inline_always void wait_low();
  inline_always void init_connection();
  inline_always void wait_for_answer();
  inline_always bool read_bit();
  inline_always uint8_t read_byte();
};

#endif