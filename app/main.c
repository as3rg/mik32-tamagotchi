#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"
#include "mik32_hal_i2c.h"
#include "lcd_driver.h"
/*
 * Данный пример демонстрирует вывод данных через USART.
 */


static void SystemClock_Config();
static void USART_Init();
static void I2C_Init();

static USART_HandleTypeDef husart0;
I2C_HandleTypeDef hi2c;

int main()
{
    SystemClock_Config();
    USART_Init();
    I2C_Init();    
    
    uint8_t data[1] = { 0x55 };
    uint8_t size = 8; 


    lcd_init ();
    lcd_put_cur(0, 0);
    lcd_send_string ("HELLO WORLD");
    lcd_put_cur(1, 0);
    lcd_send_string("from CTECH");

    while (1)
    {
        if(HAL_OK == HAL_I2C_Master_Transmit(&hi2c, 0x27, data, size, I2C_TIMEOUT_DEFAULT)) {
            HAL_USART_Print(&husart0, "I2C Example Start\r\n", USART_TIMEOUT_DEFAULT);
        }


        lcd_put_cur(0, 2);
        lcd_send_string ("Hello, Yadro");
   
      
        
        HAL_DelayMs(1000);
    }
}

void SystemClock_Config(void)
{
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

void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode3;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    husart0.dma_tx_request = Disable;
    husart0.dma_rx_request = Disable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 115200;
    HAL_USART_Init(&husart0);
}

void I2C_Init() {
    hi2c.Instance = I2C_1;
    hi2c.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c.Init.DigitalFilter = I2C_DIGITALFILTER_2CLOCKCYCLES;
    hi2c.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c.Init.AutoEnd = I2C_AUTOEND_ENABLE;

    /* Настройка частоты */
    hi2c.Clock.PRESC = 4;
    hi2c.Clock.SCLDEL = 7;
    hi2c.Clock.SDADEL = 7;
    hi2c.Clock.SCLH = 31;
    hi2c.Clock.SCLL = 31;

    if (HAL_I2C_Init(&hi2c) != HAL_OK)
    {
        HAL_USART_Print(&husart0, "I2C init error\r\n", USART_TIMEOUT_DEFAULT);
    }
}

