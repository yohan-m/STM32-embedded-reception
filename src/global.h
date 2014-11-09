/**
	* @file global.h
	* @brief Global file
	*			
	* 		Last modification : 07 Nov 2014
	*
	* @author Miquèl RAYNAL
	* @version 0.1
	* @date 29 Oct 2014
	*/


#ifndef _GLOBAL_H__
#define _GLOBAL_H__


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/


#include <stdint.h>
#include <string.h>
#include "stm32f10x.h"
#include "Clock.h"
#include "Timer_1234.h"
#include "GPIO.h"
#include "xprintf.h"	// For debug purpose only
#include "sampleAcquisition.h"
#include "usb_cdc.h"
#include "uartComm.h"
#include "usbComm.h"
#include "signalFilter.h"


/******************************************************************************
	* 
	*   CONSTANTS
	*
	*****************************************************************************/
	
	
typedef enum
{
	OFF = 0,
	ON = 1,
} LEDState;


#endif /* _GLOBAL_H__ */