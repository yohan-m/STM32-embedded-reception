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
void usbCommInit( void );
	
/*******************************************************************************
	* usbCommSendChar
	*
	*			Send character via USB
	* 			
	* @param Char to send  
	* @return Void
	******************************************************************************/
void usbCommSendChar( uint8_t c );

/*******************************************************************************
	* usbCommSendData
	*
	*			Send an array of uint8_t via USB
	* 			
	* @param Array of uint8_t to send  
	* @param Size of the array
	* @return Void
	******************************************************************************/
void usbCommSendData( uint8_t * array, uint16_t size );

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
void usbCommSendTimes( uint32_t time1, uint32_t time2, uint32_t time3, uint32_t time4 );


#endif					/* S_SERIALCOMM_H */
