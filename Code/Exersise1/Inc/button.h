
#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

void initialise_button(void);
void enable_button_interrupt(void);
void set_button_callback(void (*callback)());
uint8_t is_button_pressed(void);
void wait_for_button_press(void);
void wait_for_button_release(void);

#endif // BUTTON_H
