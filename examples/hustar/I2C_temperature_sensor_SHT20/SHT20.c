#include "SHT20.h"


/**
 * @brief Function for initializing the TWI peripheral.
 */
 /* TWI instance. */

static const nrf_drv_twi_t m_twi_SHT_2x = NRF_DRV_TWI_INSTANCE(0);

uint8_t g_USER_Data = 0;


ret_code_t SHT_2x_init (int SDA, int SCL)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t i2c_config = {
       .scl                = SCL,
       .sda                = SDA,
       .frequency          = NRF_DRV_TWI_FREQ_250K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi_SHT_2x, &i2c_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);
    
#ifdef DEBUG   
        printf("SHT_2x_init %d \r\n", err_code);
#endif   
    
    nrf_drv_twi_enable(&m_twi_SHT_2x);    

    return err_code;
}

ret_code_t SHT_2x_register_write(uint8_t reg_addr, uint8_t * p_tx_data, uint8_t bytes)
{
    ret_code_t ret_code;
    
    uint8_t tx_data[bytes+1];
    
    tx_data[0] = reg_addr;
    
    for(uint8_t i = 0 ; i<bytes ; i++) 
    {
        tx_data[i+1] = p_tx_data[i];
    }   
   
    ret_code = nrf_drv_twi_tx(&m_twi_SHT_2x, SHT_I2C_ADDR, tx_data, sizeof(tx_data), false);
    
    return ret_code;
}

uint8_t SHT_2x_register_read(uint8_t reg_addr)
{
  ret_code_t ret_code;

  uint8_t read_data[2];
  uint8_t read_addr = reg_addr;    

  ret_code = nrf_drv_twi_tx(&m_twi_SHT_2x,SHT_I2C_ADDR, &read_addr, 1, false);

  if(ret_code != NRF_SUCCESS)
  {
      return ret_code;
  }

  ret_code = nrf_drv_twi_rx(&m_twi_SHT_2x, SHT_I2C_ADDR, read_data, 1);

  return read_data[0];
}


bool checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor) 
{ 
    uint32_t remainder = (uint32_t)message_from_sensor << 8; 
    remainder |= check_value_from_sensor; 
    
    uint32_t divsor = (uint32_t)SHIFTED_DIVISOR; 

    for(int i = 0 ; i < 16 ; i++)
    { 
        if(remainder & (uint32_t)1 << (23 - i)){ 
        remainder ^= divsor; 
        } 
        divsor >>= 1; 
    }
    
    return (bool)remainder; 
} 


uint16_t SHT_2x_register_read_2(uint8_t reg_addr)
{
  ret_code_t ret_code;

  uint8_t read_data[3];
  uint8_t read_addr = reg_addr;    

  uint8_t MSB = (-1);
  uint8_t LSB = (-1);
  uint8_t CRC = (-1);
  uint16_t rawValue = 0; 

  ret_code = nrf_drv_twi_tx(&m_twi_SHT_2x,SHT_I2C_ADDR, &read_addr, 1, false);

  if(ret_code != NRF_SUCCESS)
  {
      return ret_code;
  }

  ret_code = nrf_drv_twi_rx(&m_twi_SHT_2x, SHT_I2C_ADDR, read_data, 3);

  MSB = read_data[0];
  LSB = read_data[1];
  CRC = read_data[2];

  rawValue = ((uint16_t)MSB<<8)|(uint16_t)LSB;
  
  if(checkCRC(rawValue, CRC) != 0)
  { 
    return (ERROR_BAD_CRC); 
  } 

  return rawValue & 0xFFFC;
}


ret_code_t SHT_2x_check(void)
{   
    uint8_t userData = 0;

    userData = SHT_2x_register_read(READ_USER_REG); //(I2C Read Function)     

    g_USER_Data = userData;
        
    return userData;    
}


ret_code_t SHT_2x_softReset()
{        
     return 0;
}

ret_code_t SHT_2x_writeUserRegister()
{
   
}

ret_code_t SHT_2x_setHumidityResolution()
{
   
}

ret_code_t SHT_2x_setTemperatureResolution()
{
   
}

ret_code_t SHT_2x_setDefaultData()
{
    ret_code_t ret_code;
    
    uint8_t defaultData[2] = {0,};
        
    // 0b0XXXXXX0 ~ // 0b1XXXXXX1    
    // 0b0xxxxxx0 RH = 12bit, T = 14bit     
    // 0b0xxxxxx1 RH = 08bit, T = 12bit
    // 0b1xxxxxx0 RH = 10bit, T = 13bit
    // 0b1xxxxxx1 RH = 11bit, T = 11bit

    // 0bX0XXXXXX ~ // 0bX1XXXXXX    
    // 0bX0XXXXXX   VDD > 2.25
    // 0bX1XXXXXX   VDD < 2.25
    
    // 3,4,5    Reserved
    // 0bX0XXXXXX   VDD > 2.25
    // 0bX1XXXXXX   VDD < 2.25

    // 0bXXXXX0XX ~ // 0bXXXXX1XX
    // 0bXXXXX0XX   Heater OFF
    // 0bXXXXX1XX   Heater ON

    // 0bXXXXXX0X ~ // 0bXXXXXX1X
    // 0bXXXXXX0X   Disable OTP Reload
    // 0bXXXXXX1X   Enable OTP Reload

    defaultData[0] = 0b00000010; // RH 12 Bit, T 14 Bit, VDD > 2.25V, Heater OFF, Disable OTP Reload
        
    ret_code = SHT_2x_register_write(WRITE_USER_REG, defaultData, 1);

    return ret_code;
}


float SHT_2x_readHumidity()
{
    uint16_t rawHumidity = -1;
    float tempRH = 0;
    float rh = 0;

    rawHumidity = SHT_2x_register_read_2(TRIGGER_HUMD_MEASURE_HOLD); //(I2C Read Function) 
    
    if(rawHumidity != ERROR_BAD_CRC)
    {
        tempRH = rawHumidity * (125.0 / 65536.0);
        rh = tempRH - 6.0;
    }
    else
    {
        rh = (-999.0);
    }
    return (rh);    
}
        
    
float SHT_2x_readTemperature()
{
    uint16_t rawTemperature = -1; //(I2C Read Function) 
    
    float tempTemperature = 0;
    float realTemperature = 0;
    
    rawTemperature = SHT_2x_register_read_2(TRIGGER_TEMP_MEASURE_HOLD); //(I2C Read Function)
    if(rawTemperature != ERROR_BAD_CRC)
    {
        tempTemperature = rawTemperature * (175.72 / 65536.0);
        realTemperature = tempTemperature - 46.85;
    }
    else
    {
        realTemperature = (-999.0);
    }

    return (realTemperature);
}



