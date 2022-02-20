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


/* DS1302_WRITE_ADDDRESS */
#define WRITE_SECONDS       0x80
#define WRITE_MINUTES       0x82
#define WRITE_HOUR          0x84
#define WRITE_DATE          0x86
#define WRITE_MONTH         0x88
#define WRITE_DAY           0x8A
#define WRITE_YEAR          0x8C

/* DS1302_READ_ADDDRESS */
#define READ_SECONDS       0x81
#define READ_MINUTES       0x83
#define READ_HOUR          0x85
#define READ_DATE          0x87
#define READ_MONTH         0x89
#define READ_DAY           0x8B
#define READ_YEAR          0x8D

/* DS1302_CHARGER */
#define WRITE_CHARGER      0x90
#define READ_CHARGER       0x91

typedef struct
{
    uint8_t _Year;
    uint8_t _Month;
    uint8_t _Date;
    uint8_t _Day; //1~7  Sunday is 0
    uint8_t _Hour;
    uint8_t _Minute;
    uint8_t _Seconds;
} time_config_t;

void DS1302_Init(int SCLK, int IO, int CS);

void DS1302_Set_Time(time_config_t time);
time_config_t DS1302_Get_Time(void);

void DS1302_On_Charger(int Diode_sel, int Reg_sel); // Diode = 1 or 2, Reg = 2, 4, 8
void DS1302_Off_Charger(void); // Diode = 1 or 2, Reg = 2, 4, 8
uint8_t DS1302_Read_Charger();


