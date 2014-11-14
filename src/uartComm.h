/**
	* @file uartComm.h
	* @brief UART Communication Protocol file core
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


#ifndef S_UART_COMM_H
#define S_UART_COMM_H


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
#define UART_HANDLER_PRIORITY 		4										// Priority



/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	
/*******************************************************************************
	* uartCommSendChar
	*
	*			Send character to USART serie protocol
	* 			
	* @param Char to send  
	* @return 0 if no error
	* @return 1 if error during send
	******************************************************************************/
uint8_t uartCommSendChar( uint8_t c );
	
/*******************************************************************************
	* uartCommToggleLEDStatus
	*
	*			Set the UART error LED (closest to the UART pins)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void uartCommToggleLEDStatus();


/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/


/********************************************************************************
	* uartCommInit
	*
	*      Configure all clocks and registers.
	*			Initialize all GPIOs and Timers
	* 			
	* @param Void
	* @return 0 if no error else 1
	*******************************************************************************/
uint8_t uartCommInit( void );

/*******************************************************************************
	* uartCommSendData
	*
	*			Send an array of uint8_t to USART serie protocol
	* 			
	* @param Array of uint8_t to send  
	* @return 0 if no error else the number of errors during send
	******************************************************************************/
uint8_t uartCommSendData( uint8_t * array, uint16_t size );

/*******************************************************************************
	* uartCommSendTimes
	*
	*			Send a frame containing the four times to the drone via UART
	* 			
	* @param Time of the 1st emitter in ns
	* @param Time of the 2nd emitter in ns
	* @param Time of the 3rd emitter in ns
	* @param Time of the 4th emitter in ns
	* @return 0 if no error else the number of errors during send
	******************************************************************************/
uint8_t uartCommSendTimes( uint32_t time1, uint32_t time2, uint32_t time3, uint32_t time4 );


#endif					/* S_SERIALCOMM_H */
