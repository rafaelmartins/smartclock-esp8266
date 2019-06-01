/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

esp_err_t ssd1306_init();
esp_err_t ssd1306_command(uint8_t cmd);
esp_err_t ssd1306_render();
esp_err_t ssd1306_clear();
esp_err_t ssd1306_add_pixel(uint8_t x, uint8_t y, bool on);
esp_err_t ssd1306_add_string(uint8_t x, uint8_t y, const char *string);
size_t ssd1306_get_string_width(const char *string);
