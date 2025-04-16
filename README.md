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
Input: 'buffer, buffer size, serial_port, terminating character'  
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
When SerialInputString is called the program will enter SerialInputChar and loop there until the ready to receive flag is on. The program will then load the first character into the buffer, return to SerialInputString and check if this character was the terminating character. If not, SerialInputChar is continuously called until the terminating character is received. The program will then exit the function. Furthermore after each character is loaded into the buffer the counter is compared with buffer length, if the buffer is full the program exits the while loop meaning no further characters are received.

##### User Instructions
   1. Initialise your buffer, serial port and terminating character
<pre>
SerialInitialise(BAUD_115200, &serialPort)
uint8_t terminating = '#';
uint8_t buffer[32];
uint32_t buffer_size = sizeof(buffer)/sizeof(buffer[0]);	
</pre>
   2. Call SerialInputString with the arguments of your buffer, serial port and terminating character.
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
Input: buffer, serial port, terminating character, callback function  
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
The typedef Struct SerialPort has the function pointer completion function. The address of the desired call back function is passed into SerialInitialise and this is set as the serial ports completion function. Once the program has finished receiving data from the designated serial port in SerialInputString, this serial port’s completion function is called.

##### User Instructions
   1. Initialise call back function by passing it’s address into SerialInitialise
<pre>
SerialInitialise(BAUD_115200, &USART1_PORT, &callBackFunction);
</pre>
   2. Call the callback function at end of SeriaInputString
<pre>
serial_port->completion_function(start_of_string, counter); //callback function	
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
	allows USART1 interrupt to occur
Re-enable interrupts
}

Interrupt handler{
	If interrupt function has been set{
		Call interrupt function
}
}

void InterruptInputString(buffer, buffer size, serial_port){
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
In enableInterrupts USART1’s receive interrupt is enabled and it’s priority is set. The function called in the interrupt handler is defined. The main function then goes into an infinite loop and when data is sent to USART1 the interrupt handler is called. The interrupt handler checks if a function has been set and then calls this function. Once the terminating character has been received or the buffer is full all new characters will be loaded into a discard variable.

##### User Instructions
   1. Initialise buffer, buffer size and counter variables in their designated scopes. 
<pre>
uint8_t buffer[32];
uint32_t buffer_size = sizeof(buffer)/sizeof(buffer[0]);

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
Input: data to transmit[send_data], index [txTndex], length of string [txLength]  
Output: USART transmits string 

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
InterruptOutputString is called in the main function. The transmit interrupt function is set and the first character is manually transmitted. USART1’s transmit interrupt is then enabled. The interrupt will continually be called as the following characters trigger the ready to transmit flag. The interrupt handler calls InterruptOutputChar which checks if all characters have been transmitted yet, if not it transmits the next characters, otherwise it will disable the transmit interrupt so the program is not continually interrupted when there is no more data to send.

##### User Instructions
   1. Initialise the string you want to transmit, as well as it’s length and an index function
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

##### Test Cases
Upper case, lower case, numbers and special characters  
- Input: send_data = GoTeam2025!!!  
- Output: Cutecom - GoTeam2025!!!

#### Part D: Double Buffer
The program receives and processes data using a double buffer, so that new data can be received in the middle of processing previous data without overwriting any of it. 

##### Modular Design
Input: buffer, serial port  
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

Data > buffer size
- Input: What a wonderful world! Disneyland is magical :)
- Output: What a wonderful world! Disneyl

