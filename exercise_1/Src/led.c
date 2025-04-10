#include "led.h"

#include "stm32f303xc.h"

static uint8_t led_state = 0;  // Only accessible inside this file

void initialise_leds(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;
}

void set_led_state(uint8_t state) {
    led_state = state;
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
    *led_register = state;
}

uint8_t get_led_state(void) {
    return led_state;
}

void toggle_leds(void) {
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
    *led_register ^= led_state;
}
