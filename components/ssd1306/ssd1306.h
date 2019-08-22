/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

typedef enum {
    SSD1306_LINE_ALIGN_LEFT = 0,
    SSD1306_LINE_ALIGN_RIGHT,
    SSD1306_LINE_ALIGN_CENTER,
} ssd1306_line_align_t;

esp_err_t ssd1306_init();
void ssd1306_render();
void ssd1306_clear();
esp_err_t ssd1306_add_pixel(uint8_t x, uint8_t y, bool on);
esp_err_t ssd1306_add_string(uint8_t x, uint8_t y, const char *string);
esp_err_t ssd1306_add_string_line(uint8_t line, const char *string,
    ssd1306_line_align_t align, uint8_t offset);
size_t ssd1306_get_string_width(const char *string);
