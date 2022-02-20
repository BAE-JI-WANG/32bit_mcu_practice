#include <stdint.h>
#include <string.h>
#include "app_timer.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_gpiote.h"
#include <math.h>
	
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
		
#include "nrf_drv_pwm.h"
#include "app_pwm.h"


void servo_motor_init(int pin);

void servo_motor_execute(uint8_t * data_array, uint8_t index);

void servo_motor_execute_by_degree(int degree);






