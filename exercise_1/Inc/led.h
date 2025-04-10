#ifndef LED_H
#define LED_H

#include <stdint.h>  // for uint8_t

void initialise_leds(void);
void set_led_state(uint8_t state);
uint8_t get_led_state(void);
void toggle_leds(void);

#endif
