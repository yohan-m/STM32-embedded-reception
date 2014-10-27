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


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/


#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.h"
#include "usart.h"
#include "serialComm.h"


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
uint8_t serialCommSendChar( uint8_t c )
{ 		
	uint8_t errorCode = 0;
	
	if ( ( errorCode = Send_Data_USART( USART1, c ) ) != 0 )
	{
		// NOP
	}
	
	return errorCode;
}
	
/*******************************************************************************
	* serialCommToggleLEDStatus
	*
	*			Set the UART error LED (closest to the UART pins)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void serialCommToggleLEDStatus()
{
		GPIO_Toggle(GPIOB, 5);		// Toggle status LED
}


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
uint8_t serialCommInit( void )
{
	
	uint8_t errorCode = 0; 

	// I/O configuration for Error_Code
	GPIO_Configure( GPIOB, 5, OUTPUT, OUTPUT_PPULL );	// USART ERROR LED (On/Off)
	
	if ((errorCode = Init_USART (
																UART_ID,
																UART_BAUD_RATE,
																UART_PARITY,
																UART_STOP_BIT_MODE1,
																UART_WORD_LENGTH
															)) != 0 )
	{
		// Error in initialization process
		// NOP
	}
	else
	{
		// No error in initialization process
		serialCommToggleLEDStatus();
		
		// I/O configuration for Tx
		GPIO_Configure( GPIOA, 9, OUTPUT, ALT_PPULL );	// Tx 
	}
	
	return errorCode;
}

/*******************************************************************************
	* serialCommSendData
	*
	*			Send an array of uint8_t to USART serie protocol
	* 			
	* @param Array of uint8_t to send  
	* @return 0 if no error
	* @return The number of errors during send
	******************************************************************************/
uint8_t serialCommSendData(uint8_t * array, uint16_t size)
{ 		
	uint8_t errorCode = 0;
	uint16_t idChar = 0;
		
	for (idChar = 0; idChar < size; idChar++ )
	{
			// Affichage du caractere 
			errorCode += Send_Data_USART( USART1, array[idChar] );
	}

	if (errorCode == 0)
	{
		serialCommToggleLEDStatus();
	}	
	
	return errorCode;
}