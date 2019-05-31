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
#include <ds3231/ds3231.h>
#include <shift-register/shift-register.h>
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

    shift_register_init();

    i2c_init();
    while (ssd1306_init() != 0) {
        printf("%s: failed to init SSD1306 lcd\n", __func__);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    const char *str[] = {"Hello, World!", "bola"};
    int cnt = 0;
    gpio_set_level(CONFIG_SMARTCLOCK_ESP8266_GPIO_INITIALIZED, 1);

    TickType_t startTime = xTaskGetTickCount();

    ESP_ERROR_CHECK(wifi_init());

    ds3231_init();

    struct tm t;
    ESP_ERROR_CHECK(ds3231_get_time(&t));

    char foo[1024];
    strftime(foo, 1024, "%c", &t);
    printf("%s\n", foo);

    while (1) {
        vTaskDelayUntil(&startTime, 1000 / portTICK_RATE_MS);

        ESP_LOGI("foo", "cnt: %d", cnt);

        size_t width = ssd1306_get_string_width(str[cnt % 2]);
        ssd1306_clear();
        ssd1306_add_string((128 - width) / 2, (64 - FONT_HEIGHT) / 2, str[cnt % 2]);
        ssd1306_refresh();

        shift_register_set_display_number(cnt++ % 10);
        shift_register_set_led(cnt % 2);
        shift_register_send();
    }

    i2c_cleanup();
}
