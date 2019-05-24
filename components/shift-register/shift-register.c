/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

// simple driver to send data to a shift register, specifically SN74HC595.
// the 7 less significant bits are used to run a 7 segment display number.
// the most significant bit is used as a simple output to control a led.

// FIXME: get pins from menuconfig
#define DATA_PIN 4
#define LATCH_PIN 13
#define CLOCK_PIN 15

#include <stdlib.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_err.h>

#include "./shift-register.h"

static uint8_t data = 0;
static const uint8_t numbers[] = {
    0b0111111,
    0b0000110,
    0b1011011,
    0b1001111,
    0b1100110,
    0b1101101,
    0b1111101,
    0b0000111,
    0b1111111,
    0b1101111,
};


esp_err_t
shift_register_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = BIT(DATA_PIN) | BIT(LATCH_PIN) | BIT(CLOCK_PIN);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;

    esp_err_t rv = gpio_config(&io_conf);
    if (rv != ESP_OK)
        return rv;

    return shift_register_send();
}


void
shift_register_set_bit(uint8_t bit, bool value)
{
    if (bit > 7)
        return;

    data ^= (-value ^ data) & (1 << bit);
}


esp_err_t
shift_register_send()
{
    esp_err_t rv = gpio_set_level(DATA_PIN, 0);
    if (rv != ESP_OK)
        return rv;

    for (size_t i = 0; i < 8; i++) {
        rv = gpio_set_level(DATA_PIN, (data >> i) & 1);
        if (rv != ESP_OK)
            return rv;

        vTaskDelay(5 / portTICK_PERIOD_MS);

        rv = gpio_set_level(CLOCK_PIN, 1);
        if (rv != ESP_OK)
            return rv;

        vTaskDelay(5 / portTICK_PERIOD_MS);

        rv = gpio_set_level(CLOCK_PIN, 0);
        if (rv != ESP_OK)
            return rv;
    }

    rv = gpio_set_level(LATCH_PIN, 1);
    if (rv != ESP_OK)
        return rv;

    vTaskDelay(5 / portTICK_PERIOD_MS);

    return gpio_set_level(LATCH_PIN, 0);
}


void
shift_register_set_display_number(uint8_t num)
{
    if (num > 9)
        return;

    bool tmp = (data >> 7) & 1;
    data = *(numbers + num);
    shift_register_set_bit(7, tmp);
}


void
shift_register_set_led(bool on)
{
    shift_register_set_bit(7, on);
}
