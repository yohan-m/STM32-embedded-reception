/**
	* @file serialComm.c
	* @brief Serial Communication Protocol file core
	*
	*     This file contains the service serial communication RS232 (UART).
	*			Initialization, configuration and send mode. The status LED toggles if Tx is valid.
	*
	* 		Last modification : 25 Oct 2014
	*
	* @author Miquel RAYNAL
	* @version 0.1
	* @date 25 Oct 2014
	*/


#ifndef S_SERIALCOMM_H
#define S_SERIALCOMM_H


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/


#include <stdint.h>


/******************************************************************************
	* 
	*   CONSTANTS
	*
	*****************************************************************************/


#define UART_ID										USART1							// UART to use
#define UART_BAUD_RATE						9600								// Baudrate
#define UART_PARITY								DISABLED						// Parity
#define UART_STOP_BIT_MODE1				STOP_BIT_MODE1			// Number of stop bits
#define UART_WORD_LENGTH					WORD_LENGTH_8BITS		// Word length
#define UART_HANDLER_PRIORITY 		0										// Priority

typedef enum // Error LED state
{
	LED_ON,
	LED_OFF
} t_LEDState;


/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	
/*******************************************************************************
	* serialCommSendChar
	*
	*			Send character to USART serie protocol
	* 			
	* @param Char to send  
	* @return 0 if no error
	* @return 1 if error during send
	******************************************************************************/
uint8_t serialCommSendChar( uint8_t c );
	
/*******************************************************************************
	* serialCommToggleLEDStatus
	*
	*			Set the UART error LED (closest to the UART pins)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void serialCommToggleLEDStatus();


/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/


/********************************************************************************
	* serialCommInit
	*
	*      Configure all clocks and registers.
	*			Initialize all GPIOs and Timers
	* 			
	* @param Void
	* @return 0 if no error
	* @return 1 if error takes place in the initialization
	*******************************************************************************/
uint8_t serialCommInit( void );

/*******************************************************************************
	* serialCommSendData
	*
	*			Send an array of uint8_t to USART serie protocol
	* 			
	* @param Array of uint8_t to send  
	* @return 0 if no error
	* @return The number of errors during send
	******************************************************************************/
uint8_t serialCommSendData(uint8_t * array, uint16_t size);


#endif					/* S_SERIALCOMM_H */
