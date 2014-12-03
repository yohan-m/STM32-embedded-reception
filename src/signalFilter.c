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
	*																The value is "1" if the frequencyv  is into the signal, "0" if not
	*																
  * 
	* @return 0 if working
	*******************************************************************************/

int32_t outputFilters[NB_BEACONS];

uint32_t signalProcessing ()					
{
	// Count variable
	uint16_t i,j;
		
	// Signal Variable
	int32_t signalWithoutAverage[SIGNAL_HALF_BUFFER_SIZE];

	static uint16_t IdForCos = 0;
	static uint16_t CurrentBlock = 0;
	
	static int32_t sumBlockReal[NB_BEACONS][NB_BLOCKS];
	static int32_t sumBlockImg[NB_BEACONS][NB_BLOCKS];
	
	static int32_t recordedTime[NB_BEACONS] = {0,0,0,0};
	static int32_t dectectedSignal[NB_BEACONS] = {0,0,0,0};
	static int32_t numberDetection[NB_BEACONS] = {0,0,0,0};
	static int32_t timeArrival[NB_BEACONS] = {0,0,0,0};
	
	static int32_t preOutputFilters[NB_BEACONS] = {0,0,0,0};
	
	static int32_t detectSignal = 0;
	static int32_t record = 0;
	
	int32_t tempResultReal[NB_BEACONS];
	int32_t tempResultImg[NB_BEACONS];
	
	int32_t frequencyThreshold[NB_BEACONS] = FREQUENCY_THRESHOLD;
	
	GPIO_Set( GPIOC, 12 );
	
	//Search the right block to do the signal processing
	CurrentBlock ++;
	if (CurrentBlock >= NB_BLOCKS)
	{
		CurrentBlock = 0;
	}

	//Determin the moment to start the cosinus
	IdForCos = CurrentBlock * SIGNAL_HALF_BUFFER_SIZE;
	
	//Reset the value of the block output filter
	for (j=0;j<NB_BEACONS;j++)
	{
		sumBlockReal[j][CurrentBlock] =0;
		sumBlockImg[j][CurrentBlock] =0;
		tempResultReal[j] =0;
		tempResultImg[j] =0 ;
	}
	
	//Get the recorded signal frome the DMA buffer
	for (i=0;i<SIGNAL_HALF_BUFFER_SIZE;i++)
	{
		signalWithoutAverage[i] = ((int32_t) adcBuffer[idDataToProcess + i] - (int32_t)AVERAGE_SIGNAL);
		//signalWithoutAverage[i] = (int32_t) ((cos_39500[IdForCos + i] /*+ cos_40000[IdForCos + i]*/)) - (int32_t)AVERAGE_SIGNAL;
		//signalWithoutAverage[i] = division64(signalWithoutAverage[i],MAX_SIGNAL,POSITION_VIRGULE);
		
		sumBlockReal[0][CurrentBlock] += (((signalWithoutAverage[i]*((cos_39500[IdForCos + i]))))) >> POSITION_VIRGULE;		
		sumBlockReal[1][CurrentBlock] += (((signalWithoutAverage[i]*((cos_40000[IdForCos + i]))))) >> POSITION_VIRGULE;		
		sumBlockReal[2][CurrentBlock] += (((signalWithoutAverage[i]*((cos_40500[IdForCos + i]))))) >> POSITION_VIRGULE;		
		sumBlockReal[3][CurrentBlock] += (((signalWithoutAverage[i]*((cos_41000[IdForCos + i]))))) >> POSITION_VIRGULE;	
	
		sumBlockImg[0][CurrentBlock] += (((signalWithoutAverage[i]*((sin_39500[IdForCos + i]))))) >> POSITION_VIRGULE;		
		sumBlockImg[1][CurrentBlock] += (((signalWithoutAverage[i]*((sin_40000[IdForCos + i]))))) >> POSITION_VIRGULE;		
		sumBlockImg[2][CurrentBlock] += (((signalWithoutAverage[i]*((sin_40500[IdForCos + i]))))) >> POSITION_VIRGULE;			
		sumBlockImg[3][CurrentBlock] += (((signalWithoutAverage[i]*((sin_41000[IdForCos + i]))))) >> POSITION_VIRGULE;	
	}			

	for (i=0;i<NB_BEACONS;i++)
	{
		for (j=0;j<NB_BLOCKS;j++)
		{
			tempResultReal[i] += sumBlockReal[i][j] >> 4;
			tempResultImg[i] += sumBlockImg[i][j] >> 4;
		}

		tempResultReal[i] = tempResultReal[i] >> 2; //La puissance de deux de NB_BLOCKS
		tempResultImg[i] = tempResultImg[i] >> 2; //La puissance de deux de NB_BLOCKS

		tempResultReal[i] = tempResultReal[i]*tempResultReal[i];
		tempResultImg[i] = tempResultImg[i]*tempResultImg[i];

		outputFilters[i] = (tempResultReal[i] >> 0) + (tempResultImg[i] >> 0);			
		outputFilters[i] = squaredRootHomeMade(outputFilters[i],10);
		
	}

	// Threshold processing
	for(i=0;i<NB_BEACONS;i++)
	{
		if (outputFilters[i] > frequencyThreshold[i])
		{
			if (dectectedSignal[i] == 0)
			{	
				recordedTime[i] = detectSignal;//*SIGNAL_HALF_BUFFER_SIZE;
				dectectedSignal[i] = 1;
			}
		}
	preOutputFilters[i] = outputFilters[i]; // Pour la regression linéaire
	}
	
	// Increase the arrival time of signals
	for (i=0;i<NB_BEACONS;i++)
	{
		if (dectectedSignal[i] == 1)
		{
			detectSignal ++;
			break;
		}
	}
	
	if (detectSignal > EMISSION_PERIOD_SAMPLE)
	{
		for(i=0;i<NB_BEACONS;i++)
		{
			if(dectectedSignal[i] == 1)
			{
				numberDetection[i] ++;
			}
			
			detectSignal = 0;
			dectectedSignal[i] = 0;
			
			timeArrival[i] += recordedTime[i];
			
			record ++;
			recordedTime[i] = 0;
		}
	}
	
	if (record > TDOA_PERIOD_SAMPLE)
	{
		for(i=0;i<NB_BEACONS;i++)
		{
			if (numberDetection[i] != 0)
			{
				timeArrival[i] /= numberDetection[i];
			}
			else
			{
				timeArrival[i] = 20;//EMISSION_PERIOD_SAMPLE*TDOA_PERIOD_SAMPLE;
			}
		}	
		usbCommSendCoefficients((uint32_t)timeArrival[0],(uint32_t)timeArrival[1],(uint32_t)timeArrival[2],(uint32_t)timeArrival[3] );
			
		for(i=0;i<NB_BEACONS;i++)
		{
			timeArrival[i] = 0;
			numberDetection[i] = 0;
		}
		record = 0;
	}
	
	
	//usbCommSendCoefficients((uint32_t) outputFilters[0],(uint32_t) outputFilters[1],(uint32_t) outputFilters[2],(uint32_t) outputFilters[3] );	

	GPIO_Clear( GPIOC, 12 );

	//Rajouter les LEDS pour un test réel
	
	return 0;			
		
} //0.60 ms , total = 1.40 ms !! Trop long ==> passage en 32 bits obligé 

/********************************************************************************
	* squaredRootHomeMade
	*******************************************************************************/

int32_t squaredRootHomeMade(int32_t numberToSquare, uint16_t maxIteration)
{
	uint16_t i;
	int32_t square = 0;
	int32_t preSquare = 2;
	
	for(i=0;i<maxIteration;i++)
	{
		square = (preSquare + ((int32_t)numberToSquare)/preSquare)/2.0;
		preSquare = square;
	}
	
	return (int32_t)square;
}

