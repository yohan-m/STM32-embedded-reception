/**
	* @file usbComm.c
	* @brief Serial Communication Protocol file core
	*
	*     This file contains the service serial communication USB.
	*			Initialization, configuration and send mode. The status LED toggles when a frame is sent.
	*
	* 		Last modification : 07 Nov 2014
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
#include "usb_cdc.h"
#include "usbComm.h"
#include "global.h"
#include "serialFrame.h"

	
/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	
	
/*******************************************************************************
	* setLEDComUSB
	*
	*			Set the USB status LED (2nd pin)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void setLEDComUSB( uint8_t status )
{
	//GPIO_Write( GPIOB, 6, status );		// Toggle status LED
}


/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/


/********************************************************************************
	* usbCommInit
	*
	*      Configure the usb serial communication.
	*				Blocking function.
	* 			
	* @param Void
	* @return Void
	*******************************************************************************/
void usbCommInit( void )
{
	
	Set_System();
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();
	while (USB_GetState() != CONFIGURED) {} // Wait USB is ready
		
	//GPIO_Configure( GPIOB, 6, OUTPUT, OUTPUT_PPULL );
	//setLEDComUSB( OFF );
	
}
	
/*******************************************************************************
	* usbCommSendChar
	*
	*			Send character via USB
	* 			
	* @param Char to send  
	* @return Void
	******************************************************************************/
void usbCommSendChar( uint8_t c )
{ 		

	USB_Send( c );
	
}

/*******************************************************************************
	* usbCommSendData
	*
	*			Send an array of uint8_t via USB
	* 			
	* @param Array of uint8_t to send  
	* @param Size of the array
	* @return Void
	******************************************************************************/
void usbCommSendData( uint8_t * array, uint16_t size )
{ 		
	
	uint16_t idChar = 0;
		
	setLEDComUSB( ON );
	for (idChar = 0; idChar < size; idChar++ )
	{
			// Send one char
			usbCommSendChar( array[idChar] );
	}

	//setLEDComUSB( OFF ); // Toogle the LED when a frame is sent
}

/*******************************************************************************
	* usbCommSendTimes
	*
	*			Send a frame containing the four times to the drone via USB
	* 			
	* @param Time of the 1st emitter in ns
	* @param Time of the 2nd emitter in ns
	* @param Time of the 3rd emitter in ns
	* @param Time of the 4th emitter in ns
	* @return Void
	******************************************************************************/
void usbCommSendTimes( int32_t time1, int32_t time2, int32_t time3, int32_t time4, int32_t rss1, int32_t rss2, int32_t rss3, int32_t rss4 )
{
	
	uint8_t * frame = createSendableFrame( TIME_FRAME, time1, time2, time3, time4, rss1, rss2, rss3, rss4);
	
	usbCommSendChar( SYNC_CHAR_1 );
	usbCommSendChar( SYNC_CHAR_2 );
	usbCommSendData( frame, CONVERTED_SERIAL_FRAME_SIZE );
	
}

/*******************************************************************************
	* usbCommSendCoefficients
	*
	*			Send a frame containing the four coefficients to debug purpose
	* 			
	* @param Coefficient of the 1st emitter
	* @param Coefficient of the 2nd emitter
	* @param Coefficient of the 3rd emitter
	* @param Coefficient of the 4th emitter
	* @return Void
	******************************************************************************/
void usbCommSendCoefficients( uint32_t coef1, uint32_t coef2, uint32_t coef3, uint32_t coef4)
{
	
	usbCommSendChar( SYNC_CHAR_1 );
	usbCommSendChar( SYNC_CHAR_2 );
	usbCommSendData( (uint8_t *)&coef1, 4 );
	usbCommSendData( (uint8_t *)&coef2, 4 );
	usbCommSendData( (uint8_t *)&coef3, 4 );
	usbCommSendData( (uint8_t *)&coef4, 4 );
	
}
void usbCommSendCoefficients64( uint64_t coef1, uint64_t coef2, uint64_t coef3, uint64_t coef4)
{
	
	usbCommSendChar( SYNC_CHAR_1 );
	usbCommSendChar( SYNC_CHAR_2 );
	usbCommSendData( (uint8_t *)&coef1, 8 );
	usbCommSendData( (uint8_t *)&coef2, 8 );
	usbCommSendData( (uint8_t *)&coef3, 8 );
	usbCommSendData( (uint8_t *)&coef4, 8 );
	
}