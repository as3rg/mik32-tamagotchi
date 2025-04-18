extern "C" {
#include "scr1_timer.h"
}

#include "dht11.h"

static constexpr uint32_t INIT_LOW_US = 18000;
static constexpr uint32_t INIT_HIGH_US = 40;
static constexpr uint32_t BIT_DELAY_US = 35;
static constexpr uint64_t SYSTEM_FREQ_HZ = 32000000UL;
static constexpr uint32_t US_IN_S = 1000000;
static constexpr uint8_t BITS_IN_BYTE = 8;


uint64_t get_time() {
    return (static_cast<uint64_t>(SCR1_TIMER->MTIMEH) << 32) | (SCR1_TIMER->MTIME);
}

void delay(uint32_t us) {
    uint64_t end_mtimer = get_time() + us * (SYSTEM_FREQ_HZ / US_IN_S);
    while (get_time() < end_mtimer)
        ;
}

bool dht11::get_lvl() {
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_HIGH;
}

void dht11::wait_high() {
    while (!get_lvl())
        ;
}

void dht11::wait_low() {
    while (get_lvl())
        ;
}

void dht11::init_connection() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;    
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);

    HAL_GPIO_WritePin(gpio, pin, GPIO_PIN_LOW);
    delay(INIT_LOW_US);

    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_INPUT;    
    HAL_GPIO_Init(gpio, &GPIO_InitStruct);
}

void dht11::wait_for_answer() {
    wait_low();
    wait_high();
}

bool dht11::read_bit() {
    wait_high();
    delay(BIT_DELAY_US);
    char result = get_lvl();
    wait_low();
    return result;
}

uint8_t dht11::read_byte() {
    uint8_t result = 0;
    for (uint8_t i = BITS_IN_BYTE; i--;) {
        result |= read_bit() << i;
    }
    return result;
}

dht11::packet dht11::receive() {
    dht11::packet pkg;
    init_connection();
    wait_for_answer();
    wait_low();
    pkg.humidity_h = read_byte();
    pkg.humidity_l = read_byte();
    pkg.temp_h = read_byte();
    pkg.temp_l = read_byte();
    pkg.checksum = read_byte();
    wait_high();
    return pkg;
}

