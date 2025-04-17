#ifndef SERIAL_PORT_HEADER
#define SERIAL_PORT_HEADER


#include <stdint.h>
#include<string.h>

// Defining the serial port struct, the definition is hidden in the
// c file as no one really needs to know this.
struct _SerialPort;
typedef struct _SerialPort SerialPort;
typedef void (*callback_t)(void);

// make any number of instances of the serial port (they are extern because
//   they are fixed, unique values)
extern SerialPort USART1_PORT;


//define a buffer
/*extern uint8_t buffer[2][32];
extern uint8_t bufferCounter[2][1];
extern uint8_t uartUser;
extern uint8_t uartKernel;*/



//definitions for TXI
extern uint8_t *send_data;
extern uint8_t txIndex;
extern uint8_t txLength;

// The user might want to select the baud rate
enum {
  BAUD_9600,
  BAUD_19200,
  BAUD_38400,
  BAUD_57600,
  BAUD_115200
};

void initialise_leds(void);

void set_led_state(uint8_t state);
 
// SerialInitialise - initialise the serial port
// Input: baud rate as defined in the enum
void SerialInitialise(uint32_t baudRate, SerialPort *serial_port, void (*completion_function)(uint8_t *, uint32_t) );
 

// SerialOutputChar - output a char to the serial port
//  note: this version waits until the port is ready (not using interrupts)
// Input: char to be transferred
void SerialOutputChar(uint8_t, SerialPort *serial_port);

// SerialOutputChar - input a char from the serial port
void SerialInputChar(uint8_t *data, SerialPort *serial_port);
 

// SerialOutputString - output a NULL TERMINATED string to the serial port
// Input: pointer to a NULL-TERMINATED string (if not null terminated, there will be problems)
void SerialOutputString(uint8_t *pt, SerialPort *serial_port);

//reads characters in to a buffer
void SerialInputStringdb(uint8_t buffer[][32], SerialPort *serial_port);

void SerialInputString(uint8_t *buffer, SerialPort *serial_port, uint8_t terminating);

void callback_function(uint8_t *rx_string, uint32_t counter);

void InterruptOutputString();

void InterruptOutputChar();


void InputLogic(uint8_t buffer[][32]);

extern void (*when_sending_data)();

void switch_buffers();

void InterruptInputString(uint8_t *buffer, SerialPort *serial_port);

//timer functions

void initialise_board();

void enable_clocks();

void StartOneShotTimer(uint32_t delay_ms, callback_t cb);

void StartContinuousTimer(uint32_t interval, callback_t cb);

void blink_leds4895(void);

void blink_leds36710(void);

//integration function

void ParseInput(uint8_t *input, uint8_t length);
 
#endif
