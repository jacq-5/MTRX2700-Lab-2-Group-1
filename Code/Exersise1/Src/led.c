#include "led.h"

#include "stm32f303xc.h"

static uint8_t led_state = 0;  // Only accessible inside this file

void initialise_leds(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;				// Enable clock for GPIOE
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1; // Configure PE8 to PE15 (bits 16–31 of MODER) as general purpose output mode (01), Cast to 16-bit pointer and offset by +1 to access the upper half (pins 8–15)
    *led_output_registers = 0x5555;					 // 0b0101010101010101 → All 8 pins set to output
}

void set_led_state(uint8_t state) {
    led_state = state;								// Save current state to allow toggling later
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;	// Use an 8-bit pointer offset by +1 byte to access those bits directly.
    *led_register = state;							// Write state to the LEDs
}

uint8_t get_led_state(void) {
    return led_state;
}

void toggle_leds(void) {
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;	// Access the same 8-bit region of GPIOE->ODR (bits 8–15)
    *led_register ^= led_state;								// Toggle current LED bits
}
