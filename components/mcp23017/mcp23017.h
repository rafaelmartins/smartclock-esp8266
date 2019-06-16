/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <stdbool.h>
#include <esp_err.h>

esp_err_t mcp23017_init();
void mcp23017_set_bit(uint8_t bit, bool value);
esp_err_t mcp23017_send();
void mcp23017_set_display_number(uint8_t num);
void mcp23017_set_led(bool on);
