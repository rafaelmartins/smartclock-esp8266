/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

// simple driver to send data to an attiny24 mcu, that drives two leds and a
// 7 segment display.
//
// its firmware can be found in the `attiny24_fw` directory in the root of this
// repository.

#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_err.h>

#include <i2c/i2c.h>

#include "./attiny24.h"

#define LOG_TAG "attiny24"

static TaskHandle_t th;
static uint8_t display = 0xf;
static bool led1 = false;
static bool led2 = false;


enum {
    DISPLAY = 1 << 0,
    LED1 = 1 << 1,
    LED2 = 1 << 2,
};


static void
task(void *pvParameters)
{
    uint32_t n;
    uint8_t b;

    esp_err_t err;

    while (true) {
        xTaskNotifyWait(0x00, DISPLAY | LED1 | LED2, &n, portMAX_DELAY);

        if (n & DISPLAY) {
            err = i2c_write_data(0x20, 0x00, &display, 1);
            if (err != ESP_OK)
                ESP_LOGE(LOG_TAG, "Failed to set display: %s", esp_err_to_name(err));
        }

        if (n & LED1) {
            b = (led1 << 0);
            err = i2c_write_data(0x20, 0x01, &b, 1);
            if (err != ESP_OK)
                ESP_LOGE(LOG_TAG, "Failed to set led 1: %s", esp_err_to_name(err));
        }

        if (n & LED2) {
            b = (led2 << 0);
            err = i2c_write_data(0x20, 0x02, &b, 1);
            if (err != ESP_OK)
                ESP_LOGE(LOG_TAG, "Failed to set led 2: %s", esp_err_to_name(err));
        }
    }
}


esp_err_t
attiny24_init()
{
    return pdPASS == xTaskCreate(task, "attiny24_task", 1024, NULL, 10, &th) ?
        ESP_OK : ESP_FAIL;
}


void
attiny24_set_display(uint8_t num)
{
    display = num;
    xTaskNotify(th, DISPLAY, eSetBits);
}


void
attiny24_set_led1(bool on)
{
    led1 = on;
    xTaskNotify(th, LED1, eSetBits);
}


void
attiny24_set_led2(bool on)
{
    led2 = on;
    xTaskNotify(th, LED2, eSetBits);
}
