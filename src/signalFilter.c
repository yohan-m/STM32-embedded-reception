/**
	* @file signalFilter.c
	* @brief Filtering a sum of four ultrasound signals
	*
	*     This file contains the application for filtering a signal. 
	*			
	* 		Last modification : 25 Jan 2015
	*
	* @author Guillaume Fauxpoint & Miquèl Raynal
	* @version 0.1
	* @date 25 Jan 2015
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
	*																The value is "1" if the frequencyv  is into the signal, "0" if not
	*																
  * 
	* @return 0 if working
	*******************************************************************************/
uint32_t signalProcessing ()					
{
	// Count variable
	uint32_t i,j;
	
	uint32_t syncWindow = 0;
		
	static uint32_t outputFilters[NB_BEACONS] = {0, 0, 0, 0};
	static uint32_t outputCorrelation[NB_BEACONS][NB_CORRELATIONS_FOR_TDOA];
	static uint32_t idOutputCorrelation = 0;
	static uint16_t IdForCos = 0;
	static uint16_t CurrentBlock = 0;
	static uint16_t IdForCorrelation = 0;

	static int32_t sumBlockReal[NB_BEACONS][NB_BLOCKS];
	static int32_t sumBlockImg[NB_BEACONS][NB_BLOCKS];
		
	static uint32_t inputCorrelation[NB_BEACONS][NB_SAMPLE_CORRELATION];

	static uint32_t idMaxOfArrayCorrelation[NB_BEACONS] = {0, 0, 0, 0};
		
	// Signal Variable
	int32_t signalWithoutAverage[SIGNAL_HALF_BUFFER_SIZE];
	uint64_t tempOuputCorrelation[NB_BEACONS];
	
	uint32_t tempMaximum = 0;
	uint32_t tempMinimum = -1;
	uint32_t idBeaconWithLowRSS = 0; // Not really used finally
	uint32_t idClosestBeacon = 0;
	
	static int32_t fuckingTDOA[NB_BEACONS] = {0,0,0,0};
	static uint8_t readyToSend = 0;
		
	int64_t tempResultReal[NB_BEACONS];
	int64_t tempResultImg[NB_BEACONS];
	
	uint64_t utempResultReal[NB_BEACONS];
	uint64_t utempResultImg[NB_BEACONS];
	
	static int32_t tdoaSaved[NB_BEACONS][5] = {{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}};
	static uint8_t idTdoaSaved = 0;
			
	// A block is a portion of the window where the new signal has to be processed
	CurrentBlock ++;
	if (CurrentBlock >= NB_BLOCKS)
	{
		CurrentBlock = 0;
	}

	// Determins what part of the hard-saved cosinus has to be used to process the filter
	IdForCos = CurrentBlock * SIGNAL_HALF_BUFFER_SIZE;
	
	// Reset the value of some variables
	for (j = 0; j < NB_BEACONS; j++)
	{
		sumBlockReal[j][CurrentBlock] =0;
		sumBlockImg[j][CurrentBlock] =0;
		tempResultReal[j] =0;
		tempResultImg[j] =0 ;
		tempOuputCorrelation[j] = 0;
	}
	
	///////////////
	// FILTERING //
	///////////////
	
	// Get the recorded signal from the DMA buffer
	for (i = 0; i < SIGNAL_HALF_BUFFER_SIZE; i++)
	{
		// Remove the average (ADC is on 12 bits)
		signalWithoutAverage[i] = ((int32_t) adcBuffer[idDataToProcess + i] - (int32_t)AVERAGE_SIGNAL); // Signal 4.12 signé
			
		// Save the real and the imaginary part for each beacon on the current block
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
		// Add the value of the real and imaginary part of the previously derived blocks
		for (j=0;j<NB_BLOCKS;j++)
		{
			tempResultReal[i] += (int64_t)sumBlockReal[i][j];
			tempResultImg[i] += (int64_t)sumBlockImg[i][j];
		}

		// Compute the absolute value
		if (tempResultReal[i] < 0)
		{
			tempResultReal[i] = -1 * tempResultReal[i];
		}
		if (tempResultImg[i] < 0)
		{
			tempResultImg[i] = -1 * tempResultImg[i];
		}
		
		// Turn the results into unsigned numbers (winning one bit)
		utempResultReal[i] = (uint64_t)tempResultReal[i];
		utempResultImg[i] = (uint64_t)tempResultImg[i];
		
		// Derive the signals to the power 2
		utempResultReal[i] *= utempResultReal[i];
		utempResultImg[i] *= utempResultImg[i];
		
		// The output of the filter is the sum of the real and imaginary coefficients to the power 2
		outputFilters[i] = (uint32_t)(utempResultReal[i] >> 32) + (uint32_t)(utempResultImg[i] >> 32);			
		
		inputCorrelation[i][IdForCorrelation] = outputFilters[i];
	}
	
	/////////////////
	// CORRELATION //
	/////////////////
	
	for (i = 0; i < NB_BEACONS; i++) 
	{
		// Derives the correlation for the new point using 20ms
		for (j = 0; j < NB_SAMPLE_CORRELATION; j++) 
		{
			tempOuputCorrelation[i] += inputCorrelation[i][j];
		}
		// Saves the output of the correlation in an array of 40 ms
		outputCorrelation[i][idOutputCorrelation] = (uint32_t)(tempOuputCorrelation[i] >> 6);
	}
	
	// Possibility to debug at this part sending the output of the correlation
	//usbCommSendCoefficients(outputCorrelation[0][idOutputCorrelation],outputCorrelation[1][idOutputCorrelation],outputCorrelation[2][idOutputCorrelation],outputCorrelation[3][idOutputCorrelation] );	
	
	// Sioux way to increment the ids of the arrays and to detect when they are full
	if (++idOutputCorrelation >= NB_CORRELATIONS_FOR_TDOA)
	{
		idOutputCorrelation = 0; // Reset of the id to store the next 40 ms
		readyToSend = 1;
	}
	if (++IdForCorrelation >= NB_SAMPLE_CORRELATION)
	{
		IdForCorrelation= 0;
	}
	
	// Research the id of the maximum for each correlation (ie: the position where it is possible to compare the time of arrival)
	for (i = 0; i < NB_BEACONS; i++)
	{
		tempMaximum = 0;
		for (j = 2; j < NB_CORRELATIONS_FOR_TDOA-2; j++)
		{
			if (outputCorrelation[i][j] > tempMaximum)
			{
				if  ((outputCorrelation[i][j-2] <= outputCorrelation[i][j-1])
					&&(outputCorrelation[i][j-1] <= outputCorrelation[i][j])
					&& (outputCorrelation[i][j]   >= outputCorrelation[i][j+1])
					&& (outputCorrelation[i][j+1] >= outputCorrelation[i][j+2]))
				{
					tempMaximum = outputCorrelation[i][j];
					idMaxOfArrayCorrelation[i] = j;
				}
			}
		} 
	}

	// Research the biggest and the lowest maximum ie the closest and the farest beacon
	tempMaximum= 0;
	tempMinimum= -1;
	for (i = 0; i < NB_BEACONS; i++)
	{
		if (outputCorrelation[i][idMaxOfArrayCorrelation[i]] > tempMaximum)
		{
			tempMaximum = outputCorrelation[i][idMaxOfArrayCorrelation[i]];
			idClosestBeacon = i;
		}
		if (outputCorrelation[i][idMaxOfArrayCorrelation[i]] < tempMinimum)
		{
			tempMinimum = outputCorrelation[i][idMaxOfArrayCorrelation[i]];
			idBeaconWithLowRSS = i; // Just for record
		}
	}
	
	/////////////////////
	// TDOA DERIVATION //
	/////////////////////
	
	// Determines the TDOAs in number of samples (equivalent to 64/128 = 0.5ms time based unity)
	for (i = 0; i < NB_BEACONS; i++)
	{
		// The edge is after the reference and at a correct distance (< 17 ms)
		if ((idMaxOfArrayCorrelation[i] >= idMaxOfArrayCorrelation[idClosestBeacon])
			&& ((idMaxOfArrayCorrelation[i] - idMaxOfArrayCorrelation[idClosestBeacon]) < 17*2))
		{
			fuckingTDOA[i] = (int32_t) idMaxOfArrayCorrelation[i] - (int32_t) idMaxOfArrayCorrelation[idClosestBeacon] ;
			syncWindow ++;
		}
		if ((idMaxOfArrayCorrelation[i] < idMaxOfArrayCorrelation[idClosestBeacon])
			&& ((idMaxOfArrayCorrelation[idClosestBeacon] - idMaxOfArrayCorrelation[i]) < 17*2))
		{
			fuckingTDOA[i] = (int32_t) idMaxOfArrayCorrelation[i] - (int32_t) idMaxOfArrayCorrelation[idClosestBeacon];
			syncWindow++;
		}

		tdoaSaved[i][idTdoaSaved] = fuckingTDOA[i];
	}
	
	if (++idTdoaSaved == 5)
	{
		idTdoaSaved = 0;
	}
	
	// Need a stable signal (at least four consecutive same TDOA) to validate it (done several time on each window, do not interfere with the frequency of TDOA released)
	uint8_t ready = 1;
	for (i = 0; i < NB_BEACONS; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if(tdoaSaved[i][j] != tdoaSaved[i][j+1])
			{
				ready = 0;
				break;
			}
		}
	}
	
	if(ready && readyToSend)
	{
		readyToSend = 0;
		
		// Coefficients are for debug purpose (using the Python script) while times are for the drone.
		//usbCommSendCoefficients( fuckingTDOA[0]+34,fuckingTDOA[1]+34,fuckingTDOA[2]+34,fuckingTDOA[3]+34 );	
		usbCommSendTimes( fuckingTDOA[0],fuckingTDOA[1],fuckingTDOA[2],fuckingTDOA[3], outputCorrelation[0][idMaxOfArrayCorrelation[0]], outputCorrelation[1][idMaxOfArrayCorrelation[1]], outputCorrelation[2][idMaxOfArrayCorrelation[2]], outputCorrelation[3][idMaxOfArrayCorrelation[3]] );	
	}
	
	return 0;			
		
}
