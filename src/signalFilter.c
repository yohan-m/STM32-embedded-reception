/**
	* @file signalFilter.c
	* @brief Filtering a sum of four ultrasound signals
	*
	*     This file contains the application for filtering a signal. 
	*			
	* 		Last modification : 09 Nov 2014
	*
	* @author Guillaume Fauxpoint
	* @version 0.1
	* @date 29 Oct 2014
	*/


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/

#include "usbComm.h"
#include "signalFilter.h"
#include "exp_39000.h"
#include "exp_39500.h"
#include "exp_40000.h"
#include "exp_40500.h"
#include "exp_41000.h"

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

void signalTraitementInit()
{
	GPIO_Configure( GPIOB, 6, OUTPUT, OUTPUT_PPULL ); //LED 39,5 kHz
	GPIO_Configure( GPIOB, 7, OUTPUT, OUTPUT_PPULL );	//LED 40,0 kHz
	GPIO_Configure( GPIOB, 8, OUTPUT, OUTPUT_PPULL );	//LED 40,5 kHz
	GPIO_Configure( GPIOB, 9, OUTPUT, OUTPUT_PPULL );	//LED 41,0 kHz
}

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

int signalProcessing (uint16_t * signal,uint8_t frequencyCoefficient[])
{
	int i=0;
	
	int32_t preOutputFilterReel[4];
	int32_t preOutputFilterIm[4];
	
	int32_t signalWithoutMean[NB_SAMPLES_TOTAL];
	int32_t outputFilter[4];
	
	int32_t mean = 0;
	int32_t minSignal = 0;
	int32_t maxSignal = 4096;
	
	static uint8_t TDOA39500 = 0;
	static uint8_t TDOA40000 = 0;
	static uint8_t TDOA40500 = 0;
	static uint8_t TDOA41000 = 0;
	
	static uint8_t preTDOA39500 = 0;
	static uint8_t preTDOA40000 = 0;
	static uint8_t preTDOA40500 = 0;
	static uint8_t preTDOA41000 = 0;
	
	uint8_t TDOA[4];
	
	
	//Variable initialization
	for (i=0;i<4;i++)
	{
		preOutputFilterIm[i]= 0;
		preOutputFilterReel[i] = 0;
		frequencyCoefficient[i] = 0;
		TDOA[i]=0;
	}
	
	//Determine the mean, the minimum and the maximum of the input signal
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		mean += signal[i];
		if (signal[i] > maxSignal)
		{
			maxSignal = signal[i];
		}
		if (signal[i] < minSignal)
		{
			minSignal = signal[i];
		}
	}
	mean = mean/(NB_SAMPLES_TOTAL);
	
	//Format the signal to the 19.13 fixed point format
	if ( (maxSignal - mean) > (mean - minSignal))
	{
		//Remove the mean of the input signal
		for (i=0;i<NB_SAMPLES_TOTAL;i++)
		{
			signalWithoutMean[i] = (((int32_t)(signal[i]) - mean) << POSITION_VIRGULE)/(maxSignal - mean);				
		}
	}
	else
	{
		//Remove the mean of the input signal
		for (i=0;i<NB_SAMPLES_TOTAL;i++)
		{
			signalWithoutMean[i] = (((int32_t)(signal[i]) - mean) << POSITION_VIRGULE)/(minSignal - mean);					
		}
	}
	
		
	//Apply the filter on the input signal 
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		preOutputFilterReel[0] += ((signalWithoutMean[i]*(cos_39500[i])) >> POSITION_VIRGULE);
		preOutputFilterReel[1] += ((signalWithoutMean[i]*(cos_40000[i])) >> POSITION_VIRGULE);	
		preOutputFilterReel[2] += ((signalWithoutMean[i]*(cos_40500[i])) >> POSITION_VIRGULE);	
		preOutputFilterReel[3] += ((signalWithoutMean[i]*(cos_41000[i])) >> POSITION_VIRGULE);
		
		preOutputFilterIm[0] += ((signalWithoutMean[i]*(sin_39500[i])) >> POSITION_VIRGULE);
		preOutputFilterIm[1] += ((signalWithoutMean[i]*(sin_40000[i])) >> POSITION_VIRGULE);
		preOutputFilterIm[2] += ((signalWithoutMean[i]*(sin_40500[i])) >> POSITION_VIRGULE);	
		preOutputFilterIm[3] += ((signalWithoutMean[i]*(sin_41000[i])) >> POSITION_VIRGULE);
				
	}
	
	outputFilter[0] = ((preOutputFilterReel[0]*preOutputFilterReel[0]) >> POSITION_VIRGULE)+((preOutputFilterIm[0]*preOutputFilterIm[0]) >> POSITION_VIRGULE);
	outputFilter[1] = ((preOutputFilterReel[1]*preOutputFilterReel[1]) >> POSITION_VIRGULE)+((preOutputFilterIm[1]*preOutputFilterIm[1]) >> POSITION_VIRGULE);
	outputFilter[2] = ((preOutputFilterReel[2]*preOutputFilterReel[2]) >> POSITION_VIRGULE)+((preOutputFilterIm[2]*preOutputFilterIm[2]) >> POSITION_VIRGULE);
	outputFilter[3] = ((preOutputFilterReel[3]*preOutputFilterReel[3]) >> POSITION_VIRGULE)+((preOutputFilterIm[3]*preOutputFilterIm[3]) >> POSITION_VIRGULE);
	
	// Apply the threshold traitement on the filtered signal
	
	if (outputFilter[0] > FREQUENCY_THRESHOLD_39500)
	{
		frequencyCoefficient[0] = 1;
		GPIO_Write( GPIOB, 6, ON );
		TDOA39500 ++;	
	}
	else
	{
		GPIO_Write( GPIOB, 6, OFF );
		TDOA39500 = 0;
	}
	
	if (outputFilter[1] > FREQUENCY_THRESHOLD_40000)
	{
		frequencyCoefficient[1] = 1;
		GPIO_Write( GPIOB, 7, ON );
		TDOA40000 ++;	
	}
		else
	{
		GPIO_Write( GPIOB, 7, OFF );
		TDOA40000 = 0;	
	}
	
	if (outputFilter[2] > FREQUENCY_THRESHOLD_40500)
	{
		frequencyCoefficient[2] = 1;
		GPIO_Write( GPIOB, 8, ON );
		TDOA40500 ++;	
	}
	else
	{
		GPIO_Write( GPIOB, 8, OFF );
		TDOA40500 = 0;
	}
	
	if (outputFilter[3] > FREQUENCY_THRESHOLD_41000)
	{
		frequencyCoefficient[3] = 1;
		GPIO_Write( GPIOB, 9, ON );
		TDOA41000 ++;	
	}
	else
	{
		GPIO_Write( GPIOB, 9, OFF );
		TDOA41000 = 0;
	}
	
	
	// 
	
	if ( preTDOA39500 > 2 && TDOA39500 == 0)
	{
		TDOA[0] = 1;
	}
	
	if ( preTDOA40000 > 2 && TDOA40000 == 0)
	{
		TDOA[1] = 1;
	}
	
	if ( preTDOA40500 > 2 && TDOA40500 == 0)
	{
		TDOA[2] = 1;
	}
	
	if ( preTDOA41000 > 2 && TDOA41000 == 0)
	{
		TDOA[3] = 1;
	}
	
	preTDOA39500 = TDOA39500;
	preTDOA40000 = TDOA40000;
	preTDOA40500 = TDOA40500;
	preTDOA41000 = TDOA41000;
	
	
				
	//usbCommSendCoefficients((uint32_t) frequencyCoefficient[0],(uint32_t) frequencyCoefficient[1],(uint32_t) frequencyCoefficient[2],(uint32_t) frequencyCoefficient[3] );
	usbCommSendCoefficients((uint32_t) outputFilter[0],(uint32_t) outputFilter[1],(uint32_t) outputFilter[2],(uint32_t) outputFilter[3] );
	//usbCommSendCoefficients((uint32_t) TDOA[0],(uint32_t) TDOA[1],(uint32_t) TDOA[2],(uint32_t) TDOA[3] );

	
	
	return 0;
}
