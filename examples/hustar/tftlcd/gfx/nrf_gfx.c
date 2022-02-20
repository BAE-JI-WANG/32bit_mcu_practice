/**
 * Copyright (c) 2017 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include "sdk_common.h"

#if NRF_MODULE_ENABLED(NRF_GFX)

#include "nrf_gfx.h"
#include <stdlib.h>
#include "app_util_platform.h"
#include "nrf_assert.h"

#define NRF_LOG_MODULE_NAME gfx
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

static int space_offset = 3;

static inline void pixel_draw(nrf_lcd_t const * p_instance,
                              uint16_t x,
                              uint16_t y,
                              uint32_t color)
{
    uint16_t lcd_width = nrf_gfx_width_get(p_instance);
    uint16_t lcd_height = nrf_gfx_height_get(p_instance);

    if ((x >= lcd_width) || (y >= lcd_height))
    {
        return;
    }

    p_instance->lcd_pixel_draw(x, y, color);
}

static void rect_draw(nrf_lcd_t const * p_instance,
                      uint16_t x,
                      uint16_t y,
                      uint16_t width,
                      uint16_t height,
                      uint32_t color)
{
    uint16_t lcd_width = nrf_gfx_width_get(p_instance);
    uint16_t lcd_height = nrf_gfx_height_get(p_instance);

    if ((x >= lcd_width) || (y >= lcd_height))
    {
        return;
    }

    if (width > (lcd_width - x))
    {
        width = lcd_width - x;
    }

    if (height > (lcd_height - y))
    {
        height = lcd_height - y;
    }

    p_instance->lcd_rect_draw(x, y, width, height, color);
}

static void line_draw(nrf_lcd_t const * p_instance,
                      uint16_t x_0,
                      uint16_t y_0,
                      uint16_t x_1,
                      int16_t y_1,
                      uint32_t color)
{
    uint16_t x = x_0;
    uint16_t y = y_0;
    int16_t d;
    int16_t d_1;
    int16_t d_2;
    int16_t ai;
    int16_t bi;
    int16_t xi = (x_0 < x_1) ? 1 : (-1);
    int16_t yi = (y_0 < y_1) ? 1 : (-1);
    bool swapped = false;

    d_1 = abs(x_1 - x_0);
    d_2 = abs(y_1 - y_0);

    pixel_draw(p_instance, x, y, color);

    if (d_1 < d_2)
    {
        d_1 = d_1 ^ d_2;
        d_2 = d_1 ^ d_2;
        d_1 = d_2 ^ d_1;
        swapped = true;
    }

    ai = (d_2 - d_1) * 2;
    bi = d_2 * 2;
    d = bi - d_1;

    while ((y != y_1) || (x != x_1))
    {
        if (d >= 0)
        {
            x += xi;
            y += yi;
            d += ai;
        }
        else
        {
            d += bi;
            if (swapped)
            {
                y += yi;
            }
            else
            {
                x += xi;
            }
        }
        pixel_draw(p_instance, x, y, color);
    }
}

static void write_character(nrf_lcd_t const * p_instance,       //nrf_lcd_st7789
                            nrf_gfx_font_desc_t const * p_font, //arial_72ptFontInfo
                            uint8_t character,                  //character
                            uint16_t * p_x,                     //x starting point
                            uint16_t y,                         //y starting point
                            uint16_t font_color)                //16-bit color
{
    uint8_t char_idx = character - p_font->startChar; //character - 33 ("!" ASCII)
    uint16_t bytes_in_line = CEIL_DIV(p_font->charInfo[char_idx].widthBits, 8); //# bytes per line

    if (character == ' ')           //in case of a space
    {
        *p_x += p_font->height / space_offset; //increase x position by half font's height
        return;
    }
    
    for (uint16_t i = 0; i < p_font->height; i++) //Increment through the rows
    {
        for (uint16_t j = 0; j < bytes_in_line; j++)  //Increment through the columns
        {
            for (uint8_t k = 0; k < 8; k++) //Check 0 to 7 per Byte
            {
                if ((1 << (7 - k)) & p_font->data[p_font->charInfo[char_idx].offset + i * bytes_in_line + j])
                {
                    pixel_draw(p_instance, *p_x + j * 8 + k, y + i, font_color);
                    //p_instance = nrf_lcd_st7789
                    //p_x = X starting position   -> X starting position + Column * 8 + Bit # 
                    //y = y starting position     -> Y starting position + Row
                    //font_color = 16-bit color
                }
                else
                {
                    pixel_draw(p_instance, *p_x + j * 8 + k, y + i, 0xFFFF);
                }
            }
        }
    }
    //After through entire character incremented through

    *p_x += p_font->charInfo[char_idx].widthBits + p_font->spacePixels;
    //X starting position + character's width in bits + size of space in pixels
}

ret_code_t nrf_gfx_init(nrf_lcd_t const * p_instance)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state == NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_instance->lcd_init != NULL);
    ASSERT(p_instance->lcd_uninit != NULL);
    ASSERT(p_instance->lcd_pixel_draw != NULL);
    ASSERT(p_instance->lcd_rect_draw != NULL);
    ASSERT(p_instance->lcd_display != NULL);
    ASSERT(p_instance->lcd_rotation_set != NULL);
    ASSERT(p_instance->lcd_display_invert != NULL);
    ASSERT(p_instance->p_lcd_cb != NULL);

    ret_code_t err_code;

    err_code = p_instance->lcd_init();

    if (err_code == NRF_SUCCESS)
    {
        p_instance->p_lcd_cb->state = NRFX_DRV_STATE_INITIALIZED;
    }

    return err_code;
}

void nrf_gfx_uninit(nrf_lcd_t const * p_instance)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);

    p_instance->p_lcd_cb->state = NRFX_DRV_STATE_UNINITIALIZED;

    p_instance->lcd_uninit();
}

void nrf_gfx_point_draw(nrf_lcd_t const * p_instance,
                        nrf_gfx_point_t const * p_point,
                        uint32_t color)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_point != NULL);

    pixel_draw(p_instance, p_point->x, p_point->y, color);
}

ret_code_t nrf_gfx_line_draw(nrf_lcd_t const * p_instance,
                             nrf_gfx_line_t const * p_line,
                             uint32_t color)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_line != NULL);

    uint16_t x_thick = 0;
    uint16_t y_thick = 0;

    if (((p_line->x_start > nrf_gfx_width_get(p_instance)) &&
        (p_line->x_end > nrf_gfx_height_get(p_instance))) ||
        ((p_line->y_start > nrf_gfx_width_get(p_instance)) &&
        (p_line->y_end > nrf_gfx_height_get(p_instance))))
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    if (abs(p_line->x_start - p_line->x_end) > abs(p_line->y_start - p_line->y_end))
    {
        y_thick = p_line->thickness;
    }
    else
    {
        x_thick = p_line->thickness;
    }

    if ((p_line->x_start == p_line->x_end) || (p_line->y_start == p_line->y_end))
    {
        rect_draw(p_instance,
                  p_line->x_start,
                  p_line->y_start,
                  abs(p_line->x_end - p_line->x_start) + x_thick,
                  abs(p_line->y_end - p_line->y_start) + y_thick,
                  color);
    }
    else
    {
        if (x_thick > 0)
        {
            for (uint16_t i = 0; i < p_line->thickness; i++)
            {
                line_draw(p_instance,
                          p_line->x_start + i,
                          p_line->y_start,
                          p_line->x_end + i,
                          p_line->y_end,
                          color);
            }
        }
        else if (y_thick > 0)
        {
            for (uint16_t i = 0; i < p_line->thickness; i++)
            {
                line_draw(p_instance,
                          p_line->x_start,
                          p_line->y_start + i,
                          p_line->x_end,
                          p_line->y_end + i,
                          color);
            }
        }
        else
        {
            line_draw(p_instance,
                      p_line->x_start + x_thick,
                      p_line->y_start + y_thick,
                      p_line->x_end + x_thick,
                      p_line->y_end + y_thick,
                      color);
        }
    }

    return NRF_SUCCESS;
}

ret_code_t nrf_gfx_circle_draw(nrf_lcd_t const * p_instance,
                               nrf_gfx_circle_t const * p_circle,
                               uint32_t color,
                               bool fill)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_circle != NULL);

    int16_t y = 0;
    int16_t err = 0;
    int16_t x = p_circle->r;

    if ((p_circle->x - p_circle->r > nrf_gfx_width_get(p_instance))     ||
        (p_circle->y - p_circle->r > nrf_gfx_height_get(p_instance)))
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    while (x >= y)
    {
        if (fill)
        {
            if ((-y + p_circle->x < 0) || (-x + p_circle->x < 0))
            {
                rect_draw(p_instance, 0, (-x + p_circle->y), (y + p_circle->x + 1), 1, color);
                rect_draw(p_instance, 0, (-y + p_circle->y), (x + p_circle->x + 1), 1, color);
                rect_draw(p_instance, 0, (y + p_circle->y), (x + p_circle->x + 1), 1, color);
                rect_draw(p_instance, 0, (x + p_circle->y), (y + p_circle->x + 1), 1, color);
            }
            else
            {
                rect_draw(p_instance, (-y + p_circle->x), (-x + p_circle->y), (2 * y + 1), 1, color);
                rect_draw(p_instance, (-x + p_circle->x), (-y + p_circle->y), (2 * x + 1), 1, color);
                rect_draw(p_instance, (-x + p_circle->x), (y + p_circle->y), (2 * x + 1), 1, color);
                rect_draw(p_instance, (-y + p_circle->x), (x + p_circle->y), (2 * y + 1), 1, color);
            }
        }
        else
        {
            pixel_draw(p_instance, (y + p_circle->x), (x + p_circle->y), color);
            pixel_draw(p_instance, (-y + p_circle->x), (x + p_circle->y), color);
            pixel_draw(p_instance, (x + p_circle->x), (y + p_circle->y), color);
            pixel_draw(p_instance, (-x + p_circle->x), (y + p_circle->y), color);
            pixel_draw(p_instance, (-y + p_circle->x), (-x + p_circle->y), color);
            pixel_draw(p_instance, (y + p_circle->x), (-x + p_circle->y), color);
            pixel_draw(p_instance, (-x + p_circle->x), (-y + p_circle->y), color);
            pixel_draw(p_instance, (x + p_circle->x), (-y + p_circle->y), color);
        }

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }

    return NRF_SUCCESS;
}

ret_code_t nrf_gfx_rect_draw(nrf_lcd_t const * p_instance,
                             nrf_gfx_rect_t const * p_rect,
                             uint16_t thickness,
                             uint32_t color,
                             bool fill)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_rect != NULL);

    uint16_t rect_width = p_rect->width - thickness;
    uint16_t rect_height = p_rect->height - thickness;

    if ((p_rect->width == 1)                ||
        (p_rect->height == 1)               ||
        (thickness * 2 > p_rect->width)     ||
        (thickness * 2 > p_rect->height)    ||
        ((p_rect->x > nrf_gfx_width_get(p_instance)) &&
        (p_rect->y > nrf_gfx_height_get(p_instance))))
    {
        return NRF_ERROR_INVALID_PARAM;
    }


    if (fill)
    {
        rect_draw(p_instance,
                  p_rect->x,
                  p_rect->y,
                  p_rect->width,
                  p_rect->height,
                  color);
    }
    else
    {
        nrf_gfx_line_t line;

        // Top horizontal line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y;
        line.x_end = p_rect->x + p_rect->width;
        line.y_end = p_rect->y;
        line.thickness = thickness;
        (void)nrf_gfx_line_draw(p_instance, &line, color);
        // Bottom horizontal line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y + rect_height;
        line.x_end = p_rect->x + p_rect->width;
        line.y_end = p_rect->y + rect_height;
        (void)nrf_gfx_line_draw(p_instance, &line, color);
        // Left vertical line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y + thickness;
        line.x_end = p_rect->x;
        line.y_end = p_rect->y + rect_height;
        (void)nrf_gfx_line_draw(p_instance, &line, color);
        // Right vertical line.
        line.x_start = p_rect->x + rect_width;
        line.y_start = p_rect->y + thickness;
        line.x_end = p_rect->x + rect_width;
        line.y_end = p_rect->y + rect_height;
        (void)nrf_gfx_line_draw(p_instance, &line, color);
    }

    return NRF_SUCCESS;
}

void nrf_gfx_screen_fill(nrf_lcd_t const * p_instance, uint32_t color)
{
    rect_draw(p_instance, 0, 0, nrf_gfx_width_get(p_instance), nrf_gfx_height_get(p_instance), color);
}

ret_code_t nrf_gfx_bmp565_draw(nrf_lcd_t const * p_instance,
                               nrf_gfx_rect_t const * p_rect,
                               uint16_t const * img_buf)
{
//	    ASSERT(p_instance != NULL);
//	    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
//	    ASSERT(p_rect != NULL);
//	    ASSERT(img_buf != NULL);
//	
//	    if ((p_rect->x > nrf_gfx_width_get(p_instance)) || (p_rect->y > nrf_gfx_height_get(p_instance)))
//	    {
//	        return NRF_ERROR_INVALID_PARAM;
//	    }
//	
//	    size_t idx;
//	    uint16_t pixel;
//	    uint8_t padding = p_rect->width % 2;
//	
//	    for (int32_t i = 0; i < p_rect->height; i++)
//	    {
//	        for (uint32_t j = 0; j < p_rect->width; j++)
//	        {
//	            idx = (uint32_t)((p_rect->height - i - 1) * (p_rect->width + padding) + j);
//	
//	            pixel = (img_buf[idx] >> 8) | (img_buf[idx] << 8);
//	
//	            pixel_draw(p_instance, p_rect->x + j, p_rect->y + i, pixel);
//	        }
//	    }
        ASSERT(p_instance != NULL);
        ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
        ASSERT(p_rect != NULL);
        ASSERT(img_buf != NULL);
    
        if ((p_rect->x > nrf_gfx_width_get(p_instance)) || (p_rect->y > nrf_gfx_height_get(p_instance)))
        {
            return NRF_ERROR_INVALID_PARAM;
        }
    
        //size_t idx;                           //
        uint16_t idx = 0;                        //Offset into data array
        uint8_t font_col[2];                         //Color to write to pixel
        //uint8_t padding = p_rect->width % 2;  //No padding
    
    nrf_gfx_display(p_instance, 0, 0xFFFF, p_rect->x, p_rect->y, p_rect->x + p_rect->width -1, p_rect->y + p_rect->height);
    
        for (int32_t i = 0; i < p_rect->height; i++)
        {
            for (uint32_t j = 0; j < p_rect->width; j++)
            {
                //idx = (uint32_t)((p_rect->height - i - 1) * (p_rect->width + padding) + j);
    
                //pixel = img_buf[idx]; //Color stored in Big Endian
    
                //pixel_draw(p_instance, p_rect->x + j, p_rect->y + i, img_buf[idx]);
//	                font_col[0] = img_buf[idx] >> 8;
//	                font_col[1] = img_buf[idx];
                font_col[0] = img_buf[idx];
                font_col[1] = img_buf[idx] >> 8;                
                nrf_gfx_display(p_instance, font_col, sizeof(font_col),0,0,0,0);
    
                idx++;
            }
        }

    return NRF_SUCCESS;
}

void nrf_gfx_background_set(nrf_lcd_t const * p_instance, uint16_t const * img_buf)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(img_buf != NULL);

    const nrf_gfx_rect_t rectangle =
    {
        .x = 0,
        .y = 0,
        .width = nrf_gfx_width_get(p_instance),
        .height = nrf_gfx_height_get(p_instance)
    };

    (void)nrf_gfx_bmp565_draw(p_instance, &rectangle, img_buf);
}

void nrf_gfx_display(nrf_lcd_t const * p_instance, uint8_t * dat, uint16_t len, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    ASSERT(p_instance != NULL);

    p_instance->lcd_display(dat, len, x0, y0, x1, y1);
}

void nrf_gfx_rotation_set(nrf_lcd_t const * p_instance, nrf_lcd_rotation_t rotation)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);

    bool rotated = (bool)(p_instance->p_lcd_cb->rotation % 2);

    uint16_t height = !rotated ? nrf_gfx_height_get(p_instance) :
                                 nrf_gfx_width_get(p_instance);
    uint16_t width = !rotated ? nrf_gfx_width_get(p_instance) :
                                nrf_gfx_height_get(p_instance);

    p_instance->p_lcd_cb->rotation = rotation;

    switch (rotation) {
        case NRF_LCD_ROTATE_0:
            p_instance->p_lcd_cb->height = height;
            p_instance->p_lcd_cb->width = width;
            break;
        case NRF_LCD_ROTATE_90:
            p_instance->p_lcd_cb->height = width;
            p_instance->p_lcd_cb->width = height;
            break;
        case NRF_LCD_ROTATE_180:
            p_instance->p_lcd_cb->height = height;
            p_instance->p_lcd_cb->width = width;
            break;
        case NRF_LCD_ROTATE_270:
            p_instance->p_lcd_cb->height = width;
            p_instance->p_lcd_cb->width = height;
            break;
        default:
            break;
    }

    p_instance->lcd_rotation_set(rotation);
}

void nrf_gfx_invert(nrf_lcd_t const * p_instance, bool invert)
{
    ASSERT(p_instance != NULL);

    p_instance->lcd_display_invert(invert);
}

ret_code_t nrf_gfx_print(nrf_lcd_t const * p_instance,
                         nrf_gfx_point_t const * p_point,
                         uint16_t font_color,
                         const char * string,
                         const nrf_gfx_font_desc_t * p_font,
                         bool wrap)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_point != NULL);
    ASSERT(string != NULL);
    ASSERT(p_font != NULL);

    uint16_t x = p_point->x;
    uint16_t y = p_point->y;

    if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
    {
        // Not enough space to write even single char.
        return NRF_ERROR_INVALID_PARAM;
    }

    for (size_t i = 0; string[i] != '\0' ; i++)
    {
        if (string[i] == '\n')
        {
            x = p_point->x;
            y += p_font->height + p_font->height / 10;
        }
        else
        {
            write_character(p_instance, p_font, (uint8_t)string[i], &x, y, font_color);
        }

        uint8_t char_idx = string[i] - p_font->startChar;
        uint16_t char_width = string[i] == ' ' ? (p_font->height / space_offset) :
                                                p_font->charInfo[char_idx].widthBits;

        if (x > (nrf_gfx_width_get(p_instance) - char_width))
        {
            if (wrap)
            {
                x = p_point->x;
                y += p_font->height + p_font->height / 10;
            }
            else
            {
                break;
            }

            if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
            {
                break;
            }
        }
    }

    return NRF_SUCCESS;
}

uint16_t nrf_gfx_height_get(nrf_lcd_t const * p_instance)
{
    ASSERT(p_instance != NULL);

    return p_instance->p_lcd_cb->height;
}

uint16_t nrf_gfx_width_get(nrf_lcd_t const * p_instance)
{
    ASSERT(p_instance != NULL);

    return p_instance->p_lcd_cb->width;
}


//added in

//#define enable_prints

static void write_character_fast(nrf_lcd_t const * p_instance,  //nrf_lcd_st7789
                            nrf_gfx_font_desc_t const * p_font, //arial_72ptFontInfo
                            uint8_t character,                  //character
                            uint16_t * p_x,                     //x starting point
                            uint16_t y,                         //y starting point
                            uint16_t background_color,          //16-bit color
                            uint16_t font_color,                 //16-bit color
                            int space_pixel)
{
    uint8_t char_idx = character - p_font->startChar; //character - 33 ("!" ASCII)
    uint16_t bytes_in_line = CEIL_DIV(p_font->charInfo[char_idx].widthBits, 8); //# bytes per line
                                      
    if (character == ' ')           //in case of a space
    {
        *p_x += p_font->height / space_offset; //increase x position by half font's height
        return;
    }
    
    uint8_t font_col[2] = {font_color >> 8, font_color};
    uint8_t background_col[2] = {background_color >> 8, background_color};

    //p_instance->lcd_pixel_draw(*p_x, y, background_color);
    nrf_gfx_display(p_instance, 0, 0xFFFF, *p_x, y, *p_x + bytes_in_line * 8 - 1, y + p_font->height);

    for (uint16_t i = 0; i < p_font->height; i++) //Increment through the rows
    {
        for (uint16_t j = 0; j < bytes_in_line; j++)  //Increment through the columns
        {
            for (uint8_t k = 0; k < 8; k++) //Check 0 to 7 per Byte
            {
                if ((1 << (7 - k)) & p_font->data[p_font->charInfo[char_idx].offset + i * bytes_in_line + j])
                {
                    //pixel_draw(p_instance, *p_x + j * 8 + k, y + i, font_color);
                    //p_instance = nrf_lcd_st7789
                    //p_x = X starting position   -> X starting position + Column * 8 + Bit # 
                    //y = y starting position     -> Y starting position + Row
                    //font_color = 16-bit color
                    
                    nrf_gfx_display(p_instance, font_col, sizeof(font_col),0,0,0,0);
                    #ifdef enable_prints
                    printf("1");
                    #endif
                }
                else{

                    nrf_gfx_display(p_instance, background_col, sizeof(background_col),0,0,0,0);
                    #ifdef enable_prints
                    printf("0");
                    #endif
                }
            }
        }
        #ifdef enable_prints
        printf("\n");
        #endif
    }
    //After through entire character incremented through

    *p_x += p_font->charInfo[char_idx].widthBits + space_pixel;
    //X starting position + character's width in bits + size of space in pixels
}

uint16_t nrf_gfx_print_text_fast(nrf_lcd_t const * p_instance,
                        nrf_gfx_rect_t const * p_rect,
                        const char * string,
                        const nrf_gfx_font_desc_t * p_font,
                        const nrf_lcd_text_t * text_config,
                        const nrf_lcd_cursor_t * cursor_config,
                        bool wrap)

{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(string != NULL);
    ASSERT(p_font != NULL);

    uint16_t end_x_position = 0;

    end_x_position = nrf_gfx_print_fast_in_rect(p_instance, p_rect, string, p_font, text_config, cursor_config, true);    

    return end_x_position;
}


uint16_t nrf_gfx_print_fast_in_rect(nrf_lcd_t const * p_instance,
                         nrf_gfx_rect_t const * p_rect,
                         const char * string,
                         const nrf_gfx_font_desc_t * p_font,
                         const nrf_lcd_text_t * text_config,
                         const nrf_lcd_cursor_t * cursor_config,
                         bool wrap)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_rect != NULL);
    ASSERT(string != NULL);
    ASSERT(p_font != NULL);
    
    uint16_t x = p_rect->x;
    uint16_t y = p_rect->y;

    uint16_t string_width = 0;

    if(text_config->disp_type == DISP_TYPE_FILL)
    {
        rect_draw(p_instance, p_rect->x, p_rect->y, p_rect->width, p_rect->height, text_config->bg_color);
    }

    int spacePixel = text_config->space_pixel >= 0 ? text_config->space_pixel : p_font->spacePixels; 
    
    for (size_t i = 0; string[i] != '\0' ; i++)
    {
        uint8_t char_idx = string[i] - p_font->startChar;
        uint16_t char_width = string[i] == ' ' ? (p_font->height / space_offset) :
                                              p_font->charInfo[char_idx].widthBits + spacePixel;
    
        string_width = string_width + char_width;// + p_font->spacePixels;
    }
    string_width -= spacePixel;
	
    if(text_config->horizontal == H_CENTER) 
    {
        x = p_rect->x + (p_rect->width - string_width)/2;
        y = p_rect->y;
    }
    else if(text_config->horizontal == H_RIGHT) 
    {
        x = p_rect->x + p_rect->width - string_width;
        y = p_rect->y;
    }
	
    if(text_config->vertical == V_CENTER) 
    {
        y = p_rect->y + (p_rect->height - p_font->height)/2;
    }
    else if(text_config->vertical == V_BOTTOM) 
    {
        y = p_rect->y + p_rect->height - p_font->height;
    }
	

    if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
    {
        // Not enough space to write even single char.
        return -1;//NRF_ERROR_INVALID_PARAM;
    }

    for (size_t i = 0; string[i] != '\0' ; i++)
    {
        if (string[i] == '\n')
        {
            x = p_rect->x;
            y += p_font->height + p_font->height / 10;
        }
        else
        {
            write_character_fast(p_instance, p_font, (uint8_t)string[i], &x, y, text_config->bg_color, text_config->font_color, spacePixel);
        }

        uint8_t char_idx = string[i] - p_font->startChar;
        uint16_t char_width = string[i] == ' ' ? (p_font->height / space_offset) :
                                                p_font->charInfo[char_idx].widthBits;

        if (x > (nrf_gfx_width_get(p_instance) - char_width))
        {
            if (wrap)
            {
                x = p_rect->x;
                y += p_font->height + p_font->height / 10;
            }
            else
            {
                break;
            }

            if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
            {
                break;
            }
        }
    }

    if(cursor_config->cursor_enable)
    {
        rect_draw(
              p_instance,
              x - cursor_config->added_width,
              y + p_font->height + cursor_config->margin_bottom,
              string_width + cursor_config->added_width*2,
              3,
              cursor_config->cursor_color);
    }

    return (x + string_width);

//	    return NRF_SUCCESS;
}

ret_code_t nrf_gfx_print_fast(nrf_lcd_t const * p_instance,
                         nrf_gfx_point_t const * p_point,
                         uint16_t background_color,
                         uint16_t font_color,
                         const char * string,
                         const nrf_gfx_font_desc_t * p_font,
                         int space_pixel,
                         bool wrap)
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_point != NULL);
    ASSERT(string != NULL);
    ASSERT(p_font != NULL);

    uint16_t x = p_point->x;
    uint16_t y = p_point->y;

    if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
    {
        // Not enough space to write even single char.
        return NRF_ERROR_INVALID_PARAM;
    }

    for (size_t i = 0; string[i] != '\0' ; i++)
    {
        if (string[i] == '\n')
        {
            x = p_point->x;
            y += p_font->height + p_font->height / 10;
        }
        else
        {
            write_character_fast(p_instance, p_font, (uint8_t)string[i], &x, y, background_color, font_color, space_pixel);
        }

        uint8_t char_idx = string[i] - p_font->startChar;
        uint16_t char_width = string[i] == ' ' ? (p_font->height / space_offset) :
                                                p_font->charInfo[char_idx].widthBits;

        if (x > (nrf_gfx_width_get(p_instance) - char_width))
        {
            if (wrap)
            {
                x = p_point->x;
                y += p_font->height + p_font->height / 10;
            }
            else
            {
                break;
            }

            if (y > (nrf_gfx_height_get(p_instance) - p_font->height))
            {
                break;
            }
        }
    }

    return NRF_SUCCESS;
}

ret_code_t nrf_gfx_bmp565_draw_big_endian(nrf_lcd_t const * p_instance,   //LCD screen type instance
                                          nrf_gfx_rect_t const * p_rect,  //Rectangle object
                                          uint16_t const * img_buf)       //Array of color data
{
    ASSERT(p_instance != NULL);
    ASSERT(p_instance->p_lcd_cb->state != NRFX_DRV_STATE_UNINITIALIZED);
    ASSERT(p_rect != NULL);
    ASSERT(img_buf != NULL);

    if ((p_rect->x > nrf_gfx_width_get(p_instance)) || (p_rect->y > nrf_gfx_height_get(p_instance)))
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    //size_t idx;                           //
    uint16_t idx = 0;                        //Offset into data array
    uint8_t font_col[2];                         //Color to write to pixel
    //uint8_t padding = p_rect->width % 2;  //No padding

    nrf_gfx_display(p_instance, 0, 0xFFFF, p_rect->x, p_rect->y, p_rect->x + p_rect->width -1, p_rect->y + p_rect->height);

    for (int32_t i = 0; i < p_rect->height; i++)
    {
        for (uint32_t j = 0; j < p_rect->width; j++)
        {
            //idx = (uint32_t)((p_rect->height - i - 1) * (p_rect->width + padding) + j);

            //pixel = img_buf[idx]; //Color stored in Big Endian

            //pixel_draw(p_instance, p_rect->x + j, p_rect->y + i, img_buf[idx]);
            font_col[0] = img_buf[idx] >> 8;
            font_col[1] = img_buf[idx];
            nrf_gfx_display(p_instance, font_col, sizeof(font_col),0,0,0,0);

            idx++;
        }
    }

    return NRF_SUCCESS;
}

//end of added in

#endif //NRF_MODULE_ENABLED(NRF_GFX)

