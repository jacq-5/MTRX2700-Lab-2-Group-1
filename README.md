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

## Test procedures:


## Code Explanation
### Exercise 1: Digital I/O  
Exercise 1 interfaces the LEDs and buttons for the STM32F3 discovery board to demonstrate the principles of software design. The LED state is accessed through the header file using get/set functions that take the current state of the LEDs which is defined as a global variable and shifted based on the callback function, then set the current state of the LEDs (first initalised to a known state in the main function). The LED 'blinking' 


#### 	Part A:
<H4>Modular Design</H4>
<pre> 
   Write pseudo code here
</pre>

#####  Logic Overview
#####  How to run the code
#####  Test Cases

#### 	Part B:
##### 	How to run the code 

##### How to test the code 

### Exercise 2: Serial Interface

#### Part A:
Data is received from a UART into a designated buffer. If the data is larger than the designated buffer size the extra characters will not be stored. 
##### Modular Design
Input: buffer, serial port, terminating character  
Output: received data will be loaded into buffer
<pre>Main{
	Initialise serial port
	Initialise buffer
	Calculate buffer size
	Initialise terminating character
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
</pre>

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
    Input: hello#  
    Output: buffer = hello#

No terminating character  
	Input: hello  
	Output: buffer = hello  
Limitation - without terminating character program stays in receive loop writing for more characters

Data > buffer size  
	Input: What A Wonderful World - Disney Land  
	Output: buffer = What A Wonderful World - Disney

#### Part B:
Once a terminating character has been received a call back function will be called. This function is set when initialising the serial port.

##### Modular Design
Input: buffer, serial port, terminating character, callback function  
Output: received data will be retransmitted to serial communication interface

Same as part (a) but with the following additions to each function

<pre>
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
</pre>

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
	Input: Hello#  
	Output: cutecom - Hello#

No Terminating Character  
	Input: Hello  
	Output: cutecom - 

Data > buffer size  
	Input: What A Wonderful World - Disney Land  
	Output: cutecom - What A Wonderful World - Disney


#### Part C:
An interrupt is used when receiving data from a USART so data can be received anytime throughout the program. The incoming data will be placed in the buffer and considered complete when a terminating character is received. All data after this will be discarded.

##### Modular Design
Same as part (a) but with the following additions to each function

Input: same as part (a)  
Output: same as part (a)

<pre>
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
</pre>

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

<pre>
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
</pre>

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
	Input: send_data = GoTeam2025!!!  
	Output: Cutecom - GoTeam2025!!!

#### Part D: Double Buffer
The program receives and processes data using a double buffer, so that new data can be received in the middle of processing previous data without overwriting any of it. 

##### Modular Design
Input: buffer, serial port  
Output: received data will be retransmitted to serial communication interface  
<pre>
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
</pre>

##### Logic Description
Data is received through USART1 by interrupts. Once a transmission is complete, Logic processes this data however if more data is sent during this the interrupt handler will be called to receive this new data. The double buffer allows for this new data to be received into a separate location, not overwriting the data that is not processed yet. Once this new transmission is complete the program will resume where it left off processing the old data and once this is finished it will switch which buffer it is processing and begin processing the new data.

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

