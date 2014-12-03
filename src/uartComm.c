/**
	* @file uartComm.c
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


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/


#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.h"
#include "usart.h"
#include "uartComm.h"
#include "serialFrame.h"

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
uint8_t uartCommSendChar( uint8_t c )
{ 		
	uint8_t errorCode = 0;
	
	if ( ( errorCode = Send_Data_USART( USART1, c ) ) != 0 )
	{
		// NOP
	}
	
	return errorCode;
}
	
/*******************************************************************************
	* uartCommToggleLEDStatus
	*
	*			Set the UART error LED (closest to the UART pins)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void uartCommToggleLEDStatus()
{
		GPIO_Toggle(GPIOB, 5);		// Toggle status LED
}


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
uint8_t uartCommInit( void )
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
		uartCommToggleLEDStatus();
		
		// I/O configuration for Tx
		GPIO_Configure( GPIOA, 9, OUTPUT, ALT_PPULL );	// Tx 
	}
	
	return errorCode;
}

/*******************************************************************************
	* uartCommSendData
	*
	*			Send an array of uint8_t to USART serie protocol
	* 			
	* @param Array of uint8_t to send  
	* @return 0 if no error else the number of errors during send
	******************************************************************************/
uint8_t uartCommSendData( uint8_t * array, uint16_t size )
{ 		
	uint8_t errorCode = 0;
	uint16_t idChar = 0;
	GPIO_Set(GPIOB, 5);
	for (idChar = 0; idChar < size; idChar++ )
	{
			// Affichage du caractere 
			errorCode += Send_Data_USART( USART1, array[idChar] );
	}

	if (errorCode == 0)
	{
		uartCommToggleLEDStatus();
	}	
	GPIO_Clear(GPIOB, 5);
	return errorCode;
}

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
uint8_t uartCommSendTimes( uint32_t time1, uint32_t time2, uint32_t time3, uint32_t time4 )
{
	
	uint8_t * frame = createSendableFrame( TIME_FRAME, time1, time2, time3, time4);
	
	return uartCommSendData( frame, CONVERTED_SERIAL_FRAME_SIZE );
	
}
