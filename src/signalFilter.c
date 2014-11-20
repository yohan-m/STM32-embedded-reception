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
	
int32_t squareHouseMade(int32_t numberToSquare, uint8_t maxIteration)
{
	uint8_t i;
	int32_t square = 0;
	int32_t preSquare = 0;
	
	for(i=0;i<maxIteration;i++)
	{
		square = (preSquare + numberToSquare/preSquare)/2;
		preSquare = square;
	}
	
	return square;
}
	
	
	

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
		
	GPIO_Write( GPIOB, 6, ON );
	
	int32_t preOutputFilterReel[4];
	int32_t preOutputFilterIm[4];
	
	int32_t signalWithoutMean[NB_SAMPLES_TOTAL];
	int32_t outputFilter[4];
	
	int32_t mean = 0;
	
	static uint8_t TDOA39500 = 0;
	static uint8_t TDOA40000 = 0;
	static uint8_t TDOA40500 = 0;
	static uint8_t TDOA41000 = 0;
	
	static uint8_t preTDOA39500 = 0;
	static uint8_t preTDOA40000 = 0;
	static uint8_t preTDOA40500 = 0;
	static uint8_t preTDOA41000 = 0;
	
	static uint8_t prepreTDOA39500 = 0;
	static uint8_t prepreTDOA40000 = 0;
	static uint8_t prepreTDOA40500 = 0;
	static uint8_t prepreTDOA41000 = 0;
	
	uint8_t TDOA[4];
	
	static uint8_t detectSignal = 0;
	
	static uint32_t timeArrivalBeacon1 = 0;
	static uint32_t timeArrivalBeacon2 = 0;
	static uint32_t timeArrivalBeacon3 = 0;
	static uint32_t timeArrivalBeacon4 = 0;
	
	static uint8_t dectectFirstSignal = 0;
	static uint8_t dectectSecondSignal = 0;
	static uint8_t dectectThirdSignal = 0;
	static uint8_t dectectFourthSignal = 0;
	
	static uint16_t recordFirstTDOA = 0;
	static uint16_t recordSecondTDOA = 0;
	static uint16_t recordThirdTDOA = 0;
	static uint16_t recordFourthTDOA = 0;
	
	static uint16_t record =0;
		
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
		mean += cos_39500[i];//signal[i];
	}
	mean = mean/(NB_SAMPLES_TOTAL);
	
	//Remove the mean of the input signal
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		signalWithoutMean[i] = (((int32_t)(cos_39500[i]/*signal[i]*/) - mean) << POSITION_VIRGULE);				
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
	
	for (i=0;i<4;i++)
	{
		if (preOutputFilterReel[i] < 0)
		{
			preOutputFilterReel[i] = -1*preOutputFilterReel[i];
		}
		if (preOutputFilterIm[i] < 0)
		{
			preOutputFilterIm[i] = -1*preOutputFilterIm[i];
		}
	}
	
	
	outputFilter[0] = ((preOutputFilterReel[0]*preOutputFilterReel[0]) >> POSITION_VIRGULE)+((preOutputFilterIm[0]*preOutputFilterIm[0]) >> POSITION_VIRGULE);
	outputFilter[1] = ((preOutputFilterReel[1]*preOutputFilterReel[1]) >> POSITION_VIRGULE)+((preOutputFilterIm[1]*preOutputFilterIm[1]) >> POSITION_VIRGULE);
	outputFilter[2] = ((preOutputFilterReel[2]*preOutputFilterReel[2]) >> POSITION_VIRGULE)+((preOutputFilterIm[2]*preOutputFilterIm[2]) >> POSITION_VIRGULE);
	outputFilter[3] = ((preOutputFilterReel[3]*preOutputFilterReel[3]) >> POSITION_VIRGULE)+((preOutputFilterIm[3]*preOutputFilterIm[3]) >> POSITION_VIRGULE);
	
	outputFilter[0] = squareHouseMade(outputFilter[0],20);
	outputFilter[1] = squareHouseMade(outputFilter[1],20);
	outputFilter[2] = squareHouseMade(outputFilter[2],20);
	outputFilter[3] = squareHouseMade(outputFilter[3],20);

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

	if ( /*prepreTDOA39500 == 0 && */ preTDOA39500 == 0 && TDOA39500 == 1)
	{
		TDOA[0] = 1;
		if (dectectFirstSignal == 0)
		{	
			recordFirstTDOA = detectSignal;
			dectectFirstSignal = 1;
		}
	}
		
	if ( /*prepreTDOA40000 == 0 && */ preTDOA40000 == 0 && TDOA40000 == 1)
	{
		TDOA[1] = 1;
		if (dectectSecondSignal == 0)
		{	
			recordSecondTDOA = detectSignal;
			dectectSecondSignal = 1;
		}
	}
	
	if ( /*prepreTDOA40500 == 0 &&*/ preTDOA40500 == 0 && TDOA40500 == 1)
	{
		TDOA[2] = 1;
		if (dectectThirdSignal == 0)
		{	
			recordThirdTDOA = detectSignal;
			dectectThirdSignal = 1;
		}
	}
	
	if (/*prepreTDOA41000 == 0 &&*/  preTDOA41000 == 0 && TDOA41000 == 1)
	{
		TDOA[3] = 1;
		if (dectectFourthSignal == 0)
		{	
			recordFourthTDOA = detectSignal;
			dectectFourthSignal = 1;
		}
	}
	
	prepreTDOA39500 = preTDOA39500;
	prepreTDOA40000 = preTDOA40000;
	prepreTDOA40500 = preTDOA40500;
	prepreTDOA41000 = preTDOA41000;
	
	preTDOA39500 = TDOA39500;
	preTDOA40000 = TDOA40000;
	preTDOA40500 = TDOA40500;
	preTDOA41000 = TDOA41000;
	
	// Il faudrait stocker dès qu'on valide un signal les arrivéés des autres signals
	// Une fois cette étape faite, il faudrait moyenner ce que l'on trouve sur 10 enregistrement
	// Donc dès qu'on arrive dans un état ou une TDOA est valide, on enregistre
	// On add le nombre de samples de difference pour les autres
	
	// Record the time of arrival
	
	if ( (dectectFirstSignal == 1) || (dectectSecondSignal == 1) || (dectectThirdSignal == 1) || (dectectFourthSignal == 1))
	{
		detectSignal ++;
	}
	
	if (detectSignal > 9)
	{
		if (dectectFirstSignal == 0)
		{
			recordFirstTDOA = detectSignal;
		}
		if (dectectSecondSignal == 0)
		{
			recordSecondTDOA = detectSignal;
		}
		if (dectectThirdSignal == 0)
		{
			recordThirdTDOA = detectSignal;
		}
		if (dectectFourthSignal == 0)
		{
			recordFourthTDOA = detectSignal;
		}
				
		detectSignal = 0;
		dectectFirstSignal = 0;
		dectectSecondSignal = 0;
		dectectThirdSignal = 0;
		dectectFourthSignal = 0;
				
		timeArrivalBeacon1 += recordFirstTDOA;
		timeArrivalBeacon2 += recordSecondTDOA;
		timeArrivalBeacon3 += recordThirdTDOA;
		timeArrivalBeacon4 += recordFourthTDOA;
		
		record ++;
		
		recordFirstTDOA = 0;
		recordSecondTDOA = 0; 
		recordThirdTDOA = 0;
		recordFourthTDOA = 0;
			
	}
	
	if (record > 9)
	{
		GPIO_Toggle( GPIOB, 5);
		//usbCommSendCoefficients(timeArrivalBeacon1,timeArrivalBeacon2,timeArrivalBeacon3,timeArrivalBeacon4 );
		timeArrivalBeacon1 = 0;
		timeArrivalBeacon2 = 0;
		timeArrivalBeacon3 = 0;
		timeArrivalBeacon4 = 0;
		record = 0;
		
	}
					
	//usbCommSendCoefficients((uint32_t) frequencyCoefficient[0],(uint32_t) frequencyCoefficient[1],(uint32_t) frequencyCoefficient[2],(uint32_t) frequencyCoefficient[3] );
	//usbCommSendCoefficients((uint32_t) outputFilter[0],(uint32_t) outputFilter[1],(uint32_t) outputFilter[2],(uint32_t) outputFilter[3] );
	//usbCommSendCoefficients((uint32_t) TDOA[0],(uint32_t) TDOA[1],(uint32_t) TDOA[2],(uint32_t) TDOA[3] );

	
	GPIO_Write( GPIOB, 6, OFF );
	return 0;
}
