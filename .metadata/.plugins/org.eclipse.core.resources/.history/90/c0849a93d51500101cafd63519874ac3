#include "serial.h"

#include "stm32f303xc.h"

// We store the pointers to the GPIO and USART that are used
//  for a specific serial port. To add another serial port
//  you need to select the appropriate values.
struct _SerialPort {
	USART_TypeDef *UART;
	GPIO_TypeDef *GPIO;
	volatile uint32_t MaskAPB2ENR;	// mask to enable RCC APB2 bus registers
	volatile uint32_t MaskAPB1ENR;	// mask to enable RCC APB1 bus registers
	volatile uint32_t MaskAHBENR;	// mask to enable RCC AHB bus registers
	volatile uint32_t SerialPinModeValue;
	volatile uint32_t SerialPinSpeedValue;
	volatile uint32_t SerialPinAlternatePinValueLow;
	volatile uint32_t SerialPinAlternatePinValueHigh;
	void (*completion_function)(uint8_t *, uint32_t);
};

// instantiate the serial port parameters
//   note: the complexity is hidden in the c file
SerialPort USART1_PORT = {USART1,
		GPIOC,
		RCC_APB2ENR_USART1EN, // bit to enable for APB2 bus
		0x00,	// bit to enable for APB1 bus
		RCC_AHBENR_GPIOCEN, // bit to enable for AHB bus
		0xA00,
		0xF00,
		0x770000,  // for USART1 PC10 and 11, this is in the AFR low register
		0x00, // no change to the high alternate function register
		0x00 // default function pointer is NULL
		};

//definitions for TXI
uint8_t *send_data = "send it!!";
uint8_t txIndex = 0;
uint8_t txLength = 9;

//definitions for double buffer
uint8_t uartKernel = 0;
uint8_t uartUser = 1;
uint8_t bufferCounter[2];

// InitialiseSerial - Initialise the serial port
// Input: baudRate is from an enumerated set
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint8_t *, uint32_t)) {

	serial_port->completion_function = completion_function;

	// enable clock power, system configuration clock and GPIOC
	// common to all UARTs
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// enable the GPIO which is on the AHB bus
	RCC->AHBENR |= serial_port->MaskAHBENR;

	// set pin mode to alternate function for the specific GPIO pins
	serial_port->GPIO->MODER = serial_port->SerialPinModeValue;

	// enable high speed clock for specific GPIO pins
	serial_port->GPIO->OSPEEDR = serial_port->SerialPinSpeedValue;

	// set alternate function to enable USART to external pins
	serial_port->GPIO->AFR[0] |= serial_port->SerialPinAlternatePinValueLow;
	serial_port->GPIO->AFR[1] |= serial_port->SerialPinAlternatePinValueHigh;

	// enable the device based on the bits defined in the serial port definition
	RCC->APB1ENR |= serial_port->MaskAPB1ENR;
	RCC->APB2ENR |= serial_port->MaskAPB2ENR;

	// Get a pointer to the 16 bits of the BRR register that we want to change
	uint16_t *baud_rate_config = (uint16_t*)&serial_port->UART->BRR; // only 16 bits used!

	// Baud rate calculation from datasheet
	switch(baudRate){
	case BAUD_9600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_19200:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_38400:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_57600:
		// NEED TO FIX THIS !
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	case BAUD_115200:
		*baud_rate_config = 0x46;  // 115200 at 8MHz
		break;
	}

	// enable serial port for tx and rx
	serial_port->UART->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}


void SerialOutputChar(uint8_t data, SerialPort *serial_port) {

	while((serial_port->UART->ISR & USART_ISR_TXE) == 0){		//check ready to transmit flag
	}

	serial_port->UART->TDR = data;		//load data into blank
}

void SerialInputChar(uint8_t *data, SerialPort *serial_port) { //blank maybe add

	//serial_port->UART->ICR |= USART_ICR_FECF | USART_ICR_ORECF;
	//uint8_t test = serial_port->UART->ISR & USART_ISR_RXNE;
	//serial_port->UART->ICR |= (USART_ICR_ORECF | USART_ICR_FECF);
    while ((serial_port->UART->ISR & USART_ISR_RXNE) == 0) {
    }
    //test = serial_port->UART->ISR & USART_ISR_RXNE;
    // Read received character
    *data = (uint8_t)(serial_port->UART->RDR); // Reading RDR clears RXNE

    //serial_port->UART->RQR = USART_RQR_RXFRQ;

}
// Wait until a character is received

// Check for Overrun Error (ORE) and clear it
/*if (serial_port->UART->ISR & (USART_ISR_ORE | USART_ISR_FE) != 0){
	(void)serial_port->UART->RDR;
	serial_port->UART->ICR |= USART_ICR_FECF | USART_ICR_ORECF;
}
*/



void SerialOutputString(uint8_t *pt, SerialPort *serial_port) {
	uint8_t counter = 0;

	while(*pt) {
		SerialOutputChar(*pt, serial_port);
		counter++;
		pt++;
	}

	//serial_port->completion_function(pt, counter);
}

void SerialInputString(uint8_t *pt, SerialPort *serial_port, uint8_t terminating) {
	uint32_t counter = 0;
	uint8_t *start_of_string = pt;


	    // Read first character
	    SerialInputChar(pt, serial_port);
	    counter++;

	    // Keep reading until '#' is received
	    while (*pt != terminating) {
	        pt++;  // Move pointer
	        SerialInputChar(pt, serial_port);
	        counter++;
	    }

	    serial_port->completion_function(start_of_string, counter);
}



void InterruptOutputChar(){
	while((USART1_PORT.UART->ISR & USART_ISR_TXE) == 0){
		}
	if (txIndex < txLength) {
		USART1_PORT.UART->TDR = send_data[++txIndex];
	}
	else {
		// Transmission complete, disable TXE interrupt
		USART1->CR1 &= ~USART_CR1_TXEIE;
	}

}

void InterruptOutputString(){
		when_sending_data = &InterruptOutputChar;

		SerialOutputChar(send_data[txIndex], &USART1_PORT);

		USART1->CR1 |= USART_CR1_TXEIE; //enable transmit interrupt

}

void InterruptInputString(uint8_t *buffer, SerialPort *serial_port){
	if ((serial_port->UART->ISR & USART_ISR_RXNE) != 0){
		*buffer = (uint8_t)(serial_port->UART->RDR);
	}

}

void SerialInputStringdb(uint8_t buffer[][32], SerialPort *serial_port) {

	    // Read first character
	   /* SerialInputChar(&buffer[uartKernel][bufferCounter[uartKernel]], serial_port);
	    bufferCounter[uartKernel] ++;

	    // Keep reading until '#' is received
	    while (buffer[uartKernel][bufferCounter[uartKernel]-1] != terminating) {
	    	SerialInputChar(&buffer[uartKernel][bufferCounter[uartKernel]], serial_port);
	    	bufferCounter[uartKernel] ++;
	    }*/

	    if ((serial_port->UART->ISR & USART_ISR_RXNE) != 0){
	    	buffer[uartKernel][bufferCounter[uartKernel]] = (uint8_t)(serial_port->UART->RDR);
	    	bufferCounter[uartKernel] ++;
	    }

	   /* if(buffer[uartKernel][bufferCounter[uartKernel] - 1] == '#'){
	    	    switch_buffers();
	    }*/


}

void InputLogic(uint8_t buffer[][32]){
	if (bufferCounter[uartUser] > 0)
	{
		for(int i = 0; i <= bufferCounter[uartUser]; i++){
				SerialOutputChar(buffer[uartUser][i], &USART1_PORT);

			}
	}
	switch_buffers();

}

void switch_buffers(){
	uartUser = (!uartUser) & 0x01;
	uartKernel = (!uartKernel) & 0x01;

	// Need to reset the counter for the ISR
	bufferCounter[uartKernel] = 0;
}

void ParsingInput(uint8_t *buffer){

}






