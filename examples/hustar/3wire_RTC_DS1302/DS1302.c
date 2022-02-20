#include "DS1302.h"

int _ioPin  = 0;
int _clkPin = 0;
int _cePin  = 0;

void DS1302_Init(int SCLK, int IO, int CS)
{
    _ioPin  = IO;
    _clkPin = SCLK;
    _cePin  = CS;

    nrf_gpio_cfg_output(_clkPin);
    nrf_gpio_cfg_output(_ioPin);
    nrf_gpio_cfg_output(_cePin);

    nrf_gpio_pin_clear(_clkPin);
    nrf_gpio_pin_clear(_ioPin);
    nrf_gpio_pin_clear(_cePin);
    nrf_delay_us(4);
}


void DS1302_Write_data(uint8_t value)
{
    nrf_gpio_cfg_output(_ioPin);
    nrf_gpio_pin_clear(_ioPin);
    
    for (uint8_t bit = 0; bit < 8; bit++) 
    {
        if(value & 0x01)
        {
            nrf_gpio_pin_set(_ioPin);          
        }
        else
        {
            nrf_gpio_pin_clear(_ioPin);
        }
        nrf_delay_us(1);
        nrf_gpio_pin_set(_clkPin);
        nrf_delay_us(1);
        nrf_gpio_pin_clear(_clkPin);
        
        value >>= 1;
    }
}

uint8_t DS1302_Read_data(uint8_t Reg)
{
    uint8_t value = 0;
    
    nrf_gpio_pin_set(_cePin);
    if(_ioPin != 0)
    {    
        DS1302_Write_data(Reg);
        
        nrf_gpio_cfg_input(_ioPin, NRF_GPIO_PIN_NOPULL);
        nrf_delay_us(1);
        
        for (uint8_t bit = 0; bit < 8; bit++) 
        {
            value |= (nrf_gpio_pin_read(_ioPin) << bit);

            nrf_gpio_pin_set(_clkPin);
            nrf_delay_us(1);
            
            nrf_gpio_pin_clear(_clkPin);
            nrf_delay_us(1);
        }
    }
    else
    {
        value = (-1);
    }
    nrf_gpio_pin_clear(_cePin);
    return value;
}

/*
DS1302_WRITE_ADDDRESS
#define WRITE_SECONDS       0x80
#define WRITE_MINUTES       0x82
#define WRITE_HOUR          0x84
#define WRITE_DATE          0x86
#define WRITE_MONTH         0x88
#define WRITE_DAY           0x8A
#define WRITE_YEAR          0x8C
*/

void DS1302_Set_Time(time_config_t time)
{
    uint8_t m_Year      = ((time._Year/10)<<4) | (time._Year%10);       
    uint8_t m_Month     = ((time._Month/10)<<4) | (time._Month%10);       
    uint8_t m_Date      = ((time._Date/10)<<4) | (time._Date%10);     
    uint8_t m_Day       = ((time._Day/10)<<4) | (time._Day%10);     
    uint8_t m_Hour      = ((((time._Hour/10)<<4) | (time._Hour%10)) & 0x7F);     //Only 24H mode
    uint8_t m_Minute    = ((time._Minute/10)<<4) | (time._Minute%10);     
    uint8_t m_Seconds   = ((time._Seconds/10)<<4) | (time._Seconds%10);    
    
    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_YEAR);
    DS1302_Write_data(m_Year);
    nrf_gpio_pin_clear(_cePin);
    
    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_MONTH);
    DS1302_Write_data(m_Month);
    nrf_gpio_pin_clear(_cePin);

    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_DATE);
    DS1302_Write_data(m_Date);
    nrf_gpio_pin_clear(_cePin);

    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_DAY);
    DS1302_Write_data(m_Day);
    nrf_gpio_pin_clear(_cePin);

    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_HOUR);
    DS1302_Write_data(m_Hour);
    nrf_gpio_pin_clear(_cePin);

    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_MINUTES);
    DS1302_Write_data(m_Minute);
    nrf_gpio_pin_clear(_cePin);

    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_SECONDS);
    DS1302_Write_data(m_Seconds);
    nrf_gpio_pin_clear(_cePin);

    
}

/*
DS1302_READ_ADDDRESS
#define READ_SECONDS       0x81
#define READ_MINUTES       0x83
#define READ_HOUR          0x85
#define READ_DATE          0x87
#define READ_MONTH         0x89
#define READ_DAY           0x8B
#define READ_YEAR          0x8D
*/

time_config_t DS1302_Get_Time(void)
{
    uint8_t m_Year      =   DS1302_Read_data(READ_YEAR);
    uint8_t m_Month     =   DS1302_Read_data(READ_MONTH);
    uint8_t m_Date      =   DS1302_Read_data(READ_DATE);
    uint8_t m_Day       =   DS1302_Read_data(READ_DAY);
    uint8_t m_Hour      =   DS1302_Read_data(READ_HOUR);
    uint8_t m_Minute    =   DS1302_Read_data(READ_MINUTES);
    uint8_t m_Seconds   =   DS1302_Read_data(READ_SECONDS);

    time_config_t m_time;

    m_Hour = m_Hour&0x3F;

    m_time._Year     =   (m_Year>>4)*10 + (m_Year&0x0F);
    m_time._Month    =   (m_Month>>4)*10 + (m_Month&0x0F);
    m_time._Date     =   (m_Date>>4)*10 + (m_Date&0x0F);
    m_time._Day      =   m_Day;
    m_time._Hour     =   (m_Hour>>4)*10 + (m_Hour&0x0F);
    m_time._Minute   =   (m_Minute>>4)*10 + (m_Minute&0x0F);
    m_time._Seconds  =   (m_Seconds>>4)*10 + (m_Seconds&0x0F);

    return m_time;
}

/*
DS1302_CHARGER
#define WRITE_CHARGER      0x90
#define READ_CHARGER       0x91
*/
void DS1302_On_Charger(int Diode_sel, int Reg_sel) // Diode = 1 or 2, Reg = 2, 4, 8
{
    uint8_t m_diode_sel = 0;      
    uint8_t m_reg_sel = 0;    
    uint8_t m_charger_mode = 0b10100000;

    
    switch(Diode_sel)
    {
        case 1:            
            m_diode_sel = 0b00000100;
            break;
            
        case 2:            
            m_diode_sel = 0b00001000;
            break;
            
        default:         
            break;
    }

    
    switch(Reg_sel)
    {
        case 2:            
            m_reg_sel = 0b00000001;
            break;
            
        case 4:            
            m_reg_sel = 0b00000010;
            break;
            
        case 8:            
            m_reg_sel = 0b00000011;            
            break;
            
        default:         
            break;
    }
    
    nrf_gpio_pin_set(_cePin);
    nrf_delay_us(1);
    DS1302_Write_data(WRITE_CHARGER);    
    DS1302_Write_data(m_diode_sel|m_reg_sel|m_charger_mode);    
    nrf_gpio_pin_clear(_cePin);

}
void DS1302_Off_Charger(void) // Diode = 1 or 2, Reg = 2, 4, 8
{
    DS1302_Write_data(WRITE_CHARGER);    
    DS1302_Write_data(0x00);   
}

uint8_t DS1302_Read_Charger()
{
    uint8_t value = 0;

    value = DS1302_Read_data(READ_CHARGER);

    return value;
}

