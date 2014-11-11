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

#include "signalFilter.h"
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

int signalTraitement (uint16_t * signal,uint8_t frequencyCoefficient[])
{
	int i=0;
	
	float preOutputFilterReel[4];
	float preOutputFilterIm[4];
	
	float signalWithoutMean[NB_SAMPLES_TOTAL];
	float outputFilter[4];
	
	int32_t mean = 0;
	
	//Variable initialization
	for (i=0;i<4;i++)
	{
		preOutputFilterIm[i]= 0;
		preOutputFilterReel[i] = 0;
		frequencyCoefficient[i] = 0;
	}
	
	//Determine the mean of the input signal
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		mean += signal[i]/NB_SAMPLES_TOTAL;		
		//Test with the cosinus calculate on matlab
		///mean += cos_40000[i]/NB_SAMPLES_TOTAL;
	}
		
	//Remove the mean of the input signal
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		signalWithoutMean[i] = (float)(signal[i] - mean);			
		//signalWithoutMean[i] = (float)((sin_40000[i]+cos_39500[i]) - mean);			
	}
	
	//Apply the filter on the input filter
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		preOutputFilterReel[0] += (signalWithoutMean[i]*((float)cos_39500[i]));
		preOutputFilterReel[1] += (signalWithoutMean[i]*((float)cos_40000[i]));	
		preOutputFilterReel[2] += (signalWithoutMean[i]*((float)cos_40500[i]));	
		preOutputFilterReel[3] += (signalWithoutMean[i]*((float)cos_41000[i]));
		
		preOutputFilterIm[0] += signalWithoutMean[i]*((float)sin_39500[i]);
		preOutputFilterIm[1] += signalWithoutMean[i]*((float)sin_40000[i]);	
		preOutputFilterIm[2] += signalWithoutMean[i]*((float)sin_40500[i]);	
		preOutputFilterIm[3] += signalWithoutMean[i]*((float)sin_41000[i]);
				
	}
	
	outputFilter[0] = (sqrt(preOutputFilterReel[0]*preOutputFilterReel[0]+preOutputFilterIm[0]*preOutputFilterIm[0]));
	outputFilter[1] = (sqrt(preOutputFilterReel[1]*preOutputFilterReel[1]+preOutputFilterIm[1]*preOutputFilterIm[1]));
	outputFilter[2] = (sqrt(preOutputFilterReel[2]*preOutputFilterReel[2]+preOutputFilterIm[2]*preOutputFilterIm[2]));
	outputFilter[3] = (sqrt(preOutputFilterReel[3]*preOutputFilterReel[3]+preOutputFilterIm[3]*preOutputFilterIm[3]));
	
	// Apply the threshold traitement on the filtered signal
	
	if (outputFilter[0] > FREQUENCY_THRESHOLD_39500)
	{
		frequencyCoefficient[0] = 1;
		GPIO_Write( GPIOB, 6, ON );
	}
	else
	{
		GPIO_Write( GPIOB, 6, OFF );
	}
	
	if (outputFilter[1] > FREQUENCY_THRESHOLD_40000)
	{
		frequencyCoefficient[1] = 1;
		GPIO_Write( GPIOB, 7, ON );
	}
		else
	{
		GPIO_Write( GPIOB, 7, OFF );
	}
	
	if (outputFilter[2] > FREQUENCY_THRESHOLD_40500)
	{
		frequencyCoefficient[2] = 1;
		GPIO_Write( GPIOB, 8, ON );
	}
	else
	{
		GPIO_Write( GPIOB, 8, OFF );
	}
	
	if (outputFilter[3] > FREQUENCY_THRESHOLD_41000)
	{
		frequencyCoefficient[3] = 1;
		GPIO_Write( GPIOB, 9, ON );
	}
	else
	{
		GPIO_Write( GPIOB, 9, OFF );
	}
			
	return 0;
}
