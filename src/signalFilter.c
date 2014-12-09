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

uint32_t outputFilters[NB_BEACONS];

uint32_t signalProcessing ()					
{
	// Count variable
	uint32_t i,j;
	
		
	// Signal Variable
	int32_t signalWithoutAverage[SIGNAL_HALF_BUFFER_SIZE];

	static uint16_t IdForCos = 0;
	static uint16_t CurrentBlock = 0;
	static uint16_t IdForCorrelation = 0;
	
	static int32_t sumBlockReal[NB_BEACONS][NB_BLOCKS];
	static int32_t sumBlockImg[NB_BEACONS][NB_BLOCKS];
		
	static uint32_t inputCorrelation[NB_BEACONS][NB_SAMPLE_CORRELATION];
	
	uint8_t readyToProcessTDOA = 0;
	uint8_t tentative = 0;
	
	uint64_t tempOuputCorrelation[NB_BEACONS];
	static uint32_t outputCorrelation[NB_BEACONS][NB_CORRELATIONS_FOR_TDOA];
	static uint32_t idOutputCorrelation = 0;
	uint32_t idMaxOfArrayCorrelation[NB_BEACONS];
	uint32_t tempMaximum = 0;
	uint32_t tempMinimum = 0;
	uint8_t idBeaconMinRSS = 0;

	uint32_t idClosestBeacon = 0;
	int32_t fuckingTDOA[NB_BEACONS]; // TDOA in numbers of samples (equivalent of 1/128 ms)
		
	int64_t tempResultReal[NB_BEACONS];
	int64_t tempResultImg[NB_BEACONS];
	
	uint64_t utempResultReal[NB_BEACONS];
	uint64_t utempResultImg[NB_BEACONS];
	
	
	uint8_t orderNotFound = 1;
	
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
		tempOuputCorrelation[j] = 0;
		fuckingTDOA[j] = 0;
	}
	
	//Get the recorded signal frome the DMA buffer
	for (i=0;i<SIGNAL_HALF_BUFFER_SIZE;i++)
	{
		signalWithoutAverage[i] = ((int32_t) adcBuffer[idDataToProcess + i] - (int32_t)AVERAGE_SIGNAL); // Signal 4.12 signé
			
		sumBlockReal[0][CurrentBlock] += signalWithoutAverage[i]*cos_39500[IdForCos + i];	
		sumBlockReal[1][CurrentBlock] += signalWithoutAverage[i]*cos_40000[IdForCos + i];		
		sumBlockReal[2][CurrentBlock] += signalWithoutAverage[i]*cos_40500[IdForCos + i];		
		sumBlockReal[3][CurrentBlock] += signalWithoutAverage[i]*cos_41000[IdForCos + i];	
	
		sumBlockImg[0][CurrentBlock] += signalWithoutAverage[i]*sin_39500[IdForCos + i];		
		sumBlockImg[1][CurrentBlock] += signalWithoutAverage[i]*sin_40000[IdForCos + i];		
		sumBlockImg[2][CurrentBlock] += signalWithoutAverage[i]*sin_40500[IdForCos + i];			
		sumBlockImg[3][CurrentBlock] += signalWithoutAverage[i]*sin_41000[IdForCos + i];	
	}			

	for (i=0;i<NB_BEACONS;i++)
	{
		for (j=0;j<NB_BLOCKS;j++)
		{
			tempResultReal[i] += (int64_t)sumBlockReal[i][j];
			tempResultImg[i] += (int64_t)sumBlockImg[i][j];
		}

		if (tempResultReal[i] < 0)
		{
			tempResultReal[i] = -1 * tempResultReal[i];
		}
		if (tempResultImg[i] < 0)
		{
			tempResultImg[i] = -1 * tempResultImg[i];
		}
		
		utempResultReal[i] = (uint64_t)tempResultReal[i];
		utempResultImg[i] = (uint64_t)tempResultImg[i];
		
		utempResultReal[i] *= utempResultReal[i];
		utempResultImg[i] *= utempResultImg[i];
		
		outputFilters[i] = (uint32_t)(utempResultReal[i] >> 32) + (uint32_t)(utempResultImg[i] >> 32);			
		
		inputCorrelation[i][IdForCorrelation] = outputFilters[i];
	}
	
	for (i=0;i<NB_BEACONS;i++) 
	{
		for (j=0;j< NB_SAMPLE_CORRELATION; j++) // Derives the correlation for the new point using 20ms
		{
			tempOuputCorrelation[i] += inputCorrelation[i][j];
		}
		// Saves the output of the correlation in an array of 40 ms
		outputCorrelation[i][idOutputCorrelation] = (uint32_t)(tempOuputCorrelation[i] >> 6);
	}
	
	//usbCommSendCoefficients(outputCorrelation[0][idOutputCorrelation],outputCorrelation[1][idOutputCorrelation],outputCorrelation[2][idOutputCorrelation],outputCorrelation[3][idOutputCorrelation] );	
	
	// Sioux way to increment the ids of the arrays and to detect when they are full
	if (++idOutputCorrelation >= NB_CORRELATIONS_FOR_TDOA)
	{
		idOutputCorrelation = 0; // Reset of the id to store the next 40 ms
		readyToProcessTDOA = 1;
	}
	if (++IdForCorrelation >= NB_SAMPLE_CORRELATION)
	{
		IdForCorrelation= 0;
	}
		
	if (1/*readyToProcessTDOA*/)
	{

		// Research the id of the maximum for each correlation
		for (i = 0; i < NB_BEACONS; i++)
		{
			tempMaximum = 0;
			for (j = 2; j < NB_CORRELATIONS_FOR_TDOA-2; j++)
			{
				if (outputCorrelation[i][j] > tempMaximum)
				{
					if  ((outputCorrelation[i][j-2] <= outputCorrelation[i][j-1])
						&& (outputCorrelation[i][j-1] <= outputCorrelation[i][j])
						&& (outputCorrelation[i][j]   >= outputCorrelation[i][j+1])
						&& (outputCorrelation[i][j+1] >= outputCorrelation[i][j+2]))
					{
						tempMaximum = outputCorrelation[i][j];
						idMaxOfArrayCorrelation[i] = j;
					}
				}
			} 
		}

		// Research the biggest maximum ie the closest beacon
		tempMaximum = 0;
		for (i = 0; i < NB_BEACONS; i++)
		{
			if (outputCorrelation[i][idMaxOfArrayCorrelation[i]] > tempMaximum)
			{
				tempMaximum = outputCorrelation[i][idMaxOfArrayCorrelation[i]];
				idClosestBeacon = i;
			}
		}

		//usbCommSendCoefficients( idClosestBeacon, 0,0,0 );	

		// Determines the TDOAs in number of samples (equivalent to 64/128 = 0.5ms time based unity)
		for (i = 0; i < NB_BEACONS; i++)
		{
			// The edge is after the reference and at a correct distance (< 17 ms)
			if ((idMaxOfArrayCorrelation[i] >= idMaxOfArrayCorrelation[idClosestBeacon])
				&& (idMaxOfArrayCorrelation[i] - idMaxOfArrayCorrelation[idClosestBeacon] < 17*2))
			{
				fuckingTDOA[i] = (int32_t) idMaxOfArrayCorrelation[i] - (int32_t) idMaxOfArrayCorrelation[idClosestBeacon] +17*2;
			}
			if ((idMaxOfArrayCorrelation[i] < idMaxOfArrayCorrelation[idClosestBeacon])
				&& (idMaxOfArrayCorrelation[idClosestBeacon] - idMaxOfArrayCorrelation[i] < 17*2))
			{
				fuckingTDOA[i] = (int32_t) idMaxOfArrayCorrelation[i] - (int32_t) idMaxOfArrayCorrelation[idClosestBeacon] +17*2;
			}
			/*else // Case of a beacon far from the drone
			{
				fuckingTDOA[i] = idMaxOfArrayCorrelation[i] + NB_CORRELATIONS_FOR_TDOA - idMaxOfArrayCorrelation[REF_BEACON];
			}*/
			
			// Check the possibility of that order
			/*if (fuckingTDOA[i] > 17*2) // Impossible, that means the beacon with the highest RSSI is not the closest
			{
				orderNotFound = 1;
				break;
			}*/		
		}
		
		/*if (orderNotFound)
		{
			for (j = idMaxOfArrayCorrelation[idClosestBeacon] + 17*2; j < NB_CORRELATIONS_FOR_TDOA; j++)
			{
				for (i = 0; i < NB_BEACONS; i++)

			}
		}*/
		
		// Mise à zéro de la valeur d'amplitude minimale
		tempMaximum = -1;
		for (i = 0; i < NB_BEACONS; i++)
		{
			if (outputCorrelation[i][idMaxOfArrayCorrelation[i]] < tempMaximum)
			{
				tempMaximum = outputCorrelation[i][idMaxOfArrayCorrelation[i]];
				idBeaconMinRSS = i;
			}
		}
		fuckingTDOA[idBeaconMinRSS] = 0;

		usbCommSendCoefficients(fuckingTDOA[0],fuckingTDOA[1],fuckingTDOA[2],fuckingTDOA[3] );	

	}
		
	GPIO_Clear( GPIOC, 12 );

	//Rajouter les LEDS pour un test réel
	
	return 0;			
		
}

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


