#include <stdint.h>
#include <string.h>

#include "app_uart.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "boards.h"

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#include "app_util_platform.h"


#include "nrfx_twim.h"
#include "nrf_drv_twi.h"
#include <math.h>


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"





#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(format, args...) ((void)0)
#endif

/* ds3231 I2C-ADDRESS */              
#define ds3231_I2C_ADDR           0xD0 >> 1             

/* General Register */
#define REG_SEC          0x00    // Current Seconds Register
#define REG_MIN          0x01    // Current Minutes Register
#define REG_HOUR         0x02    // Current Hours Register 
#define REG_DAY          0x03    // Current Day Register
#define REG_DATE         0x04    // Current Dates Register
#define REG_MONTH        0x05    // Current Months Register
#define REG_YEAR         0x06    // Current Years Register
#define REG_TEMP_UPPER   0x11    // Temperature Upper Byte
#define REG_TEMP_LOWER   0x12    // Temperature Lower Byte



typedef struct
{
    uint8_t scl_pin;
    uint8_t sda_pin;
} ds3231_twi_pins_config_t;

enum {
 DS3231_WRITE_YEAR1,          
 DS3231_WRITE_YEAR2,      
 DS3231_WRITE_MONTH1,         
 DS3231_WRITE_MONTH2,   
 DS3231_WRITE_DATE1,          
 DS3231_WRITE_DATE2,                               
 DS3231_WRITE_HOUR1,          
 DS3231_WRITE_HOUR2,                 
 DS3231_WRITE_MIN1,           
 DS3231_WRITE_MIN2,
 DS3231_WRITE_SEC1,        
 DS3231_WRITE_SEC2,
 DS3231_WRITE_DAY,
 DS3231_WRITE_MAX
};

enum {
 DS3231_READ_YEAR,          
 DS3231_READ_MONTH,      
 DS3231_READ_DATE,         
 DS3231_READ_HOUR,   
 DS3231_READ_MIN,          
 DS3231_READ_SEC,                               
 DS3231_READ_DAY,          
 DS3231_READ_MAX
};

/* TWI Init Function */
ret_code_t ds3231_twi_setup(ds3231_twi_pins_config_t const * p_pins_config);
void ds3231_twi_uninit();


/* Register Control Function */
void ds3231_set_current_time(uint8_t * data_array);

void ds3231_get_current_time(uint8_t * data_array);

float ds3231_get_current_temperature();