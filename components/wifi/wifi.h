/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <esp_err.h>

esp_err_t wifi_init();
void wifi_wait_for_ip();
char* wifi_get_ip(char *buf, size_t buf_len);
