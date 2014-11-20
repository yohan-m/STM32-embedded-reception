/**
	* @file sampleAcquisition.c
	* @brief ADC acquisition of an ultrasound sample
	*
	*     This file contains the service for acquire sample from the ADC.
	*			Initialization, configuration and start. The status LED turn on at each acquisition.
	*			
	* 		Last modification : 07 Nov 2014
	*
	* @author Miquèl RAYNAL
	* @version 0.1
	* @date 29 Oct 2014
	*/


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/
	
	
#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.h"
#include "sampleAcquisition.h"
#include "global.h"
#include "adc_dma.h"


/******************************************************************************
	* 
	*   VARIABLES
	*
	*****************************************************************************/


float sampleTime;												// Real sample time
uint16_t adcBuffer[NB_SAMPLES_TOTAL];		// Static buffer to save a signal


/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	
	
/*******************************************************************************
	* setLEDAcquisition
	*
	*			Set the acquisition LED (3rd pin)
	* 			
	* @param Void  
	* @return Void
	******************************************************************************/
void setLEDAcquisition( uint8_t status )
{
		GPIO_Write( GPIOB, 5, status );		// Toggle status LED
}


/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/


/********************************************************************************
	* sampleAcquisitionInit
	*
	*      Init the sampling routine.
	*				Blocking function.
	* 			
	* @param Void
	* @return 0 if working
	*******************************************************************************/
uint8_t sampleAcquisitionInit( void )
{
	// I/O configuration on ADC1 input, channel 10
	GPIO_Configure( GPIOC, 0, INPUT, ANALOG );														// ADC_In10 (channel 10) 
	
	// ADC Initialization
	sampleTime = Init_TimingADC_ActiveADC( ADC_ID, ADC_CONVERSION_TIME );	// Samples of 1us
	Single_Channel_ADC( ADC_ID, ADC_CHANNEL ); 														// ADC 1 on channel 10
	
	// LED 3 turn on at each burst acquisition
	GPIO_Configure( GPIOB, 5, OUTPUT, OUTPUT_PPULL );
	setLEDAcquisition( OFF );
	
	return 0;
}

/********************************************************************************
	* acquireSample
	*
	*      Manual acquisition of one sample
	*				Blocking function.
	* 			
	* @param Void
	* @return The sampled value from the ADC
	*******************************************************************************/
uint16_t acquireSample ( void )
{	
	// Start conversion
	Start_ADC_Conversion( ADC_ID );
	// Wait EOC
	Wait_On_EOC_ADC( ADC_ID );
	// Read value
	return Read_ADC( ADC_ID );
}

/********************************************************************************
	* acquireBurstManual
	*
	*      Manual acquisition of an array of samples.
	*				Blocking function.
	* 			
	* @param Void
	* @return A pointer on the filled array
	*******************************************************************************/
uint16_t * acquireBurstManual( void )
{
	
	uint32_t idSample = 0;
	for ( idSample = 0; idSample < NB_SAMPLES_TOTAL ; idSample++ )
	{
		adcBuffer[idSample] = acquireSample();
	}
	
	return adcBuffer;
}

/********************************************************************************
	* acquireBurstDMA
	*
	*      Automatic acquisition of an array of samples (using DMA).
	*				Blocking function.
	* 			
	* @param Void
	* @return A pointer on the filled array
	*******************************************************************************/
uint16_t * acquireBurstDMA( void )
{

	// LED 3 turned on at each acquisition
	//setLEDAcquisition( ON );
	
	// Conversion on timer trig
	Init_Conversion_On_Trig_Timer(ADC_ID , TIM1_CC1, ADC_SAMPLING_FREQUENCY);	
	
	Wait_On_EOC_ADC(ADC_ID);
	
	
	// DMA stores acquisition data (u16) in buffer
	Init_ADC1_DMA1(DMA_NON_CIRCULAR, (vu16 *) adcBuffer);

	// Starts DMA to fulfill the buffer
	Start_DMA1( NB_SAMPLES_TOTAL );
	
	// Wait the end of the acquisition at the previously given frequency
	Wait_On_End_Of_DMA1();
	
	setLEDAcquisition( OFF );
	
	return adcBuffer;
}
