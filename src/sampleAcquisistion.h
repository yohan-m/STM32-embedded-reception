/**
	* @file sampleAcquisition.h
	* @brief ADC acquisition of an ultrasound sample
	*
	*     This file contains the service for acquire sample from the ADC.
	*			Initialization, configuration and start. The status LED toggles at each valid sample saved.
	*
	* 		Last modification : 07 Nov 2014
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


#define ADC_ID										ADC1						// ADC ID
#define ADC_HANDLER_PRIORITY 			4 							// EOC ADC Interruption Priority
#define ADC_CHANNEL								10							// ADC channel
#define ADC_CONVERSION_TIME				5.0							// Minimum time of conversion for 1 sample in µs (RC response at the entry of the ADC)
#define ADC_SAMPLING_FREQUENCY		128							// Sampling frequency in kHz

#define NB_SAMPLES_TOTAL					256							// Number of samples to acquire

#define DMA_NON_CIRCULAR					0								// "char circ" parameter of the Init_ADC1_DMA1 function


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
		GPIO_Write( GPIOB, 7, status );		// Toggle status LED
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
uint8_t sampleAcquisitionInit( void );

/********************************************************************************
	* acquireSample
	*
	*      Manual acquisition of one sample
	*				Blocking function.
	* 			
	* @param Void
	* @return The sampled value from the ADC
	*******************************************************************************/
uint16_t acquireSample ( void );

/********************************************************************************
	* acquireBurstManual
	*
	*      Manual acquisition of an array of samples.
	*				Blocking function.
	* 			
	* @param Void
	* @return A pointer on the filled array
	*******************************************************************************/
uint16_t * acquireBurstManual( void );

/********************************************************************************
	* acquireBurstDMA
	*
	*      Automatic acquisition of an array of samples (using DMA).
	*				Blocking function.
	* 			
	* @param Void
	* @return A pointer on the filled array
	*******************************************************************************/
uint16_t * acquireBurstDMA( void );


#endif					/* S_SERIALCOMM_HS_SAMPLEACQUISITION_H */
