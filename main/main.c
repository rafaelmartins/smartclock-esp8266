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

#include <button/button.h>
#include <i2c/i2c.h>
#include <ssd1306/ssd1306.h>
#include <ssd1306/font.h>
#include <ds3231/ds3231.h>
#include <attiny24/attiny24.h>
#include <wifi/wifi.h>


void app_main(void)
{
    printf("SDK version: %s\n", esp_get_idf_version());

    // hardware initialization

    ESP_ERROR_CHECK(button_init());
    ESP_ERROR_CHECK(i2c_init());
    ESP_ERROR_CHECK(attiny24_init());
    ESP_ERROR_CHECK(ssd1306_init());

    ssd1306_add_string_line(0, "SYSTEM INITIALIZATION", SSD1306_LINE_ALIGN_LEFT, 0);
    ssd1306_add_string_line(4, "Wifi: Waiting", SSD1306_LINE_ALIGN_LEFT, 0);
    ssd1306_add_string_line(5, "SNTP: Waiting", SSD1306_LINE_ALIGN_LEFT, 0);
    ssd1306_render();

    // wifi failures are fatal for now :/
    ESP_ERROR_CHECK(wifi_init());

    // our rtc must be initialized after wifi. it will wait for an ip before
    // querying times to SNTP
    ds3231_init();

    char ip[16];
    ssd1306_add_string_line(4, wifi_get_ip(ip, 16), SSD1306_LINE_ALIGN_LEFT, 6);
    ssd1306_render();

    ds3231_wait_for_sntp();
    ssd1306_add_string_line(5, "Synchronized", SSD1306_LINE_ALIGN_LEFT, 6);
    ssd1306_render();

    vTaskDelay(2000 / portTICK_RATE_MS);

    TickType_t startTime = xTaskGetTickCount();

    struct tm t;
    char timef[9];
    int cnt = 0;

    ssd1306_clear();
    ssd1306_add_string_line(3, "Time:         ", SSD1306_LINE_ALIGN_CENTER, 0);

    while (1) {
        vTaskDelayUntil(&startTime, 1000 / portTICK_RATE_MS);

        ds3231_wait_for_sntp();
        ds3231_get_time(&t);
        strftime(timef, 9, "%H:%M:%S", &t);

        ESP_LOGI("foo", "cnt: %d", cnt);

        ssd1306_add_string_line(3, timef, SSD1306_LINE_ALIGN_CENTER, 6);
        ssd1306_render();

        attiny24_set_display(cnt++ % 10);
        attiny24_set_led1((cnt % 2) == 0);
        attiny24_set_led2((cnt % 2) != 0);
    }

    i2c_cleanup();
}
