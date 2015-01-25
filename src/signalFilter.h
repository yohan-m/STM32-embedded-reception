#ifndef _FILTER_H__
#define _FILTER_H__


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/

#include "global.h"
#include <math.h>
#include "sampleAcquisition.h"

/******************************************************************************
	* 
	*   CONSTANTS
	*
	*****************************************************************************/


#define NB_BLOCKS										(256/SIGNAL_HALF_BUFFER_SIZE)
#define AVERAGE_SIGNAL              2048

#define NB_SAMPLE_CORRELATION				(EMISSION_PERIOD*NB_BLOCKS/2) // Equivalent to 20ms 
#define NB_CORRELATIONS_FOR_TDOA		((EMISSION_PERIOD+EMISSION_OFF_PERIOD)*NB_BLOCKS/2) // Equivalent to 40ms

#define NB_BEACONS									4

#define EMISSION_PERIOD 						20 //ms
#define EMISSION_OFF_PERIOD	 				20

#define EMISSION_PERIOD_SAMPLE 			((EMISSION_PERIOD + 17)*(256/SIGNAL_HALF_BUFFER_SIZE))	

#define TDOA_PERIOD 								1000 //ms
#define TDOA_PERIOD_SAMPLE 					(TDOA_PERIOD/EMISSION_PERIOD_SAMPLE)


/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/

/********************************************************************************
	* signalProcessing
	*
	*  	Filtering a signal. 
	*		Allow to know if this signal is composed by signals with these following frequencies :
 	*		39,5 kHz; 40,0 kHz; 40,5 kHz; 41,0 kHz
	*
	*
	* @param signal Pointer on the array which contains the data of the signal
	* @param frequencyCoefficient[] Require a four size array. Each case represent a frequency.
	* 															The first is for 39,5 kHz
	*																The second for 40,0 kHz
	*																The third for 40,5 kHz 
	*																The last for 41,0 kHz
	*																The value is "1" if the frequency is into the signal, "0" if not
	*																
  * 
	* @return 0 if working
	*******************************************************************************/
uint32_t signalProcessing ();


#endif