## MTRX2700 Lab 2 Group 1
# Thursday 2pm-5pm

## Members
Hailey Thill-Turke,
Lillie Mellin,
Jacqueline Ford,
James Robison

## Roles and responsibilities
Hailey: Exercise 2,
Lillie: Exercise 1,
Jacqueline: Integration and minutes,
James: Exercise 3,

All group members assisted with and scaffolded the integration code associated with the exercise they compeleted. 

## Instructions for user:

[1]Instructions to import Projects
<br> </br>

1. Ensure the latest STM32CubeIDE software is downloaded on the chosen device.
   
2. Connect the STM32 board to the computer.
   
3. Open the application and select “Import Project”.
 ![Image](https://github.com/user-attachments/assets/75b6f0c1-c893-4c91-bc02-a9e28a2d47f0)
4. Select the desired project folder from your directory.
 ![Image](https://github.com/user-attachments/assets/07443769-289d-417c-8020-bdd395f53d06)
5. Select finish.
   
 ![Image](https://github.com/user-attachments/assets/1ca9df2c-acb4-4ba8-b99b-07bdb248d538) 
 
6. Within the project in the source folder select main.c file and click the debug button.

 ![Image](https://github.com/user-attachments/assets/8d616329-7b3a-4cbd-9536-0d6b6b57365e)

7. Select either the play button to run through the entire code or step through it line by line with the arrow.

 ![Image](https://github.com/user-attachments/assets/63089134-5046-4434-84d8-1fa757924fa1)

[2]Instructions to use Cutecom
<br> </br>

1. Download Cutecom
   
2. Open the application
   
3. Select connect

![Image](https://github.com/user-attachments/assets/0d5520aa-b567-46e0-a8f5-3b70bb563180)

4. Select the desired device and baud rate before hitting open device

![Image](https://github.com/user-attachments/assets/7ea8dd11-80e8-4ba9-b456-2e84afb75d7c)
 
5. You will now be able to transmit and receive data from your board

### Exercise 1

#### Exercise Overview
Exercise 1 interfaces the LEDs and buttons for the STM32F3 discovery board to demonstrate the principles of software design. It is structured around modular and interrupt-driven design principles. The project is structured into three main components: the Button Module (`button.c`, `button.h`), LED Module (`led.c`, `led.h`), and Main Program (`main.c`). Each module is responsible for a specific aspect of the digital I/O functionality on the STM32F303 microcontroller.

#### How to use:
- Provide the user-defined time interval in milliseconds (e.g. 500, 1000)
- Provide the user-defined LED state bitmask (e.g. `0b00000001` to light up one at a time)
- Provide the user-defined callback function
- Every time the timer reaches the interval or a button is pressed, an interrupt is triggered
- The ISR (interrupt service routine) defined in `TIM2_IRQHandler()` is called
- The timer interrupt causes the ‘blinking’ appearance
- The button interrupt and LED toggle cause the ‘chase’ effect respectively

#### How to run:
The code can be debugged and flashed directly to the STM32 to run. However, additional functionality including initialization checks have been implemented for the user. The following instructions relate to the sanity checks for initializing the LEDs, button, and timer:
- Add breakpoints in the main file underneath `if (leds.init())`, `if (button.init())`, and `init_timer()` as these all include checks that each function has been successfully flashed to the STM.
- After running the code to the first breakpoint, LED 0 will turn on to indicate the GPIOE is configured.
- After running the code to the second breakpoint, LED 1 will turn on to indicate the button is configured.
- After running the code to the third breakpoint, LED 2 will turn on to indicate the timer is configured.
- These LED states are then cleared so that the remainder of the project can be flashed to the LED without any interference from these checks.

#### Part A:  
This project implements a digital I/O interface for both a button and LEDs. The goal is to encapsulate the functionality of interacting with buttons and LEDs into separate independent modules. This allows for reusability. The task is inspired by the W05-C-interrupt code which was used as a guide for the initialization functions.

##### LED Initialization
###### Logic Description
`led_init(void)` enables clocks, GPIOE, and sets pins PE8–PE15 as outputs.
- `MODER` is a 32-bit register that controls the mode of each pin in port E. Each pin uses 2 bits where `01 = Output`. Since it’s 32 bits, it configures 16 pins: PE0–PE15. 
- `'(uint16_t *)&(GPIOE->MODER)'` casts the address of `MODER` to a pointer to a 16-bit (`uint16_t`) value, effectively dividing the register into two 16-bit halves.
- `'+ 1'` adds 1 to the pointer, so it now points to the upper 16 bits of `MODER`, which control pins PE8 to PE15 where the LEDs are connected.
- `uint16_t *moder = ...` stores the pointer to the upper half of `MODER` in a variable called `moder`. This allows the code to modify only the LED-related pin modes without touching the lower 8 pins.

##### Button initialisation
###### Modular Design 

```c
// Pseudocode
FUNCTION button_init(callback):
    ENABLE clock for GPIOA port A (used by the button)
    ENABLE clock for system configuration controller (SYSCFG)
    CLEAR pin 0 (PA0) 2 bits in GPIOA->MODER to configure it as input
    SET SYSCFG->EXTICR[0] to connect external interrupt line 0 (EXTI0) to PA0
    ENABLE rising trigger (button press) for EXTI line 0
    ENABLE interrupt mask for EXTI line 0 to allow interrupt
    SET priority for EXTI0 interrupt
    ENABLE EXTI0 interrupt in NVIC
    STORE the callback function for later use
    RETURN true to indicate success
```

###### Logic Description
The button interface handles the digital input (button press) functionality. It initializes the button GPIO pin and configures the interrupt system for detecting button presses. It takes a callback function as an argument that will is called when the button is pressed.

#### Part B:
Part b implements a system where a function pointer (callback) is passed during button initialization, which is later called automatically whenever the button is pressed.  
This allows the main program to define what happens when the button is pressed, while the button module handles how to detect the button press.

##### Logic Description
###### 1. `main.c` — Entry Point

```c
ButtonInterface button = get_button_interface();
if (button.init(chase_led_callback)) led_test(1);
```

The main program gets an interface struct (`ButtonInterface`) from `button.h` containing the `init()` function.  It calls `init()` and passes a function pointer called `chase_led_callback` as an argument. This function is stored in the `button.c` module and automatically called when the button is pressed.

###### 2. `button.c` — Digital I/O (Button) Module
A static global variable holds the pointer to the callback function provided during init. It uses the type `ButtonCallback`, which is defined as a function pointer. The `button_init()` function stores the function pointer in the static variable. This allows the button interrupt to later use it to call the user's function (`chase_led_callback()`). The interrupt handler (`EXTI0_IRQHandler()`) calls the saved function using the `button_callback()` pointer. This is where `chase_led_callback()` gets executed when the button is pressed.`ButtonInterface` is a struct to separate the button code from the main project such that the main code doesn't directly access or modify the internal functions. The callback function (`chase_led_callback`) will be triggered every time the button is pressed. The logic performs a rotating LED pattern on each press.


#### Part C:

##### Modular design
Part c implements the LED control module where the internal LED state is fully encapsulated, and the only way to access or modify it is through the module's header file. The files involved in this module are:
- `led.h` — Public interface for the LED module  
- `led.c` — Implementation of the LED logic  
- `main.c` — Application code using the LED interface  

##### Logic Description
The only ways to access or modify `led_state` are through the functions in `led.c`. `led_state` is stored privately inside `led.c` so that it is not visible or accessible to `main.c` or any external file. This ensures that all read/write access goes through controlled functions only. The logic involved in each function is as follows:
- `set_state(uint8_t state)`: Updates both the internal state and reflects the change on the physical GPIO port (Port E).
- `get_state(void)`: Allows external files to read the current state of the LEDs. Returns the internal `led_state` rather than reading the actual GPIO.
- `toggle_leds(void)`: Uses the current internal `led_state` to toggle the LEDs.
- `get_led_interface()`: Allows the user to interact with LEDs without touching `led_state` or registers directly.

#### Part D:

To accomplish the advanced functionality, TIM2 on the STM32 was configured to generate periodic interrupts based on a user-defined time interval in milliseconds. It is designed for the user to specify the timer interval in `main` through the function call `init_timer(500)`.

##### Modular Design
```c
set_timer_interval(interval_ms) {
    Set prescaler to get 1ms per tick (TIM2 -> PSC = 8000 - 1)
    Set ARR to get desired delay (TIM2->ARR = interval_ms - 1)
    Reset counter to count from 0 again (TIM2->CNT = 0)
    Generate an update event and set the Update Interrupt Flag (TIM_SR_UIF), calling TIM2_IRQHandler()
    Force immediate reload of PSC and ARR values to keep ticking (TIM2->EGR = TIM2_EGR_UG)
}

init_timer(interval_ms) {
    Stop timer while prescaler, auto-reload and reset counter are being set (PSC, ARR, CNT)
    Call set_timer_interval() to set these values and force immediate reload once update event reached 
    Enable timer to generate an interrupt on update events 
    Start the timer again
    Show health LED if complete
}

TIM2_IRQHandler() {
    Triggered by ARR overflow in set_timer_interval
    Check if UIF flag in status register is set
    If set, toggle LED
}
```

##### Logic Description
The timer has two configuration values:
- PSC (Prescaler): divides the input clock  
- ARR (Auto Reload Register): sets the maximum count (or "tick") value before the timer overflows and triggers an update event (interrupt)

The time interval between timer interrupts is given by:

```
Interval (seconds) = ((ARR + 1) × (PSC + 1)) / timer_clock
```

The STM timer is driven by an 8 MHz timer clock (8 million ticks per second). To tick once every millisecond (1 kHz), the timer clock must be divided by 8000:

```
8000000 Hz / 8000 = 1000 Hz → 1 ms per tick
```

##### User Instructions
In main.c, input desired timer into `init_timer(500)`, where 500 is in milliseconds (i.e. 0.5 seconds). It should be half the length of time required between 'blinks' so setting it to 500 would result in an LED 'blink' every 1 second.

### Exercise 2: Serial Interface

#### Part A:
Data is received from a UART into a designated buffer. If the data is larger than the designated buffer size the extra characters will not be stored. 
##### Modular Design
Input: `buffer, buffer size, serial_port, terminating character`  
Output: received data will be loaded into buffer
```c
Main{
	Initialise serial port
	Initialise buffer
	Calculate buffer size
	Initialise terminating character
	SerialInputString(buffer, buffer size, serial_port, terminating character)
}

SerialInputString(buffer, buffer size, serial_port, terminating character){
	Initialise a counter
	Initialize pointer to start of buffer
	
	SerialInputChar()
	Increment counter
	
	While (character != terminating character){
		If counter > buffer length {
			Break
		}
		SerialInputChar()
		Increment counter
	}
}

SerialInputChar(){
	Check ready to receive flag 
	Buffer = incoming character
}
```

##### Logic Description
When `SerialInputString` is called the program will enter `SerialInputChar` and loop there until the ready to receive flag is on `USART_ISR_RXNE`. The program will then load the first character into the buffer, return to `SerialInputString` and check if this character was the terminating character. If not, `SerialInputChar` is continuously called until the terminating character is received. The program will then exit the function. Furthermore after each character is loaded into the buffer, `counter` is compared with `buffer_size`, if the buffer is full the program exits the while loop meaning no further characters are received.

##### User Instructions
   1. Initialise your buffer, buffer_size, serial port and terminating character in main
<pre>
SerialInitialise(BAUD_115200, &serialPort)
uint8_t terminating = '#';
uint8_t buffer[32];
uint32_t buffer_size = sizeof(buffer)/sizeof(buffer[0]);	
</pre>
   2. Call SerialInputString with the following arguments.
<pre>
SerialInputString(buffer, buffer_size, &serialPort, terminating);
</pre>


##### Test Cases
Terminating character
- Input: hello#  
- Output: buffer = hello#

No terminating character
- Input: hello  
- Output: buffer = hello  
Limitation - without terminating character program stays in receive loop writing for more characters

Data > buffer size
- Input: What A Wonderful World - Disney Land  
- Output: buffer = What A Wonderful World - Disney

#### Part B:
Once a terminating character has been received a call back function will be called. This function is set when initialising the serial port.

##### Modular Design
Input: `buffer, serial port, terminating character, callback function`    
Output: received data will be retransmitted to serial communication interface

Same as part (a) but with the following additions to each function

```c
Main{
	Initialise serial port including the call back function argument
	[part (a) code]
}

callBackFunction(buffer, counter){
	Insert function code
}

SerialInputString(buffer, buffer size, serial_port, terminating character){
[part (a) code]	
Call serial port callback function
}
```

##### Logic Description
The typedef Struct `SerialPort` has the function pointer `completion_function`. The address of the desired call back function is passed into `SerialInitialise` and this is set as the serial ports `completion_function`. Once the program has finished receiving data from the designated serial port in `SerialInputString`, this serial port’s `completion_function` is called.

##### User Instructions
   1. Initialise call back function by passing it’s address into SerialInitialise
<pre>
SerialInitialise(BAUD_115200, &USART1_PORT, &callBackFunction);
</pre>
   2. Call the callback function at end of SeriaInputString
<pre>
serial_port->completion_function(start_of_string, counter); 	
</pre>
   3. Follow instructions from part (a)

##### Test Cases
These test cases are based on callBackFunction retransmitting the received string

Terminating Character
- Input: Hello#  
- Output: cutecom - Hello#

No Terminating Character
- Input: Hello  
- Output: cutecom - 

Data > buffer size
- Input: What A Wonderful World - Disney Land  
- Output: cutecom - What A Wonderful World - Disney


#### Part C:
An interrupt is used when receiving data from a USART so data can be received anytime throughout the program. The incoming data will be placed in the buffer and considered complete when a terminating character is received. All data after this will be discarded.

##### Modular Design
Same as part (a) but with the following additions to each function

Input: same as part (a)  
Output: same as part (a)

```c
Main file scope:
Initialise buffer
Initialise terminating character
Calculate buffer size
Initialize interrupt function pointer

Serial.c file scope:
Initialize counter

Main{
	enableInterrupts()
	Set interrupt function to InterruptInputString
	Infinite loop
}

void enable_interrupt() {
	Disable interrupts
	enable receive interrupt
	Set priority for USART1 interrupt
	allow USART1 interrupt to occur
	Re-enable interrupts
}

Interrupt handler{
	If interrupt function has been set{
		Call interrupt function
	}
}

void InterruptInputString(buffer, terminating, buffer size, serial_port){
	if(character == terminating OR counter >= buffer size){
		discard = new character
	}
	else{
		buffer[counter] = new character
		Increment counter
	}
}
```

##### Logic Description
In `enableInterrupts` USART1’s receive interrupt is enabled and it’s priority is set. The function called in the interrupt handler 'when_receiving_data' is defined. The main function then goes into an infinite loop and when data is sent to USART1 the interrupt handler `USART1_EXTI25_IRQHandler` is called. The interrupt handler checks if a function has been set and then calls 'when_receiving_data'. Once the terminating character has been received or the buffer is full all new characters will be loaded into a discard variable.

##### User Instructions
   1. Initialise buffer, terminating character, buffer size and counter variables in their designated scopes. 
<pre>
uint8_t buffer[32];
uint32_t buffer_size = sizeof(buffer)/sizeof(buffer[0]);
uint8_t terminating = '#';

uint8_t counter;	
</pre>
   2. Initialize interrupt function pointer
<pre>
void (*when_receiving_data)(uint8_t [], uint32_t buffer_size, SerialPort *) = 0x00;	
</pre>
   3. Initialise the interrupt.
<pre>
enableInterrupt()	
</pre>
   4. Set interrupt function to desired function address
<pre>
when_receiving_data = &InterruptInputString;	
</pre>

##### Test Cases
Same test cases as part (a) because functionality has not changed.

#### Part D: Transmit Interrupt
The program transmits the first byte of an array, initialises the transmit interrupt and this is used to transmit the rest of the string. 

##### Modular Design
Input: `send_data, tx_index, tx_length`  
Output: string will be retransmitted to serial communication interface

```c
Serial.c file scope:
uint8_t *send_data = "send it!!";
uint8_t txIndex = 0;
uint8_t txLength = 9;

Main{
	enableInterrupts	
	InterruptOutputString;
}

void InterruptOutputChar(){
	if (Index < Length) {								
		Transmit character
	}
	else {
		Disable transmit interrupt
	}
}

void InterruptOutputString(){
	Initialize transmit interrupt function	as InterruptOutputChar		
	Transmit first character
	enable transmit interrupt 					
}
```

##### Logic Description
`InterruptOutputString` is called in the main function. The transmit interrupt function `when_sending_data` is set and the first character is manually transmitted by `SerialOutputChar`. USART1’s transmit interrupt is then enabled. The interrupt will continually be called as the following characters trigger the ready to transmit flag. The interrupt handler calls `InterruptOutputChar` which checks if all characters have been transmitted yet, if not it transmits the next character, otherwise it will disable the transmit interrupt so the program is not continually interrupted when there is no more data to send.

##### User Instructions
   1. Initialise the string you want to transmit, as well as it’s length and an index function in serial.c file scope
<pre>
uint8_t *send_data = "send it!!";
uint8_t txIndex = 0;
uint8_t txLength = 9;	
</pre>
   2. Initialize interrupt function pointer
<pre>
void (*when_sending_data)() = 0x00;	
</pre>
   3. Enable interrupts.
<pre>
enableInterrupt()	
</pre>
   4. Call InterruptOutputString.
<pre>
InterruptOutputString();	
</pre>

##### Test Cases
Upper case, lower case, numbers and special characters  
- Input: send_data = GoTeam2025!!!  
- Output: Cutecom - GoTeam2025!!!

#### Part D: Double Buffer
The program receives and processes data using a double buffer, so that new data can be received in the middle of processing previous data without overwriting any of it. 

##### Modular Design
Input: `buffer, buffer_size, serial port`  
Output: received data will be retransmitted to serial communication interface  
```c
Main{
	enableInterrupts()
	Set interrupt function to SerialInputStringdb
	Infinite loop
}

void SerialInputStringdb(buffer, serial_port) {
	load data into kernel buffer
 	increment kernel buffer counter
	if(character != terminating character){
		buffer[uartKernel][bufferCounter[uartKernel]] = new character
		increment kernel buffer counter
	}
 if(character == terminating character){
	   if(still_reading = off){		
		   InputLogic(buffer);			   
	}
}

void InputLogic(buffer){
	if (there’s data to be processed)
	{
		still_reading = on;	
		for(i in user buffer count){
			Desired logic code
		}
		still_reading = off;		
	}
	switch_buffers();
	if (there’s data to be processed){
		InputLogic(buffer);
	}
}

switchBuffers(){
	switch kernel and user index
	Reset kernel buffer counter 
}
```

##### Logic Description
Data is received through USART1 by interrupts. Once a transmission is complete, Logic processes this data however if more data is sent during this the interrupt handler will be called to receive this new data. The double buffer allows for this new data to be received into a separate location, not overwriting the data that is not processed yet. Once this new transmission is complete the program will resume where it left off processing the old data and once this is finished it will switch which buffer it is processing and begin processing the new data. If the data transmitted is greater than the buffer size, all excess chracaters will be discarded.

##### User Instructions
   1. Initialize double buffer in main file scope.
<pre>
uint8_t double_buffer[2][32];	
</pre>
   2. Initialise uartKernel, uartUser, BufferCounter and still_reading variables in serial.c file scope.
<pre>
uint8_t uartKernel = 0;
uint8_t uartUser = 1;
uint8_t bufferCounter[2];
//uint8_t userBufferReady = 0;
uint8_t still_reading = 0;	
</pre>
   3. Initialize interrupt function pointer
<pre>
void (*when_receiving_data_db)(uint8_t [][32], SerialPort *) = 0x00;
</pre>
   4. Set interrupt function to desired function address
<pre>
when_receiving_data_db = &SerialInputStringdb;	
</pre>
   3. Enable interrupts.
<pre>
enableInterrupt()	
</pre>

##### Test Cases
Multiple Inputs
- Input: Data1, Data2 (input these separately)
- Output: Cutecom -   
Data1  
Data2


#### Exercise 3: Timer Interface
Exercise 3 involves creating a module to interface with hardware timers, triggering callback functions at regular intervals, changing these intervals with another function, and using a oneshot function to trigger a onetime event.

#### Raw Assembly
Each major function in this module contains commented assembly code to explain on a hardware level the function of the code. This provides clarity on how high-level C operations translate to register-level manipulations.

#### Clocks & GPIO
- `enable_clocks()` enables clocks for **GPIOE**, **TIM2**, and **TIM3**.
- `initialise_board()` configures **PE8–PE15** as outputs.

```c
void enable_clocks() {
	RCC->AHBENR  |= RCC_AHBENR_GPIOEEN;
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
}
```

```c
void initialise_board() {
	uint16_t *led_output_registers = ((uint16_t *)&(GPIOE->MODER)) + 1;
	*led_output_registers = 0x5555;  // Set PE8–PE15 as output
}
```

#### TIM2 – Periodic Interrupt Timer
- `timer_init()` sets up TIM2 for periodic interrupts (1 kHz).
- `reset_period()` changes the interval at runtime.
- `get_period()` returns the current timer interval.
- `TIM2_IRQHandler()` triggers a user-defined callback.

The timer interface is modular by design. A function pointer is passed into `timer_init`, and the timer calls it back automatically on each period match.

```c
void timer_init(uint32_t interval, callback_t cb) {
	user_callback = cb;
	interval_ms = interval;

	TIM2->PSC = 7999;               // 1ms tick
	trigger_prescaler();            // Force prescaler reload
	TIM2->ARR = interval_ms;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1  |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}
```

#### TIM3 – One-Shot Timer
- `start_oneshot_timer_TIM3()` configures TIM3 to fire once after a specified delay (ms).
- `TIM3_IRQHandler()` executes a one-shot callback once the delay elapses.

The one-shot mechanism is modular and uses a separate function pointer `oneshot_callback`. Once fired, the handler clears state and disables the timer.

```c
void start_oneshot_timer_TIM3(uint32_t delay_ms, callback_t cb) {
    oneshot_mode = 1;
    oneshot_callback = cb;

    TIM3->CR1 = 0;
    TIM3->CNT = 0;
    TIM3->PSC = 7999;
    TIM3->ARR = delay_ms;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->SR &= ~TIM_SR_UIF;
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_OPM | TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
}
```

#### LED Blinking
- `blink_led1()` toggles PE8 for periodic events.
- `blink_led2()` toggles PE15 for one-shot visual feedback.

Each blink function is isolated, modular, and registered via a callback mechanism.

```c
void blink_led1(void) {
    const uint8_t mask = 0b00000001;
    static uint8_t state = 0;
    uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

    state ^= mask;
    *led_output_register = (*led_output_register & ~mask) | (state & mask);
}
```

```c
void blink_led2(void) {
    const uint8_t mask = 0b10000000;
    static uint8_t state = 0;
    uint8_t *led_output_register = ((uint8_t*)&(GPIOE->ODR)) + 1;

    state ^= mask;
    *led_output_register = (*led_output_register & ~mask) | (state & mask);
}
```

#### Logic

##### Part A, B & C
The function `timer_init` is called upon in `main`, taking the parameters `interval` and `cb`. `interval` refers to the interval in milliseconds that is desired in which the callback function is run, and `cb` is the name of said callback function.  
Then within `timer_init`, a prescaler value is set and loaded for 7999, resulting in a 1kHz clock frequency. The Auto Reload Register (ARR) is set to the interval passed in the function call. The timer is then started, and the interrupt is enabled.  
When the counter reaches ARR (i.e., the interval), the `TIM2_IRQHandler` checks the UIF flag, clears it if it is active, and then runs the callback specified in `timer_init`.  
`blink_led1` is a function that toggles the state of the PE8 LED to display the operation of the periodic timer.

##### Part B
The function `reset_period` is used to input a new value for ARR to change the period of the timer (in milliseconds). This is done by passing the new value, assigning it to ARR, resetting the counter, and forcing an update event to kickstart the timer. This results in an immediate call to `blink_led1`, and the LED then blinks at the new interval.

```c
void reset_period(uint32_t period) {
	interval_ms = period;
	TIM2->ARR = interval_ms;
	TIM2->CNT = 0;
	TIM2->EGR |= TIM_EGR_UG;
}
```

The function `get_period()` simply returns the current period, as `interval_ms` is globally defined:

```c
uint32_t get_period(void) {
	return interval_ms;
}
```

##### Part C
The function `timer_start_oneshot` is used, which takes in the interval in milliseconds and a callback function — similar to `timer_init`, but instead leverages TIM3 to create a separate clock that doesn't interfere with TIM2. Internally, it operates similarly using the same prescaler value.  
However, unlike the periodic timer, once the interrupt fires in `TIM3_IRQHandler`, the timer is explicitly disabled inside the handler after calling the callback function. This ensures that the interrupt and timer do not repeat and act only once. This is typically done by clearing the `CEN` (Counter Enable) bit in `TIM3->CR1` after checking and clearing the UIF flag.

```c
void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        if (oneshot_mode && oneshot_callback) {
            callback_t cb = oneshot_callback;
            oneshot_callback = 0;
            oneshot_mode = 0;
            cb();  // One-shot callback
        }
    }
}
```

#### Testing
Largely, `blink_led1` and `blink_led2` have been used for debugging and display purposes:

1. An oscilloscope was used to assure the proper period intervals specified. This was done by applying the oscilloscope to the LED connector solder and observing the on and off periods, as well as confirming a 50% duty cycle.
2. A reset period was inputted with a large difference to observe a visible change once the function was called.
3. The one-shot timer was tested using `blink_led2`, confirming it only triggered once and only after the delay had elapsed.
4. The delay periods use `uint32_t` input types. If an invalid input (e.g., a string) were used, compilation would fail, preventing undefined behavior.

#### Summary of Modularity
- Clean separation between TIM2 and TIM3 allows for concurrent periodic and one-shot functionality.
- Callback functions are passed as function pointers, enabling reusable, flexible code.
- Common configurations (e.g., prescaler, ARR setting, update forcing) are abstracted into helper functions.
- Assembly annotations to help with general low level hardware code flow.


### Integration
#### Code Description
The Integration code was designed to use the modules created in the previous exercises without changing them. The program was developed to receive a command string from the serial port. The string is then parsed to determine which action was requested by the user. The input string to the serial port is made up of two parts, the command (which specifies what type of action is to occur) and the operand (which provides the data to be used in the action).

#### Modular Design
The previous exercises above have code that is designed to act as independent modules and still be easily accessed to make the completed integration code. The ParseInput() function is the core logic module that enables the modular design. It is able to read the action command and choose the needed function from the required module without having to change any of the existing code.

Input:
* uint8_t *input: A pointer to the buffer containing the full command string received via serial.
* uint8_t length: The number of characters in the command string (including the command and operand).
Output:
* Executes one of the defined actions:
	- Produces an LED output pattern
	- Echoes a string back to the serial
	- Starts a one-shot timer
	- Starts a continuous timer
* Sends a response or error over serial if needed.

```c
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
```

How the ParseInput() function contributes to a modular design
* The function deals solely with interpreting and acting on commands. It does not handle receiving input or output directly, which is handled by SerialInputString() and SerialOutputString() respectively.
* New commands can be added by simply adding new if-else statements without modifying other parts of the program.
* It uses strchr() to find the command/operand boundary and strncpy() to safely copy strings, which keeps parsing code self-contained and avoids duplication elsewhere.
* If the input format is invalid, the function responds with an error message instead of crashing or executing undefined behavior.




#### Logic Description
The user is to send a string containing the command and the operand in the serial port.

SerialInputString() will read the characters from the serial port. Once a complete message (terminated with #) is received, it is passed to ParseInput().
ParseInput() separates the command from the operand using the first space in the string.
```c
void SerialInputString(uint8_t *pt, SerialPort *serial_port, uint8_t terminating) {
	uint32_t counter = 0;
	uint8_t *start_of_string = pt;								/*initialise pointer to start of string
																to pass to callback function*/

	// Read first character
	SerialInputChar(pt, serial_port);
	counter++;

	// Keep reading until terminating character is received
	while (*pt != terminating) {
		pt++;  													// Move pointer
		SerialInputChar(pt, serial_port);
		counter++;
		/*if(counter >= buffer_size){
		   break;
		 }
		 */
	}

	//serial_port->completion_function(start_of_string, counter); //callback function
	ParseInput(start_of_string, counter);
}
```


Based on the command:
"led" converts a binary string to a byte and sends it to the set_led_state function as an input, which will then display the operand on the LEDs.
```c
if (strcmp(command, "led") == 0) {
        // Convert binary string to byte and output to GPIO (stub here)
        uint8_t led_state = (uint8_t)strtol(operand, NULL, 2);
        // Send pattern to LEDs
        set_led_state(led_state); //LED DISPLAY FUNCTION
```

"serial" sends the operand to the SerialOutputString function, which echoes the operand in the serial.
```c
} else if (strcmp(command, "serial") == 0) {
        // Echo the operand back through serial
        SerialOutputString(operand, &USART1_PORT);
        SerialOutputString("\r\n", &USART1_PORT);
```

"oneshot" sends the operand to the StartOneshotTimer function, which starts a one-shot timer using the operand as duration in ms.
```c
} else if (strcmp(command, "oneshot") == 0) {
        uint32_t duration = (uint32_t)strtoul(operand, NULL, 10);
        StartOneShotTimer(duration, blink_leds36710);
```

"timer" sends the operand to the StartContinuousTimer function, which starts a repeating timer with the operand as the period in ms.
```c
} else if (strcmp(command, "timer") == 0) {
        uint32_t period = (uint32_t)strtoul(operand, NULL, 10);
        StartContinuousTimer(period, blink_leds4895);
```

If the command is not recognized, a message is returned over serial.
```c
} else {
        SerialOutputString("Unknown command\r\n", &USART1_PORT);
    }
```


#### User Instructions
To run and test the integration code successfully, follow the instructions below. These steps assume the use of a PC with a serial terminal program (e.g., PuTTY or CuteCom) connected via USB to your STM32 board (instructions listed at beginning of file).

Open the main.c file and upload the code to the board by running the program. Then navigate to the serial terminal program you are using, and type in a command with the following format:
```c
<command> <operand>#
```
Where
* <command> = one of the supported action words (led, serial, oneshot, timer)
* <operand> = value for that command
* # = terminates the command

NOTE: You must include the # for the data to be read

