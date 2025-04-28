#include "interface.h"
#include <cstdint>
extern "C" {
#include "mik32_hal.h"
#include "mik32_hal_adc.h"
#include "mik32_hal_irq.h"
#include "mik32_hal_spi.h"
#include "mik32_hal_timer32.h"
#include "mik32_memory_map.h"
#include "uart_lib.h"
}

#define SYSTEM_FREQ_HZ 32000000UL
#define UPDATE_FREQ_HZ 50
#define UPDATE_PERIOD (SYSTEM_FREQ_HZ / UPDATE_FREQ_HZ)

TIMER32_HandleTypeDef htimer32;
TIMER32_CHANNEL_HandleTypeDef htimer32_channel;

void SystemClock_Config(void);
static void SPI0_Init(void);
static void GPIO_Init(void);
static void Timer32_Init(void);
static void ADC_Init(void);

tamagotchi::interface interface_{SPI_0};

int main() {
  SystemClock_Config();
  HAL_Init();
  GPIO_Init();
  UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
  SPI0_Init();
  Timer32_Init();
  ADC_Init();

  HAL_Timer32_Value_Clear(&htimer32);
  if (HAL_Timer32_Compare_Start_IT(&htimer32, &htimer32_channel) != HAL_OK) {
    // xprintf("Error\r\n");
  }

  HAL_EPIC_MaskLevelSet(HAL_EPIC_TIMER32_0_MASK | HAL_EPIC_TIMER32_1_MASK |
                        HAL_EPIC_TIMER32_2_MASK);
  HAL_IRQ_EnableInterrupts();

  interface_.init();
  while (1) {
    HAL_IRQ_DisableInterrupts();
    interface_.update();
    HAL_IRQ_EnableInterrupts();
    interface_.draw();
  }
}

void GPIO_Init() {
  __HAL_PCC_GPIO_0_CLK_ENABLE();
  __HAL_PCC_GPIO_1_CLK_ENABLE();
  __HAL_PCC_GPIO_2_CLK_ENABLE();
  __HAL_PCC_GPIO_IRQ_CLK_ENABLE();
}

void SystemClock_Config(void) {
  PCC_InitTypeDef PCC_OscInit = {0};

  PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
  PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
  PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
  PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
  PCC_OscInit.AHBDivider = 0;
  PCC_OscInit.APBMDivider = 0;
  PCC_OscInit.APBPDivider = 0;
  PCC_OscInit.HSI32MCalibrationValue = 128;
  PCC_OscInit.LSI32KCalibrationValue = 8;
  PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
  PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
  HAL_PCC_Config(&PCC_OscInit);
}

static void SPI0_Init(void) {
  SPI_HandleTypeDef hspi0;
  hspi0.Instance = SPI_0;

  /* Режим SPI */
  hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;

  /* Настройки */
  hspi0.Init.CLKPhase = SPI_PHASE_ON;
  hspi0.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi0.Init.ThresholdTX = 4;

  /* Настройки для ведущего */
  hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
  hspi0.Init.Decoder = SPI_DECODER_NONE;
  hspi0.Init.ManualCS = SPI_MANUALCS_OFF;
  hspi0.Init.ChipSelect = SPI_CS_0;

  if (HAL_SPI_Init(&hspi0) != HAL_OK) {
    // xprintf("SPI_Init_Error\n");
  }
}

static void Timer32_Init(void) {
  htimer32.Instance = TIMER32_1;
  htimer32.Top = UPDATE_PERIOD;
  htimer32.Clock.Source = TIMER32_SOURCE_PRESCALER;
  htimer32.Clock.Prescaler = 0;
  htimer32.InterruptMask = TIMER32_INT_OVERFLOW_M | TIMER32_INT_UNDERFLOW_M;
  htimer32.CountMode = TIMER32_COUNTMODE_FORWARD;
  if (HAL_Timer32_Init(&htimer32) != HAL_OK) {
    // xprintf("Timer32_Init error\r\n");
  }

  htimer32_channel.TimerInstance = htimer32.Instance;
  htimer32_channel.ChannelIndex = TIMER32_CHANNEL_0;
  htimer32_channel.PWM_Invert = TIMER32_CHANNEL_NON_INVERTED_PWM;
  htimer32_channel.Mode = TIMER32_CHANNEL_MODE_COMPARE;
  htimer32_channel.OCR = htimer32.Top;
  if (HAL_Timer32_Channel_Init(&htimer32_channel) != HAL_OK) {
    // xprintf("Timer32_Channel_Init error\r\n");
  }
}
static void ADC_Init(void) {
  ADC_HandleTypeDef hadc;
  hadc.Instance = ANALOG_REG;
  hadc.Init.Sel = 0;
  hadc.Init.EXTRef = ADC_EXTREF_OFF;
  hadc.Init.EXTClb = ADC_EXTCLB_CLBREF;

  HAL_ADC_Init(&hadc);
}

extern "C" void trap_handler() {
  if (EPIC_CHECK_TIMER32_1()) {
    uint32_t interrupt_status = HAL_Timer32_InterruptFlags_Get(&htimer32);
    uint32_t interrupt_mask = htimer32.Instance->INT_MASK;

    if ((interrupt_status & TIMER32_INT_OC_M(htimer32_channel.ChannelIndex)) &
        interrupt_mask) {
      interface_.tick();
    }
    HAL_TIMER32_INTERRUPTFLAGS_CLEAR(&htimer32); /* Сброс всех флагов */
  }

  HAL_EPIC_Clear(0xFFFFFFFF);
}