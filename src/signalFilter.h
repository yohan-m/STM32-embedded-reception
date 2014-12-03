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

#define FREQUENCY_THRESHOLD					{1500,1500,1500,1500}
#define POSITION_VIRGULE						9

#define NB_BLOCKS										(256/SIGNAL_HALF_BUFFER_SIZE)
#define MAX_SIGNAL									1
#define AVERAGE_SIGNAL              1970

#define NB_BEACONS									4

#define EMISSION_PERIOD 						1 //ms
#define EMISSION_OFF_PERIOD	 				24

#define EMISSION_PERIOD_SAMPLE 			(EMISSION_PERIOD*(256/SIGNAL_HALF_BUFFER_SIZE))	

#define TDOA_PERIOD 								1000 //ms
#define TDOA_PERIOD_SAMPLE 					(TDOA_PERIOD/(EMISSION_OFF_PERIOD + EMISSION_PERIOD))

/******************************************************************************
	*
	*   PUBLIC FUNCTIONS
	*
	*****************************************************************************/

/********************************************************************************
	* signalTraitementInit
	*
	* Congifure the GPIO for the signal reception
	*
	*******************************************************************************/

void signalTraitementInit();

/********************************************************************************
	* signalTraitement
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

int32_t squaredRootHomeMade(int32_t numberToSquare, uint16_t maxIteration);


#endif