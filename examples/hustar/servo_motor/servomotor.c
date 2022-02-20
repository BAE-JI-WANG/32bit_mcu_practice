#include "servomotor.h"
	
APP_TIMER_DEF(m_servo_timer_id);
	
APP_PWM_INSTANCE(PWM1,1);

    
long period_in_us = 5000L;
	
long servo_interval = 0;
int servo_degree = 20;
int servo_degree_max = 360;

bool is_forward = true;

static void timer_init(void);
void servo_timer_start() ;
void servo_timer_stop();
void servo_timer_handler();
void servo_motor_pwm_init(int pin);

	
void servo_motor_pwm_init(int pin)
{
    /* 1-channel PWM, 200Hz, output on DK LED pins, 5ms period */
	app_pwm_config_t pwm1_cfg = APP_PWM_DEFAULT_CONFIG_1CH(period_in_us, pin);
	
	pwm1_cfg.pin_polarity[0] = APP_PWM_POLARITY_ACTIVE_HIGH;  
	
	int ret = app_pwm_init(&PWM1,&pwm1_cfg,NULL);
	
	app_pwm_enable(&PWM1);
}
	
void servo_motor_init(int pin)
{
    timer_init();

    servo_motor_pwm_init(pin);
}
	
	
/**@brief Function for initializing the timer module.
 */
static void timer_init(void)
{
    uint32_t err_code;
	
    err_code = app_timer_create(&m_servo_timer_id, APP_TIMER_MODE_SINGLE_SHOT, servo_timer_handler);
}


void servo_motor_execute_by_degree(int degree)
{
    servo_timer_stop();
    
    uint32_t err_code;
    
    long freq_temp = 0;
    int duty = 0;

    if(degree <= 90)
    {
      freq_temp = (1500 - ((90 - degree)/0.097));
      duty = ((freq_temp * 100) / period_in_us);
    }
    else
    {
      freq_temp = (1500 + ((degree - 90)/0.097));
      duty = ((freq_temp * 100) / period_in_us);
    }

    printf("\r\n degree : %d freq_temp %d duty %d\r\n", degree, freq_temp, duty);

    while (app_pwm_channel_duty_set(&PWM1, 0, duty) == NRF_ERROR_BUSY);   
}


/* data_array : moving data
        ex> 180\n  -> Move Once
               030 130 0050\n -> Move Continuously
     index : data_array length
*/

void servo_motor_execute(uint8_t * data_array, uint8_t index)
{
    if(index == 4)  // Move Once
    {
        char degree[3] = {data_array[0],data_array[1],data_array[2]};// degree
        int degree_val = atoi(degree);
        servo_degree = degree_val;
	
        servo_timer_stop();
	
        long freq_temp = 0;
        int duty = 0;
	
        if(servo_degree <= 90)
        {
          freq_temp = (1500 - ((90 - servo_degree)/0.097));
          duty = ((freq_temp * 100) / period_in_us);
        }
        else
        {
          freq_temp = (1500 + ((servo_degree - 90)/0.097));
          duty = ((freq_temp * 100) / period_in_us);
        }
	
        printf("\r\n freq_temp %d duty %d\r\n", freq_temp, duty);
	
        while (app_pwm_channel_duty_set(&PWM1, 0, duty) == NRF_ERROR_BUSY);   
    }
    else if(index == 13)    // Move Continuously
    {
        char start[3] = {data_array[0],data_array[1],data_array[2]};// degree
        int start_val = atoi(start);
        servo_degree = start_val;
	
        char end[3] = {data_array[4],data_array[5],data_array[6]};// degree
        int end_val = atoi(end);
        servo_degree_max = end_val;
	
        char speed[4] = {data_array[8],data_array[9],data_array[10],data_array[11]};//speed
        long valf = atol(speed);
	
        servo_interval = valf;

        servo_timer_stop();
        servo_timer_start(10);
    }
	
}
	
void servo_timer_start(uint32_t interval) 
{
    uint32_t err_code;

    err_code = app_timer_start(m_servo_timer_id, interval, NULL);   
}
	
void servo_timer_stop() 
{
    uint32_t err_code;
	
    err_code = app_timer_stop(m_servo_timer_id); 
}
	
void servo_timer_handler()
{
    if(servo_degree == servo_degree_max) 
    {
        is_forward = false;
    }
    else if(servo_degree == 0) 
    {
        is_forward = true;
    }

    if(is_forward)
    {
        servo_degree++;
    }
    else
    {
        servo_degree--;
    }

    long freq_temp = 0;
    int duty = 0;
        
    if(servo_degree <= 90)
    {
        freq_temp = (1500 - ((90 - servo_degree)/0.097));
        duty = ((freq_temp * 100) / period_in_us);
    }
    else
    {
        freq_temp = (1500 + ((servo_degree - 90)/0.097));
        duty = ((freq_temp * 100) / period_in_us);
    }


    printf("\r\n freq_temp %d duty %d\r\n", freq_temp, duty);
    
    while (app_pwm_channel_duty_set(&PWM1, 0, duty) == NRF_ERROR_BUSY);  

    servo_timer_start(APP_TIMER_TICKS(servo_interval));
    
}
	


