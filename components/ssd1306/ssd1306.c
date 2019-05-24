/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

// This is a very simple driver for OLED display SSD1306, using I2C interface.
// The driver is specific for the display I own:
//
// https://www.amazon.de/gp/product/B01L9GC470/

#include <stdbool.h>
#include <string.h>
#include <esp_err.h>

#include <i2c/i2c.h>

#include "./font.h"
#include "./ssd1306.h"

static const uint8_t startup_cmds[] = {
    0xAE,        // display off (page 28, 37)
    0xA8, 0x3F,  // set multiplex ratio (page 31, 37, app: page 5)
    0xD3, 0x00,  // set display offset (page 31, 37, app: page 5)
    0x40,        // set display start line (page 31, 36, app: page 5)
    0xA1,        // set segment remap (page 31, 36)
    0xC8,        // set COM output scan direction (page 40-42)
    0xDA, 0x12,  // set COM pins hardware configuration (page 40-42)
    0x81, 0x7F,  // set contrast control (page 36, app: page 5)
    0xA4,        // disable entire display on (page 28, 37)
    0xA6,        // set normal display (page 28, 37, app: page 5)
    0xD5, 0x80,  // set osc frequency (page 40, app: page 5)
    0x8D, 0x14,  // enable charge pump regulator (app: page 3, 5)
    0x20, 0x00,  // set memory addressing mode - horizontal (page 34-35)
    0xD9, 0xF1,  // set pre-charge period (page 32, 40)
    0xDB, 0x40,  // set Vcomh deselect level (page 32, 43)
    0x2E,        // deactivate scroll (page 29, 46)
    0xAF,        // display on (page 28, 37, app: page 5)
    0xFF,
};

static const uint8_t refresh_cmds[] = {
    0x21, 0x00, 0x7F,  // set column address (page 30, 35)
    0x22, 0x00, 0x07,  // set page address (page 31, 36)
    0xFF,
};

static uint8_t fb[1024] = {0};


esp_err_t
ssd1306_init()
{
    for (size_t i = 0; startup_cmds[i] != 0xFF; i++) {
        esp_err_t rv = ssd1306_command(startup_cmds[i]);
        if (rv != ESP_OK)
            return rv;
    }

    esp_err_t rv = ssd1306_clear();
    if (rv != ESP_OK)
        return rv;

    return ssd1306_refresh();
}


esp_err_t
ssd1306_command(uint8_t cmd)
{
    return i2c_write_data(0x3C, 0, &cmd, 1);
}


esp_err_t
ssd1306_refresh()
{
    for (size_t i = 0; refresh_cmds[i] != 0xFF; i++) {
        esp_err_t rv = ssd1306_command(refresh_cmds[i]);
        if (rv != ESP_OK)
            return rv;
    }

    for (size_t i = 0; i < 1024; i += 16) {
        esp_err_t rv = i2c_write_data(0x3C, 0x40, &fb[i], 16);
        if (rv != ESP_OK)
            return rv;
    }

    return ESP_OK;
}


esp_err_t
ssd1306_clear()
{
    memset(fb, 0, 1024);
    return ESP_OK;
}


esp_err_t
ssd1306_add_pixel(uint8_t x, uint8_t y, bool on)
{
    if ((x >= 128) || (y >= 64))
        return ESP_ERR_INVALID_ARG;

    // line_start = line_width * line_number = 128 * (y / 8)
    // index = line_start + x
    uint16_t index = (128 * (y / 8)) + x;

    // bit_ord = y & 0b111
    // bit_selected = 1 << bit_ord
    uint8_t bit_selected = (1 << (y & 0b111));

    if (on)
        fb[index] |= bit_selected;
    else
        fb[index] &= ~bit_selected;

    return ESP_OK;
}


esp_err_t
ssd1306_add_string(uint8_t x, uint8_t y, const char *string)
{
    esp_err_t rv = ESP_OK;

    if (string == NULL)
        return rv;

    for (size_t i = 0; string[i] != '\0';) {
        uint16_t offset = *(font_descriptors + string[i]);
        const uint8_t *bm = font_bitmaps + offset;

        for (uint8_t j = 0; j < FONT_HEIGHT; ++j) {
            for (uint8_t k = 0; k < FONT_WIDTH; ++k) {
                // XXX: won't work for font width >= 8
                rv = ssd1306_add_pixel(x + k, y + j, (bm[j] << k) & 0b10000000);
                if (rv != ESP_OK)
                    return rv;
            }
        }

        x += FONT_WIDTH;
        if (string[++i] != '\0')
            x++;
    }

    return rv;
}


size_t
ssd1306_get_string_width(const char *string)
{
    if (string == NULL)
        return 0;

    size_t x = 0;
    for (size_t i = 0; string[i] != '\0';) {
        x += FONT_WIDTH;
        if (string[++i] != '\0')
            x++;
    }

    return x;
}
