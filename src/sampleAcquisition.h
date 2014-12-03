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

#define ADC1_DR_Address    	((uint32_t)0x4001244C)
#define TIM1_CCR1_Address  	((uint32_t)0x40012C34)

#define SIGNAL_BUFFER_SIZE	128
#define SIGNAL_HALF_BUFFER_SIZE 		(SIGNAL_BUFFER_SIZE/2)

extern uint16_t adcBuffer[SIGNAL_BUFFER_SIZE];		// Static buffer to save a signal
extern uint16_t idDataToProcess;

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
void setLEDAcquisition( uint8_t status );


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
void sampleAcquisitionInit( void );



#endif					/* S_SERIALCOMM_HS_SAMPLEACQUISITION_H */
