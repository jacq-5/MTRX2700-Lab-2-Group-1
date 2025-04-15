## MTRX2700 Lab 2 Group 1
# Thursday 2pm-5pm

## Members
Hailey Thill,
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
 
6. Within the project in the source folder select assembly.s file and click the debug button.

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
<pre> 
   Main:
	Initialise serial port

   SerialInputString(buffer, serial_port, terminating character){
   Initialise a counter
   Calculate buffer length
   
   SerialInputChar()
   While (character != terminating character){
   	If counter > buffer length {
   		Break
   	}
   	SerialInputChar()
   }
   }
   
   SerialInputChar(){
   	Check ready to receive flag 
   Buffer = incoming character
   }
</pre>

##### Logic Description
When SerialInputString is called the program will enter SerialInputChar and loop there until the ready to receive flag is on. The program will then load the first character into the buffer, return to SerialInputString and check if this character was the terminating character. If not, SerialInputChar is continuously called until the terminating character is received. The program will then exit the function. Furthermore after each character is loaded into the buffer the counter is compared with buffer length, if the buffer is full the program exits the while loop and function meaning no further characters are received.

##### User Instructions
   1. Initialise your buffer, serial port and terminating character 
   2. Insert the functions into your code.
   3. Call SerialInputString with the arguments of your buffer, serial port and terminating character. 

##### Test Cases
Terminating character
Input: hello#
Output: buffer = hello#

No terminating character
Input: hello
Output: buffer = 

Data > buffer size
Input: What A Wonderful World - Disney Land
Output: buffer = 


