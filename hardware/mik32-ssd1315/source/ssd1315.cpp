#include "ssd1315.h"

#define INIT_DELAY_MS (10)

ssd1315::ssd1315(
  SPI_TypeDef* spi, 
  GPIO_TypeDef *dc_gpio, 
  HAL_PinsTypeDef dc_pin,
  GPIO_TypeDef *reset_gpio,
  HAL_PinsTypeDef reset_pin
) : spi(spi), 
    dc_gpio(dc_gpio), 
    dc_pin(dc_pin), 
    reset_gpio(reset_gpio), 
    reset_pin(reset_pin) {}

ssd1315::~ssd1315() = default;

ssd1315::ssd1315(const ssd1315&) = default;
ssd1315::ssd1315(ssd1315&&) = default;
ssd1315& ssd1315::operator=(const ssd1315&) = default;
ssd1315& ssd1315::operator=(ssd1315&&) = default;

HAL_StatusTypeDef spi_exchange(SPI_TypeDef* spi, const uint8_t transmit[], uint8_t receive[], uint32_t n, uint32_t timeout)
{
    bool txallowed = true;
    HAL_StatusTypeDef error_code = HAL_OK;
    uint32_t timeout_counter = 0;

    size_t rx_count = n;
    size_t tx_count = n;

    spi->TX_THR = 1;

    /* Включить SPI если выключено */
    if (!(spi->ENABLE & SPI_ENABLE_M))
    {
        spi->ENABLE = SPI_ENABLE_M;
    }

    while ((tx_count > 0) || (rx_count > 0))
    {
        if ((spi->INT_STATUS & SPI_INT_STATUS_TX_FIFO_NOT_FULL_M) && (tx_count > 0) && txallowed)
        {
            if (transmit) {
                spi->TXDATA = *(transmit++);
            } else {
                spi->TXDATA = 0;
            }
            tx_count--;
            txallowed = false;
        }

        if ((spi->INT_STATUS & SPI_INT_STATUS_RX_FIFO_NOT_EMPTY_M) && (rx_count > 0))
        {
            if (receive) {
                *(receive++) = spi->RXDATA;
            } else {
                (void) spi->RXDATA;
            }
            rx_count--;
            txallowed = true;
        }

        if ((timeout_counter++) >= timeout)
        {
            error_code = HAL_TIMEOUT;
            goto error;
        }
    }

error:
    if (!(spi->CONFIG & SPI_CONFIG_MANUAL_CS_M))
    {
        spi->ENABLE &= ~SPI_ENABLE_M;
        spi->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M | SPI_ENABLE_CLEAR_RX_FIFO_M;
    }

    (void) spi->INT_STATUS;

    return error_code;
}

HAL_StatusTypeDef ssd1315::init() const {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = dc_pin;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    HAL_GPIO_Init(dc_gpio, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = reset_pin;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    HAL_GPIO_Init(reset_gpio, &GPIO_InitStruct);

    HAL_GPIO_WritePin(reset_gpio, reset_pin, GPIO_PIN_HIGH);
    HAL_DelayMs(INIT_DELAY_MS);
    HAL_GPIO_WritePin(reset_gpio, reset_pin, GPIO_PIN_LOW);
    HAL_DelayMs(INIT_DELAY_MS);
    HAL_GPIO_WritePin(reset_gpio, reset_pin, GPIO_PIN_HIGH);

    initialized = true;
    return HAL_OK;
}

HAL_StatusTypeDef ssd1315::command(const uint8_t* command, size_t n) const {
    if (!initialized) {
        init();
    }
    set_command();
    return spi_exchange(spi, command, nullptr, n, SPI_TIMEOUT_DEFAULT);
}

HAL_StatusTypeDef ssd1315::data(const uint8_t* data, size_t n) const {
    if (!initialized) {
        init();
    }
    set_data();
    return spi_exchange(spi, data, nullptr, n, SPI_TIMEOUT_DEFAULT);
}

void ssd1315::set_command() const {
    HAL_GPIO_WritePin(dc_gpio, dc_pin, GPIO_PIN_LOW);
}

void ssd1315::set_data() const {
    HAL_GPIO_WritePin(dc_gpio, dc_pin, GPIO_PIN_HIGH);
}
