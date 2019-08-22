/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>

#include <driver/gpio.h>

#include "./button.h"

static TaskHandle_t th;

#define LOG_TAG "button"


static void IRAM_ATTR
button_handler(void *arg)
{
    vTaskNotifyGiveFromISR(th, NULL);
}


static void
button_task(void *pvParameters)
{
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        ESP_LOGI(LOG_TAG, "clicked!");
    }
}


esp_err_t
button_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = BIT(CONFIG_SMARTCLOCK_ESP8266_GPIO_BUTTON);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;

    esp_err_t rv = gpio_config(&io_conf);
    if (rv != ESP_OK)
        return rv;

    rv = gpio_install_isr_service(0);
    if (rv != ESP_OK)
        return rv;

    if (pdPASS != xTaskCreate(button_task, "button_task", 1024, NULL, 10, &th))
        return ESP_FAIL;

    return gpio_isr_handler_add(CONFIG_SMARTCLOCK_ESP8266_GPIO_BUTTON,
        button_handler, NULL);
}
