/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <errno.h>
#include <string.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <lwip/apps/sntp.h>
#include <esp_err.h>
#include <esp_log.h>

#include <i2c/i2c.h>
#include <wifi/wifi.h>

#include "./ds3231.h"

#define LOG_TAG "ds3231"

static EventGroupHandle_t eg;


static uint8_t
dec_to_bcd(uint8_t v)
{
    return (v / 10) * 16 + v % 10;
}


static uint8_t
bcd_to_dec(uint8_t v)
{
    return (v / 16) * 10 + v % 16;
}


void
ds3231_init()
{
    eg = xEventGroupCreate();

    setenv("TZ", CONFIG_SMARTCLOCK_ESP8266_TIMEZONE, 1);
    tzset();

    wifi_wait_for_ip();

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
}


void
ds3231_wait_for_sntp()
{
    xEventGroupWaitBits(eg, BIT0, false, true, portMAX_DELAY);
}


esp_err_t
ds3231_write(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_write_data(0x68, reg, data, len);
}


esp_err_t
ds3231_read(uint8_t reg, uint8_t *data, size_t len)
{
    return i2c_read_data(0x68, reg, data, len);

}


esp_err_t
ds3231_get_time(struct tm *t)
{
    uint8_t data[7];

    esp_err_t rv = ds3231_read(0x00, data, 7);
    if (rv != ESP_OK)
        return rv;

    // check page 11 of datasheet for details on the timekeeping registers

    t->tm_sec = bcd_to_dec(data[0]);
    t->tm_min = bcd_to_dec(data[1]);

    // bit 6 up means 12h
    if (data[2] & 0b1000000) {

        // 5 lsb are the hours < 12
        t->tm_hour = bcd_to_dec(data[2] & 0b11111) - 1;

        // bit 5 up means PM
        if (data[2] & 0b100000)
            t->tm_hour += 12;
    }
    else {
        t->tm_hour = bcd_to_dec(data[2]);
    }

    // ds3231's week day starts from 1 instead of 0
    t->tm_wday = bcd_to_dec(data[3]) - 1;

    t->tm_mday = bcd_to_dec(data[4]);

    // 5 lsb are the month, starting from 1 instead of 0
    t->tm_mon  = bcd_to_dec(data[5] & 0b11111) - 1;

    // ds3231's base year is 2000, but `struct tm`'s tm_year starts from 1900
    t->tm_year = 100;

    // bit 7 up means year > 2099 && < 2200 (this is the year limit of ds3231)
    if (data[5] & 0b10000000)
        t->tm_year += 100;

    t->tm_year += bcd_to_dec(data[6]);

    t->tm_isdst = 0;

    return rv;
}


void
ds3231_set_time(time_t sec, suseconds_t us)
{
    ESP_LOGI(LOG_TAG, "Updating time from SNTP ...");

    struct tm t;
    if (NULL == localtime_r(&sec, &t)) {
        ESP_LOGE(LOG_TAG, "Failed to convert SNTP time to localtime: %s",
            strerror(errno));
        xEventGroupClearBits(eg, BIT0);
        return;
    }

    struct timeval tv = {
        .tv_sec = sec,
        .tv_usec = us,
    };

    if (-1 == settimeofday(&tv, NULL)) {
        ESP_LOGE(LOG_TAG, "Failed to set system time: %s", strerror(errno));
        xEventGroupClearBits(eg, BIT0);
        return;
    }

    uint8_t data[7];
    data[0] = dec_to_bcd(t.tm_sec);
    data[1] = dec_to_bcd(t.tm_min);

    // always set on 24h format
    data[2] = dec_to_bcd(t.tm_hour);

    // ds3231 week day starts from 1 instead of 0
    data[3] = dec_to_bcd(t.tm_wday + 1);

    data[4] = dec_to_bcd(t.tm_mday);

    // 5 lsb are the month, starting from 1 instead of 0
    data[5] = dec_to_bcd(t.tm_mon + 1);

    data[6] = dec_to_bcd(t.tm_year - 100);

    // set bit 7 of byte 5 if year > 2099
    if (t.tm_year > 2099)
        data[5] |= 0b10000000;

    esp_err_t rv = ds3231_write(0x00, data, 7);
    if (rv != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to set system time to DS3231: %s",
            esp_err_to_name(rv));
        xEventGroupClearBits(eg, BIT0);
        return;
    }

    xEventGroupSetBits(eg, BIT0);
}
