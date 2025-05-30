#include "serial.h"

#include "stm32f303xc.h"



// global variable and callback pointer
static uint8_t led_state = 0;			// stores current LED output state (8 bits)


// Returns the current LED state- allows other parts of the program to retrieve the LED state without directly accessing the led_state variable.
uint8_t get_led_state() {
    return led_state;
}

// Set the LED state with encapsulation
void set_led_state(uint8_t state) {
    led_state = state;
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1; // updates led_state and writes it to GPIOE output register (specifically bits 8–15, hence the offset).
    // To control LEDs, need to write to bits 8-15 of ODR (output data register)
    // Note that it is only uint8_t here (compared to uint16_t above) because each LED (PE8-15) uses only 1 bit in the ODR register compared to 2 in the MODER register.
    *led_register = state;
}

void initialise_leds(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
    uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
    *led_output_registers = 0x5555;
}

void toggle_leds(void) {
    uint8_t *led_register = ((uint8_t*)&(GPIOE->ODR)) + 1;
    *led_register ^= led_state;
}

