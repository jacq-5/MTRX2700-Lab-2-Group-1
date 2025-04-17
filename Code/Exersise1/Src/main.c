#include "stm32f303xc.h"
#include "led.h"
#include "button.h"
#include <stddef.h>

// Modular Timer Functions
void calc_timer_interval(uint32_t interval_ms) {
    TIM2->PSC = 8000 - 1;                      // Prescaler: 8000 → 1 ms tick (8 MHz / 8000 = 1 kHz)
    TIM2->ARR = interval_ms - 1;               // Auto-reload to match interval: number of milliseconds - 1
    TIM2->CNT = 0;                             // Reset counter

    TIM2->EGR = TIM_EGR_UG;                    // Force update event
    TIM2->SR &= ~TIM_SR_UIF;                   // Clear the interrupt flag
}

// Enable TIM2 clock
void set_timer(uint32_t interval_ms) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->CR1 &= ~TIM_CR1_CEN;                // Stop the timer while configuring
    calc_timer_interval(interval_ms);          // Set interval and force reload

    TIM2->DIER |= TIM_DIER_UIE;               // Enable update interrupt
    NVIC_EnableIRQ(TIM2_IRQn);                // Enable IRQ in NVIC

    TIM2->CR1 |= TIM_CR1_CEN;                 // Start the timer
}

// Timer ISR
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;              // Clear interrupt flag
        toggle_leds();                        // Your custom toggle function
    }
}

// Button callback
void chase_led_callback(void) {
    uint8_t state = get_led_state();		 		// Read current LED state
    state = (state << 1) | ((state & 0x80) >> 7);   // Rotate left
    set_led_state(state);							// Update LED state
}

// Main function
int main(void) {
    initialise_leds();
    initialise_button();

    // Initialisation check - blink LED quickly 3 times
    for (int i = 0; i < 3; ++i) {
        set_led_state(0xFF);
        for (volatile int j = 0; j < 100000; ++j);
        set_led_state(0x00);
        for (volatile int j = 0; j < 100000; ++j);
    }

    // User-inputs
    set_timer(500);						  	  // Set timer with 500ms interval
    set_led_state(0b00000001);                // Initial LED state
    set_button_callback(chase_led_callback);  // Register button behavior

    enable_button_interrupt();                // Enable EXTI for the user button

    while (1) {
        // Idle loop
    }
}

