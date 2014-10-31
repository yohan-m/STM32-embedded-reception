/**
	* @file main.c
	* @brief Main function core
	*
	*      This file contains the main function that initializes, configurates
	* 			and start all applications with their services. 
	*
	*			Last modification : 25 Oct 2014
	*
	* @author Miquèl RAYNAL
	* @version 0.1
	* @date 25 Oct 2014
	*/


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

float realPeriod;
char serialText[50];
	
	
void Clignote(void)		// Interruption routine
{
	GPIO_Toggle(GPIOC, 12);
	//serialCommSendData((uint8_t *)serialText, sizeof(serialText));
	//serialCommSendTimes(0x11111111, 0x22222222, 0x33333333, 0x44444444);
}


int main (void)
{

	/*******************
	 * Initializations *
	 *******************/
	// Clock
	CLOCK_Configure();		// Initialization of the system clock
												//	- If the compilation key STM32F107 is defined, the CPU is clocked at 50MHz
												//  - Else (STM32F107 non defined), the CPU is clocked at 40MHz

	
	
	// GPIO (Stat LED)
	GPIO_Configure( GPIOC, 12, OUTPUT, OUTPUT_PPULL ); // Led Stat
	GPIO_Clear( GPIOC, 12 );
	
	// Timer
	realPeriod = Timer_1234_Init( TIM2, 100000.0 );	// Configuration of Timer 2 (float value un µs)
																									// Real_period contains the real period as close as demanded

	// IT on timer overflow
	Active_IT_Debordement_Timer( TIM2, 1, Clignote );  // Allows interrupt for Timer 2 overflow (priority 1)
  
	// UART communication
	uartCommInit();
	
	// USB communication
	usbCommInit();	


	/***********
	 * Process *
	 ***********/

	
	uint16_t * tab;
	sampleAcquisitionInit();


	while(1)
	{

		tab = acquireSignals();
		usbCommSendData( (uint8_t *)tab, 500 );
		
	}

	return 0;
}

	