#include "steppermotor.h"


int loop_count = 0;
int target_loop_count = 0;
int motor_state = 0;
	
uint8_t PIN_A = 18;
uint8_t PIN_B = 17;
uint8_t PIN_C = 16;
uint8_t PIN_D = 15;

bool is_reverse = true;
int interval_t = 30;

int test_count = 0;


APP_TIMER_DEF(m_motor_timer_id);

static void timer_init(void);
void motor_timer_start() ;
void motor_timer_stop();
void motor_timer_handler();



void stepper_motor_init()
{
    timer_init();
    
    nrf_gpio_cfg_output(PIN_A);
    nrf_gpio_cfg_output(PIN_B);
    nrf_gpio_cfg_output(PIN_C);
    nrf_gpio_cfg_output(PIN_D);
}


/**@brief Function for initializing the timer module.
 */
static void timer_init(void)
{
    uint32_t err_code;

    err_code = app_timer_create(&m_motor_timer_id, APP_TIMER_MODE_REPEATED, motor_timer_handler);
}


void stepper_motor_execute_continously()
{
    loop_count = 0;
    target_loop_count = -1;
    interval_t = APP_TIMER_TICKS(5);
    motor_timer_stop();
    motor_timer_start();
}

void stepper_motor_change_direction()
{
    if(is_reverse)
    {
        is_reverse = false;
    }
    else
    {
        is_reverse = true;
    }
}

/* data_array : moving data
        ex> 090 005\n
*/

void stepper_motor_execute_by_angle(uint8_t * data_array)
{
    char angle[3] = {data_array[0],data_array[1],data_array[2]};// angle
    printf("\r\n angle : %s \r\n", angle);
    int angle_val = atoi(angle);
    
    char interval[3] = {data_array[4],data_array[5],data_array[6]};// interval <= motor speed
    printf("\r\n interval : %s \r\n", interval);
    int interval_val = atoi(interval);
    
    target_loop_count = (512 * angle_val ) /360;
    interval_t = APP_TIMER_TICKS(interval_val);
    motor_timer_stop();
    motor_timer_start();
}

void motor_timer_start() 
{
    //printf("\r\n motor_timer_start\r\n");
    loop_count = 0;

    uint32_t err_code;
    uint32_t interval = interval_t;
    
    err_code = app_timer_start(m_motor_timer_id, interval, NULL);
    
}
	
void motor_timer_stop() 
{
    uint32_t err_code;
	
    err_code = app_timer_stop(m_motor_timer_id); 
    test_count = 0;
}
	
void motor_timer_handler()
{
    test_count++;

    if(is_reverse)
    {
        if(motor_state == 0)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_set(PIN_D);
        }
        else if(motor_state == 1)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_set(PIN_D);
        }
        else if(motor_state == 2)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_set(PIN_D);
        }
        else if(motor_state == 3)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 4)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 5)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 6)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 7)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
    	
            loop_count++;
        }
    }
    else 
    {
        if(motor_state == 0)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 1)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 2)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 3)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_set(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 4)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_clear(PIN_D);
        }
        else if(motor_state == 5)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_set(PIN_C);
            nrf_gpio_pin_set(PIN_D);
        }
        else if(motor_state == 6)
        {
            nrf_gpio_pin_clear(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_set(PIN_D);
        }
        else if(motor_state == 7)
        {
            nrf_gpio_pin_set(PIN_A);
            nrf_gpio_pin_clear(PIN_B);
            nrf_gpio_pin_clear(PIN_C);
            nrf_gpio_pin_set(PIN_D);
    	
            loop_count++;
        }
    }
    if(motor_state >= 7)
    {
        motor_state = 0;
    }
    else
    {
        motor_state++;
    }
	
    if(target_loop_count != loop_count)
    {
        //printf("%d", loop);
        //motor_timer_start();
    }
    else 
    {
        printf("\r\n motor_timer_stop %d, %d \r\n", loop_count, test_count);
        loop_count = 0;
        motor_timer_stop();
    }
	
}


