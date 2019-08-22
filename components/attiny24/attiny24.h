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

esp_err_t attiny24_init();
void attiny24_set_display(uint8_t num);
void attiny24_set_led1(bool on);
void attiny24_set_led2(bool on);
