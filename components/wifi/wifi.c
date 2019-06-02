/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_err.h>

#include <tcpip_adapter.h>

#include "./wifi.h"

static EventGroupHandle_t eg;


static esp_err_t
event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;

        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(eg, BIT0);
            break;

        case SYSTEM_EVENT_STA_DISCONNECTED:
            esp_wifi_connect();
            xEventGroupClearBits(eg, BIT0);
            break;

        default:
            break;
    }

    return ESP_OK;
}


esp_err_t
wifi_init()
{
    tcpip_adapter_init();

    eg = xEventGroupCreate();

    esp_err_t rv = esp_event_loop_init(event_handler, NULL);
    if (rv != ESP_OK)
        return rv;

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    rv = esp_wifi_init(&cfg);
    if (rv != ESP_OK)
        return rv;

    rv = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (rv != ESP_OK)
        return rv;

    rv = esp_wifi_set_mode(WIFI_MODE_STA);
    if (rv != ESP_OK)
        return rv;

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_SMARTCLOCK_ESP8266_WIFI_SSID,
            .password = CONFIG_SMARTCLOCK_ESP8266_WIFI_PASSWORD,
        },
    };

    rv = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (rv != ESP_OK)
        return rv;

    return esp_wifi_start();
}


void
wifi_wait_for_ip()
{
    xEventGroupWaitBits(eg, BIT0, false, true, portMAX_DELAY);
}
