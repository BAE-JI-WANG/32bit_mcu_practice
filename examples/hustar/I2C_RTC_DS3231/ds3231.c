#include "ds3231.h"

/* Interrupt Function */
static void irq_handler_init(uint8_t int_pin);
void irq_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
	
	
/* Register Control Function */
ret_code_t write_register(uint8_t reg_addr, uint8_t wr_data);
uint8_t read_register(uint8_t reg_addr);
	
	
/* ETC Function */
void ds3231_delay(uint32_t delay);
	
	
/**
 * @brief Function for initializing the TWI peripheral.
 */
 /* TWI instance. */
#define TWI_INSTANCE  1 /**< TWI instance index. */
static const nrf_drv_twi_t m_twi_ds3231 = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE);
	
	
/* TWI Init Function */
static ret_code_t twi_init(ds3231_twi_pins_config_t const * p_pins_config);
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context);
	
	
/* Common Function */
	
ret_code_t write_register(uint8_t reg_addr, uint8_t wr_data)
{
    ret_code_t ret_code;
	
    uint8_t tx_data[2];
	
    tx_data[0] = reg_addr;
    tx_data[1] = wr_data;
	
    ds3231_delay(10);
	
    ret_code = nrf_drv_twi_tx(&m_twi_ds3231, ds3231_I2C_ADDR, tx_data, sizeof(tx_data), false);
	
    if(ret_code != NRF_SUCCESS) 
    {
        printf("\r\n write : 0x%02X -> %d", reg_addr, ret_code);
    }
	
    ds3231_delay(10);   
	
    return ret_code;
}
	
	
uint8_t read_register(uint8_t reg_addr)
{
    ret_code_t ret_code;
	
    uint8_t read_data[2];
	
    uint8_t read_addr = reg_addr;
	
    ds3231_delay(10);
	
    ret_code = nrf_drv_twi_tx(&m_twi_ds3231, ds3231_I2C_ADDR, &read_addr, 1, false);
	
    if(ret_code != NRF_SUCCESS)
    {
        printf("\r\n ds3231_read first : 0x%02X -> %d \r\n", reg_addr, ret_code);
        return ret_code;
    }
	
    ds3231_delay(10);
	
    ret_code = nrf_drv_twi_rx(&m_twi_ds3231, ds3231_I2C_ADDR, read_data, 1);
	
    if(ret_code != NRF_SUCCESS)
    {
        printf("\r\n ds3231_read second : 0x%02X -> %d \r\n", reg_addr, ret_code);
        return ret_code;
    }
    //printf("\r\n read_register : 0x%02X, 0x%02X \r\n", read_data[0], read_data[1]);
	
    ds3231_delay(10);       
	
    return read_data[0];
}
	
void ds3231_delay(uint32_t delay)
{
    nrf_delay_ms(delay); 
}
	
	
/**
 * @brief TWI setup
 * @param[in] p_instance Pointer to the driver instance structure.
 * @param[in] int_pin interrupt pin number.
 * @param[in] callback receive data callback function. 
 */
ret_code_t ds3231_twi_setup(ds3231_twi_pins_config_t const * p_pins_config)
{
    ret_code_t err_code = twi_init(p_pins_config);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
	
    return NRF_SUCCESS;
}
	
/**
 * @brief TWI initialization.
 *
 * @param[in] p_pins_config Pointer to structere holding pins numbers to be used by TWI.
 */
static ret_code_t twi_init(ds3231_twi_pins_config_t const * p_pins_config)
{
    ret_code_t err_code;
	
    const nrf_drv_twi_config_t twi_ds3231_config = {
       .scl                = p_pins_config->scl_pin,
       .sda                = p_pins_config->sda_pin,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };
	
    err_code = nrf_drv_twi_init(&m_twi_ds3231, &twi_ds3231_config, twi_handler, NULL);
    if (err_code != NRF_SUCCESS)
    {
        printf("\r\n nrf_drv_twi_init error : %d \r\n", err_code);
        return err_code;
    }
	
    nrf_drv_twi_enable(&m_twi_ds3231);
	
    return NRF_SUCCESS;
}
	
	
/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{   
    //printf("\r\n twi_handler : %d\r\n", p_event->type);
	
    switch(p_event->type)
    {
        case NRF_DRV_TWI_EVT_DONE:
	        //printf("\r\n!****************************!\r\nDevice found \r\n!****************************!\r\n\r\n");
            break;
        case NRF_DRV_TWI_EVT_ADDRESS_NACK:
            printf("\r\n twi_handler ERROR : %d \r\n", p_event->type);
            break;
        case NRF_DRV_TWI_EVT_DATA_NACK:
            printf("\r\n twi_handler ERROR : %d \r\n", p_event->type);
            break;
        default:
            printf("\r\n twi_handler ERROR : %d \r\n", p_event->type);
            break;        
    }   
}
	
void ds3231_twi_uninit() 
{
    nrf_drv_twi_disable(&m_twi_ds3231);
    nrf_drv_twi_uninit(&m_twi_ds3231);
}
	
void ds3231_set_current_time(uint8_t * data_array)
{
    printf("\r\n set_current_time \r\n");
	
    uint8_t year_byte = (data_array[DS3231_WRITE_YEAR1] << 4) | (data_array[DS3231_WRITE_YEAR2] & 0x0F); 
    uint8_t month_byte = (data_array[DS3231_WRITE_MONTH1] << 4) | (data_array[DS3231_WRITE_MONTH2] & 0x0F); 
    uint8_t date_byte = (data_array[DS3231_WRITE_DATE1] << 4) | (data_array[DS3231_WRITE_DATE2] & 0x0F); 
	
    uint8_t hour_byte = (data_array[DS3231_WRITE_HOUR1] << 4) | (data_array[DS3231_WRITE_HOUR2] & 0x0F); 
    uint8_t min_byte = (data_array[DS3231_WRITE_MIN1] << 4) | (data_array[DS3231_WRITE_MIN2] & 0x0F);	
    uint8_t sec_byte = (data_array[DS3231_WRITE_SEC1] << 4) | (data_array[DS3231_WRITE_SEC2] & 0x0F);	

     uint8_t day_byte = (data_array[DS3231_WRITE_DAY] & 0x0F); 
        
    write_register(REG_SEC, sec_byte);
    write_register(REG_MIN, min_byte);
    write_register(REG_HOUR, hour_byte);
    write_register(REG_DAY, day_byte);
    write_register(REG_DATE, date_byte);
    write_register(REG_MONTH, month_byte);
    write_register(REG_YEAR, year_byte);
}
	
void ds3231_get_current_time(uint8_t * data_array)
{
    data_array[DS3231_READ_YEAR] = read_register(REG_YEAR);
    data_array[DS3231_READ_MONTH] = read_register(REG_MONTH);
    data_array[DS3231_READ_DATE] = read_register(REG_DATE);
    data_array[DS3231_READ_HOUR] = read_register(REG_HOUR);
    data_array[DS3231_READ_MIN] = read_register(REG_MIN);
    data_array[DS3231_READ_SEC] = read_register(REG_SEC);
    data_array[DS3231_READ_DAY] = read_register(REG_DAY);    
}

float ds3231_get_current_temperature()
{
    uint8_t data_array[2];
    
    data_array[0] = read_register(REG_TEMP_UPPER);
    data_array[1] = read_register(REG_TEMP_LOWER);

    uint16_t temp_value = (data_array[0] << 2) | (data_array[1] >> 6);

    if((temp_value & 0x0200) == 0x0200)
    {
        temp_value = ((~temp_value + 1) & 0x03FF);

        return -(temp_value * 0.25);
    }
    else
    {
        return (temp_value * 0.25);
    }
}

