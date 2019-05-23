/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <esp_err.h>

esp_err_t i2c_init();
esp_err_t i2c_write_data(int8_t slave_address, uint8_t reg_address, uint8_t *data,
    size_t data_len);
esp_err_t i2c_read_data(int8_t slave_address, uint8_t reg_address, uint8_t *data,
    size_t data_len);
