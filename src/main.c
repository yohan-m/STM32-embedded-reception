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

#include "global.h"

/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	
	
void Clignote(void)		// Interruption routine
{
	GPIO_Toggle(GPIOC, 12);
}


/******************************************************************************
	*
	*   MAIN FUNCTION
	*
	*****************************************************************************/

int main (void)
{


	/******************
	 * Main variables *
	 ******************/
	
	uint16_t * signal;
	uint32_t time[] = { 0x00112233, 0x44556677, 0x8899AABB, 0xCCDDEEFF };
	uint32_t coefficient[] = { 555, 6666, 77777, 8888888 };

	uint8_t signalOutputfilter[4];

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
	Timer_1234_Init( TIM2, 100000.0 );	// Configuration of Timer 2 (float value un µs)
																									// Real_period contains the real period as close as demanded

	// IT on timer overflow
	Active_IT_Debordement_Timer( TIM2, 1, Clignote );  // Allows interrupt for Timer 2 overflow (priority 1)
  
	// UART communication
	//uartCommInit();
	
	// USB communication
	//usbCommInit();	

	// Signals acquisition
	sampleAcquisitionInit();
	
	// Signals filtering
	signalTraitementInit();
	
	
	/***********
	 * Process *
	 ***********/
	

	while(1)
	{

		signal = acquireBurstDMA();

		signalProcessing( signal, signalOutputfilter );

		//usbCommSendData( (uint8_t *)signal, NB_SAMPLES_TOTAL*2 ); // Sending NB_SAMPLES_TOTAL of uint16_t
		//usbCommSendCoefficients( coefficient[0], coefficient[1], coefficient[2], coefficient[3] );
	}

	return 0;
}

	