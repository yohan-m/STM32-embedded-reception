/**
	* @file sampleAcquisition.c
	* @brief ADC acquisition of an ultrasound sample
	*
	*     This file contains the service for acquire sample from the ADC.
	*			Initialization, configuration and start. The status LED toggles at each valid sample saved.
	*			
	* 		Last modification : 29 Oct 2014
	*
	* @author Miquèl RAYNAL
	* @version 0.1
	* @date 29 Oct 2014
	*/


#include <stdint.h>
#include "stm32f10x.h"
#include "gpio.h"
#include "sampleAcquisition.h"
#include "adc_dma.h"

float sampleTime;
uint16_t adcBuffer[NB_SAMPLES_TOTAL];

uint8_t sampleAcquisitionInit( void )
{
	// I/O configuration on ADC1 input, channel 10
	GPIO_Configure( GPIOC, 0, INPUT, ANALOG );														// ADC_In10 (channel 10) 
	
	// ADC Initialization
	sampleTime = Init_TimingADC_ActiveADC( ADC_ID, ADC_CONVERSION_TIME );	// Samples of 1us
	Single_Channel_ADC( ADC_ID, ADC_CHANNEL ); 														// ADC 1 on channel 10
	
	return 0;
}

uint16_t acquireSample ( void )
{	
	// Start conversion
	Start_ADC_Conversion( ADC_ID );
	// Wait EOC
	Wait_On_EOC_ADC( ADC_ID );
	// Read value
	return Read_ADC( ADC_ID );
}

/*uint8_t sampleAcquisitionStart( void )
{
	uint8_t idMeasure = 0;

	// The first 30 measures are trashed
	for ( idMeasure = 0; idMeasure < 30; idMeasure++ )
	{
		acquireSample();
	}	
	return 0;
}*/

uint16_t * acquireSignals( void )
{
	
	uint32_t idSample = 0;
	for ( idSample = 0; idSample < NB_SAMPLES_TOTAL ; idSample++ )
	{
		adcBuffer[idSample] = acquireSample();
	}
	
	return adcBuffer;
}
