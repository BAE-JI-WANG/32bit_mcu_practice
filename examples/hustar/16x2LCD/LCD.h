#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf.h"

#include "nrf_delay.h"

#include "nrf_drv_gpiote.h"



typedef struct
{
    uint8_t rs_pin;
    uint8_t en_pin;
    uint8_t bg_pin;
    uint8_t db4_pin;
    uint8_t db5_pin;
    uint8_t db6_pin;
    uint8_t db7_pin;
}lcd_pins_config_t;



void LCD_init(lcd_pins_config_t const * p_pins_config);
void LCD_STRING_WRITE(uint8_t x, uint8_t y, uint8_t len, uint8_t *p);
void LCD_SET_BG(bool onoff);


 
