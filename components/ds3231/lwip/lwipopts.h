/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#pragma once

#include <time.h>

#include_next "lwipopts.h"

void ds3231_set_time(time_t sec, suseconds_t us);

#ifdef SNTP_SET_SYSTEM_TIME_US
#undef SNTP_SET_SYSTEM_TIME_US
#endif

#define SNTP_SET_SYSTEM_TIME_US ds3231_set_time
