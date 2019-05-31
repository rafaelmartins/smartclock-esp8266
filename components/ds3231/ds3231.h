/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <time.h>

esp_err_t ds3231_write(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ds3231_read(uint8_t reg, uint8_t *data, size_t len);
esp_err_t ds3231_get_time(struct tm *t);
esp_err_t ds3231_set_time(struct tm *t);
