/*
 * smartclock-: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"

#define OUT_PIN 5


void app_main(void)
{
    printf("SDK version: %s\n", esp_get_idf_version());

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BIT(OUT_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    int cnt = 0;
    while (1) {
        ESP_LOGI("foo", "cnt: %d", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
        gpio_set_level(5, cnt % 2);
    }
}
