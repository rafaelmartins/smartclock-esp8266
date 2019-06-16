/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

// simple driver to send data to an i2c port expander, specifically MCP23017.
// the 7 less significant bits are used to run a 7 segment display number.
// the most significant bit is used as a simple output to control a led.

#include <stdlib.h>
#include <esp_err.h>

#include <i2c/i2c.h>

#include "./mcp23017.h"

static uint8_t data = 0;
static const uint8_t pin_config = 0;  // all pins are output
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
mcp23017_init()
{
    esp_err_t rv = i2c_write_data(0x20, 0x00, (uint8_t*) &pin_config, 1);
    if (rv != ESP_OK)
        return rv;

    return mcp23017_send();
}


void
mcp23017_set_bit(uint8_t bit, bool value)
{
    if (bit > 7)
        return;

    data ^= (-value ^ data) & (1 << bit);
}


esp_err_t
mcp23017_send()
{
    return i2c_write_data(0x20, 0x14, &data, 1);
}


void
mcp23017_set_display_number(uint8_t num)
{
    if (num > 9)
        return;

    data &= ~0b1111111;
    data |= *(numbers + num) & 0b1111111;
}


void
mcp23017_set_led(bool on)
{
    mcp23017_set_bit(7, on);
}
