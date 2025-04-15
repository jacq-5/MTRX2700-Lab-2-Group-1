/*
 * Integration.c
 *
 *  Created on: Apr 15, 2025
 *      Author: haileythill-turke
 */

#include "serial.h"

#include "stm32f303xc.h"

void ParseInput(uint8_t *input, uint8_t length) {
    char *input_str = (char *)input;

    // Find the first space between command and operand
    char *space = strchr(input_str, ' ');
    if (!space) {
        SerialOutputString("Invalid command format\r\n", &USART1_PORT);
        return;
    }

    // Extract command
    size_t command_len = space - input_str;
    if (command_len >= 20) command_len = 19;

    char command[20] = {0};
    strncpy(command, input_str, command_len);
    command[command_len] = '\0';

    // Extract operand
    char operand[80] = {0};
    strncpy(operand, space + 1, sizeof(operand) - 1);

    // End string at new line
    char *newline = strpbrk(operand, "\r\n");
    if (newline) *newline = '\0';


    if (strcmp(command, "led") == 0) {
        // Convert binary string to byte and output to GPIO (stub here)
        uint8_t led_state = (uint8_t)strtol(operand, NULL, 2);
        // Example: send pattern to LEDs (replace with your actual LED function)
        set_led_state(led_state); //LED DISPLAY FUNCTION

    } else if (strcmp(command, "serial") == 0) {
        // Echo the operand back through serial
        SerialOutputString(operand, &USART1_PORT);
        SerialOutputString("\r\n", &USART1_PORT);

    } else if (strcmp(command, "oneshot") == 0) {
        uint32_t duration = (uint32_t)strtoul(operand, NULL, 10);
        StartOneShotTimer(duration, blink_leds36710); // Replace with timer input

    } else if (strcmp(command, "timer") == 0) {
        uint32_t period = (uint32_t)strtoul(operand, NULL, 10);
        StartContinuousTimer(period, blink_leds4895); // Replace with timer input

    } else {
        SerialOutputString("Unknown command\r\n", &USART1_PORT);
    }
}
