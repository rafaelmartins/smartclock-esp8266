/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_system.h>

#include <i2c/i2c.h>
#include <ssd1306/ssd1306.h>
#include <ssd1306/font.h>
#include <shift-register/shift-register.h>

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

    i2c_init();
    while (ssd1306_init() != 0) {
        printf("%s: failed to init SSD1306 lcd\n", __func__);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    const char *str = "Hello, World!";
    size_t width = ssd1306_get_string_width(str);

    ssd1306_add_string((64 - FONT_HEIGHT) / 2, (128 - width) / 2, str);
    ssd1306_refresh();

    shift_register_init();

    int cnt = 0;
    while (1) {
        ESP_LOGI("foo", "cnt: %d", cnt);
        vTaskDelay(1000 / portTICK_RATE_MS);
        shift_register_set_display_number(cnt++ % 10);
        shift_register_set_led(cnt % 2);
        shift_register_send();
        gpio_set_level(OUT_PIN, cnt % 2);
    }

    i2c_cleanup();
}
