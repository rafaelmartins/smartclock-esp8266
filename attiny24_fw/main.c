/*
 * smartclock-esp8266: Smart Clock prototype built with ESP8266.
 * Copyright (C) 2019 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 *
 * This program can be distributed under the terms of the BSD License.
 * See the file LICENSE.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c-device.h"

//                                                   G F 0 A B
//                 +------+                          | | | | |
//           VCC --|o     |-- GND                   +---------+
//    LED1 - PB0 --|      |-- PA0 - G               |   =A=   |
//    LED2 - PB1 --|      |-- PA1 - F               |  F   B  |
//     N/C - PB3 --|      |-- PA2 - E               |   =G=   |
//       A - PB2 --|      |-- PA3 - D               |  E   C  |
//       B - PA7 --|      |-- PA4 - SCL             |   =D=   |
//     SDA - PA6 --|      |-- PA5 - C               +---------+
//                 +------+                          | | | | |
//                                                   E D 0 C H

static uint8_t display_pins[10][2] = {
//     b c defg         a
//   A:10101111  B:00000100
    {0b10101110, 0b00000100},  // 0
    {0b10100000, 0b00000000},  // 1
    {0b10001101, 0b00000100},  // 2
    {0b10101001, 0b00000100},  // 3
    {0b10100011, 0b00000000},  // 4
    {0b00101011, 0b00000100},  // 5
    {0b00101111, 0b00000100},  // 6
    {0b10100000, 0b00000100},  // 7
    {0b10101111, 0b00000100},  // 8
    {0b10101011, 0b00000100},  // 9
};


static void
set_display(uint8_t reg, uint8_t v)
{
    (void) reg;

    v &= 0xf;

    if (v > 9 && v != 0xf)
        return;

    // it is a nice trick, isn't it? :P
    PORTA &= ~display_pins[8][0];
    PORTB &= ~display_pins[8][1];

    if (v < 10) {
        PORTA |= display_pins[v][0];
        PORTB |= display_pins[v][1];
    }
}


static void
set_led(uint8_t reg, uint8_t v)
{
    // leds are PB0 and PB1
    // registers are 0x1 and 0x2
    PORTB ^= (-(bool) v ^ PORTB) & (1 << ((reg & 2) - 1));
}


int
main(void)
{
    DDRA |= display_pins[8][0];
    DDRB |= display_pins[8][1] | (1 << 0) | (1 << 1);

    i2c_device_init(0x20);
    i2c_device_add_register(set_display);
    i2c_device_add_register(set_led);
    i2c_device_add_register(set_led);
    i2c_device_set_register(0x00, 0xf);

    sei();

    while (true);

    return 0;
}
