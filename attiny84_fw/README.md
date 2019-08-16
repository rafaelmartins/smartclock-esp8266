# attiny84\_fw

This is a simple firmware for Attiny84, that controls a 7 segment display with numeric values (0-9) and 2 leds.

The pinout of the Attiny84 and 7 segment display (common cathode) are displayed below:

                                                       G F 0 A B
                     +------+                          | | | | |
               VCC --|o     |-- GND                   +---------+
        LED1 - PB0 --|      |-- PA0 - G               |   =A=   |
        LED2 - PB1 --|      |-- PA1 - F               |  F   B  |
         N/C - PB3 --|      |-- PA2 - E               |   =G=   |
           A - PB2 --|      |-- PA3 - D               |  E   C  |
           B - PA7 --|      |-- PA4 - SCL             |   =D=   |
         SDA - PA6 --|      |-- PA5 - C               +---------+
                     +------+                          | | | | |
                                                       E D 0 C H

The I2C address is `0x20`, and it implements 3 virtual registers, available for read and write from the master:

- `0x00`: 7 segment display. 4 less significant bits are the decimal number to be shown by the display, from 0 to 9. Setting all of these bits high will clear the display. Display initializes in the clear state.
- `0x01`: Led 1 (PB0). Less significant bit controls the led state (ON/OFF). Initial state is OFF.
- `0x02`: Led 2 (PB1). Less significant bit controls the led state (ON/OFF). Initial state is OFF.
