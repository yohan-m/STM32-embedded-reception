#ifndef _FILTER_H__
#define _FILTER_H__


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/

#include "global.h"
#include <math.h>

/******************************************************************************
	* 
	*   CONSTANTS
	*
	*****************************************************************************/

#define FREQUENCY_THRESHOLD_39500 100000.0			
#define FREQUENCY_THRESHOLD_40000 100000.0
#define FREQUENCY_THRESHOLD_40500 100000.0
#define FREQUENCY_THRESHOLD_41000 100000.0

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

int signalTraitement (uint16_t * signal,uint8_t frequencyCoefficient[]);

#endif