#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "nrfx_twim.h"
#include "nrf_drv_twi.h"
#include <math.h>


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/* SHT20 I2C-ADDRESS */
#define SHT_I2C_ADDR                          0x40

#define ERROR_I2C_TIMEOUT                     998
#define ERROR_BAD_CRC                         999


#define TRIGGER_TEMP_MEASURE_HOLD             0xE3
#define TRIGGER_HUMD_MEASURE_HOLD             0xE5

#define TRIGGER_TEMP_MEASURE_NOHOLD           0xF3
#define TRIGGER_HUMD_MEASURE_NOHOLD           0xF5

#define WRITE_USER_REG                        0xE6
#define READ_USER_REG                         0xE7
#define SOFT_RESET                            0xFE
#define USER_REGISTER_RESOLUTION_MASK         0x81
#define USER_REGISTER_RESOLUTION_RH12_TEMP14  0x00
#define USER_REGISTER_RESOLUTION_RH8_TEMP12   0x01
#define USER_REGISTER_RESOLUTION_RH10_TEMP13  0x80
#define USER_REGISTER_RESOLUTION_RH11_TEMP11  0x81
#define USER_REGISTER_END_OF_BATTERY          0x40
#define USER_REGISTER_HEATER_ENABLED          0x04
#define USER_REGISTER_DISABLE_OTP_RELOAD      0x02
#define MAX_WAIT                              100
#define DELAY_INTERVAL                        10
#define SHIFTED_DIVISOR                       0x988000
#define MAX_COUNTER                           (MAX_WAIT/DELAY_INTERVAL)


ret_code_t SHT_2x_init (int SDA, int SCL);
ret_code_t SHT_2x_register_write(uint8_t reg_addr, uint8_t * p_tx_data, uint8_t bytes);
uint8_t SHT_2x_register_read(uint8_t reg_addr);
uint16_t SHT_2x_register_read_2(uint8_t reg_addr);

ret_code_t SHT_2x_check(void);
ret_code_t SHT_2x_setDefaultData();
float SHT_2x_readHumidity();
float SHT_2x_readTemperature();

ret_code_t SHT_2x_softReset();
ret_code_t SHT_2x_writeUserRegister();
ret_code_t SHT_2x_setHumidityResolution();
ret_code_t SHT_2x_setTemperatureResolution();
ret_code_t SHT_2x_setTemperatureResolution();


    
    
