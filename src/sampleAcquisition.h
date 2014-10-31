/**
	* @file sampleAcquisition.h
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


#ifndef S_SAMPLEACQUISITION_H
#define S_SAMPLEACQUISITION_H


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


#define ADC_ID										ADC1								// ADC ID
#define ADC_HANDLER_PRIORITY 			1 									// EOC ADC Interruption Priority
#define ADC_CHANNEL								10									// ADC channel
#define ADC_CONVERSION_TIME				7										// Time of conversion for 1 sample in µs
#define ADC_SAMPLING_FREQUENCY		128									// Sampling frequency in kHz
#define SAMPLING_PERIOD						20									// Time of a complete sample in ms
#define NB_SAMPLES_TOTAL					(ADC_SAMPLING_FREQUENCY*SAMPLING_PERIOD) // Number of samples


/******************************************************************************
	*
	*   PRIVATE FUNCTIONS
	*
	*****************************************************************************/
	

uint8_t sampleAcquisitionInit( void );
uint16_t acquireSample ( void );
uint16_t * acquireSignals( void );


#endif					/* S_SERIALCOMM_HS_SAMPLEACQUISITION_H */
