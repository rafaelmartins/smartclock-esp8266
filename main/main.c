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
#include <mcp23017/mcp23017.h>
#include <wifi/wifi.h>


void app_main(void)
{
    printf("SDK version: %s\n", esp_get_idf_version());

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BIT(CONFIG_SMARTCLOCK_ESP8266_GPIO_INITIALIZED);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    ESP_ERROR_CHECK(button_init());

    // gpio ready, we can power on our stuff :)
    gpio_set_level(CONFIG_SMARTCLOCK_ESP8266_GPIO_INITIALIZED, 1);


    // hardware initialization

    // i2c device failure is fatal for now :/
    ESP_ERROR_CHECK(i2c_init());
    ESP_ERROR_CHECK(mcp23017_init());
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

        ssd1306_add_string_line(3, timef, SSD1306_LINE_ALIGN_CENTER, 6);
        ssd1306_render();

        ESP_LOGI("foo", "cnt: %d", cnt);

        mcp23017_set_display_number(cnt++ % 10);
        mcp23017_set_led(cnt % 2);
        mcp23017_send();
    }

    i2c_cleanup();
}
