#include "button.h"
#include "stm32f303xc.h"

// Static function pointer for button press callback
static void (*button_callback)(void) = 0;


// Initialize the user button on PA0 as input.

void initialise_button(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;               // Enable GPIOA clock
    GPIOA->MODER &= ~(0x3 << (0 * 2));               // Clear MODER bits for PA0 (set as input)
}

// Configure EXTI0 to trigger interrupt on rising edge of PA0.
void enable_button_interrupt(void) {
    __disable_irq();                                 // Disable global interrupts during setup

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;            // Enable SYSCFG clock
    SYSCFG->EXTICR[0] = SYSCFG_EXTICR1_EXTI0_PA;     // Map EXTI0 to PA0
    EXTI->RTSR |= EXTI_RTSR_TR0;                     // Trigger on rising edge
    EXTI->IMR |= EXTI_IMR_MR0;                       // Unmask interrupt line
    NVIC_SetPriority(EXTI0_IRQn, 1);                 // Set interrupt priority
    NVIC_EnableIRQ(EXTI0_IRQn);                      // Enable EXTI0 interrupt

    __enable_irq();                                  // Re-enable global interrupts
}

// Assign a callback function to be triggered when the button is pressed.
void set_button_callback(void (*callback)(void)) {
    button_callback = callback;						// callback Pointer to the function to call on button press.
}


uint8_t is_button_pressed(void) {
    return (GPIOA->IDR & 0x01);						// Read the current button state
}

// EXTI0 interrupt handler – calls the registered button callback function.
void EXTI0_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR |= EXTI_PR_PR0;                     // Clear pending interrupt
        if (button_callback != 0) {
            button_callback();                       // Call registered function
        }
    }
}

