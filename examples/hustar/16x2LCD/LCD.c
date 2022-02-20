#include "LCD.h"

bool mIsDisplayPause = false;

uint8_t en_pin;
uint8_t rs_pin;
uint8_t bg_pin;
uint8_t db4_pin;
uint8_t db5_pin;
uint8_t db6_pin;
uint8_t db7_pin;


void LCD_ENABLE()
{
    nrf_gpio_pin_set(en_pin);
    nrf_delay_us (10);
    nrf_gpio_pin_clear(en_pin);
    nrf_delay_us (50);
}

void LCD_SET_DATA (uint8_t DATA)
{
    DATA = (DATA&0x0F);
    if (DATA & 0x01)
    {
        nrf_gpio_pin_set(db4_pin);
    }
    else
    {
        nrf_gpio_pin_clear(db4_pin);
    }
    if ((DATA>>1) & 0x01)
    {
        nrf_gpio_pin_set(db5_pin);
    }
    else
    {
        nrf_gpio_pin_clear(db5_pin);
    }
    if ((DATA>>2) & 0x01)
    {
        nrf_gpio_pin_set(db6_pin);
    }
    else
    {
        nrf_gpio_pin_clear(db6_pin);
    }
    if ((DATA>>3) & 0x01)
    {
        nrf_gpio_pin_set(db7_pin);
    }
    else
    {
        nrf_gpio_pin_clear(db7_pin);
    }
}

void LCD_CM_WRITE (uint8_t CM)
{
    nrf_gpio_pin_clear(rs_pin);
    LCD_SET_DATA(CM>>4);
    LCD_ENABLE();
    LCD_SET_DATA(CM & 0x0F);
    LCD_ENABLE();
    nrf_delay_ms(10);
}

void LCD_DATA_WRITE (uint8_t DATA)
{
    nrf_gpio_pin_set(rs_pin);
    LCD_SET_DATA(DATA>>4);
    LCD_ENABLE();
    LCD_SET_DATA(DATA & 0x0F);
    LCD_ENABLE();
    //nrf_delay_ms(10);

}

void LCD_init(lcd_pins_config_t const * p_pins_config)
{
    en_pin = p_pins_config->en_pin;
    rs_pin = p_pins_config->rs_pin;
    bg_pin = p_pins_config->bg_pin;
    db4_pin = p_pins_config->db4_pin;
    db5_pin = p_pins_config->db5_pin;
    db6_pin = p_pins_config->db6_pin;
    db7_pin = p_pins_config->db7_pin;

    nrf_gpio_cfg_output(en_pin); 
    nrf_gpio_cfg_output(rs_pin); 
//	    nrf_gpio_cfg_output(bg_pin);
    nrf_gpio_cfg_output(db4_pin); 
    nrf_gpio_cfg_output(db5_pin); 
    nrf_gpio_cfg_output(db6_pin); 
    nrf_gpio_cfg_output(db7_pin);

    nrf_gpio_pin_clear(en_pin);
    nrf_gpio_pin_clear(rs_pin);
    nrf_gpio_pin_clear(db4_pin);
    nrf_gpio_pin_clear(db5_pin);
    nrf_gpio_pin_clear(db6_pin);
    nrf_gpio_pin_clear(db7_pin);

//	    nrf_gpio_pin_set(bg_pin);

    nrf_delay_ms(15);
    LCD_SET_DATA(0x03);
    nrf_delay_ms(5);
    LCD_SET_DATA(0x03);
    nrf_delay_us(100);
    LCD_SET_DATA(0x03);
    LCD_SET_DATA(0x02);

    LCD_CM_WRITE(0x28);    /* function set */
    LCD_CM_WRITE(0x08);
    LCD_CM_WRITE(0x01);    /* clear display */
    LCD_CM_WRITE(0x06);    /* entry mode, set increment */

    LCD_CM_WRITE(0x0C);    /* display on,cursor off,blink off */

//	    nrf_delay_ms(25);
//	    LCD_SET_DATA(0x03);
//	    nrf_delay_ms(15);
//	    LCD_SET_DATA(0x03);
//	    nrf_delay_us(160);
//	    LCD_SET_DATA(0x03);
//	    nrf_delay_us(160);
//	    LCD_SET_DATA(0x02);
//	
//	    LCD_CM_WRITE(0x28);    /* function set */
//	    //LCD_CM_WRITE(0x08);
//	    LCD_CM_WRITE(0x0C);    /* display on,cursor off,blink off */
//	    LCD_CM_WRITE(0x01);    /* clear display */
//	    LCD_CM_WRITE(0x06);    /* entry mode, set increment */
}

void LCDBuildChar(uint8_t loc, uint8_t *p)
{
     unsigned char i;
     
     if(loc<8)                                 //If valid address
     {

         LCD_CM_WRITE(0x40+(loc*8));               //Write to CGRAM
         for(i=0;i<8;i++)
            LCD_DATA_WRITE(p[i]);                   //Write the character pattern to CGRAM
     }
     LCD_CM_WRITE(0x80);                           //shift back to DDRAM location 0
}

void lcd_gotoxy(unsigned char x, unsigned char y)
{
    if(y!=0) LCD_CM_WRITE(0xC0+x);
    else LCD_CM_WRITE(0x80+x);
    //nrf_delay_us(1000);
}

void LCD_STRING_WRITE(uint8_t x, uint8_t y, uint8_t len, uint8_t *p)
{
    lcd_gotoxy(x, y);
    
    for(int i = 0; i< len; i++)
    {
        LCD_DATA_WRITE(p[i]);  
    }
}


void LCD_SET_CURSOR(uint8_t x, uint8_t y)
{
    lcd_gotoxy(x, y);
    LCD_CM_WRITE(0x0F);
}

void LCD_CLEAR_CURSOR()
{
    LCD_CM_WRITE(0x0C);
}

void LCD_DISPLAY_PAUSE()
{
    mIsDisplayPause = true;
}

void LCD_DISPLAY_START()
{
    mIsDisplayPause = false;
}

void LCD_CLEAR()
{
    LCD_CM_WRITE(0x01);
}

void LCD_SET_BG(bool onoff)
{
//	    if(onoff)
//	    {
//	        nrf_gpio_pin_set(bg_pin);
//	    }
//	    else
//	    {
//	        nrf_gpio_pin_clear(bg_pin);
//	    }
}



