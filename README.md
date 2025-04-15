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

## Test procedures:


## Code Explanation
### Exercise 1: Digital I/O  

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


