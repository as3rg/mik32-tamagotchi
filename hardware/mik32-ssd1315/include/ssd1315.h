#ifndef MIK32_HAL_SSD1315
#define MIK32_HAL_SSD1315

extern "C" {
#include "mik32_hal_spi.h"
#include "mik32_hal_gpio.h"
#include <inttypes.h>
#include <stddef.h>
}

struct ssd1315 {
public:
  ssd1315(
    SPI_TypeDef* spi, 
    GPIO_TypeDef *dc_gpio, 
    HAL_PinsTypeDef dc_pin,
    GPIO_TypeDef *reset_gpio,
    HAL_PinsTypeDef reset_pin
  );

  ~ssd1315();

  ssd1315(const ssd1315&);
  ssd1315(ssd1315&&);
  ssd1315& operator=(const ssd1315&);
  ssd1315& operator=(ssd1315&&);

  HAL_StatusTypeDef init() const;
  HAL_StatusTypeDef command(const uint8_t* command, size_t n) const;
  HAL_StatusTypeDef data(const uint8_t* data, size_t n) const;

private:
  mutable bool initialized = false;
  SPI_TypeDef* spi;
  GPIO_TypeDef* dc_gpio;
  HAL_PinsTypeDef dc_pin;
  GPIO_TypeDef* reset_gpio;
  HAL_PinsTypeDef reset_pin;

  void set_command() const;
  void set_data() const;
};

#endif
