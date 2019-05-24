/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

// FIXME: get pins from menuconfig
#define I2C_SCL_PIN 12
#define I2C_SDA_PIN 14

#include <freertos/FreeRTOS.h>
#include <esp_err.h>

#include <driver/i2c.h>

#include "./i2c.h"


esp_err_t
i2c_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN;
    conf.sda_pullup_en = 1;
    conf.scl_io_num = I2C_SCL_PIN;
    conf.scl_pullup_en = 1;

    esp_err_t rv = i2c_driver_install(I2C_NUM_0, conf.mode);
    if (rv != ESP_OK)
        return rv;

    return i2c_param_config(I2C_NUM_0, &conf);
}


esp_err_t
i2c_cleanup()
{
    return i2c_driver_delete(I2C_NUM_0);
}


esp_err_t
i2c_write_data(int8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, slave_address << 1 | I2C_MASTER_WRITE, 1);  // ack
    i2c_master_write_byte(cmd_handle, reg_address, 1);  // ack
    if (data != NULL)
        i2c_master_write(cmd_handle, data, data_len, 1);  // ack
    i2c_master_stop(cmd_handle);
    esp_err_t rv = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd_handle);
    return rv;
}


esp_err_t
i2c_read_data(int8_t slave_address, uint8_t reg_address, uint8_t *data, size_t data_len)
{
    ESP_ERROR_CHECK(i2c_write_data(slave_address, reg_address, NULL, 0));

    i2c_cmd_handle_t cmd_handle = i2c_cmd_link_create();
    i2c_master_start(cmd_handle);
    i2c_master_write_byte(cmd_handle, slave_address << 1 | I2C_MASTER_READ, 1);  // ack
    i2c_master_read(cmd_handle, data, data_len, 2);  // last nack
    i2c_master_stop(cmd_handle);
    esp_err_t rv = i2c_master_cmd_begin(I2C_NUM_0, cmd_handle, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd_handle);
    return rv;
}
