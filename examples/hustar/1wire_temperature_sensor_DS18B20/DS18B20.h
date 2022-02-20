#include "nrf_gpio.h"
#include "nrf_delay.h"
#include <stdio.h>

#define ONEWIRE_PIN             28

void DS18B20_Run(void);
float DS18B20_get_temperature(void);

void DS18B20_request_temperature();
float DS18B20_read_temperature();



