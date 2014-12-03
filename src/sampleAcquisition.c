/**
	* @file sampleAcquisition.c
	* @brief ADC acquisition of an ultrasound sample
	*
	*     This file contains the service for acquire sample from the ADC.
	*			Initialization, configuration and start. The status LED turn on at each acquisition.
	*			
	* 		Last modification : 07 Nov 2014
	*
	* @author Miquèl RAYNAL
	* @version 0.1
	* @date 29 Oct 2014
	*/


/******************************************************************************
	* 
	*   INCLUDED FILES
	*
	*****************************************************************************/
	
	
#include <stdint.h>
#include "misc.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "sampleAcquisition.h"
#include "global.h"



/******************************************************************************
	* 
	*   VARIABLES
	*
	*****************************************************************************/

uint16_t adcBuffer[SIGNAL_BUFFER_SIZE];		// Static buffer to save a signal
uint16_t idDataToProcess = 0;


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
void setLEDAcquisition( uint8_t status )
{
		GPIO_Write( GPIOB, 5, status );		// Toggle status LED
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{
	// Defines the ADC clock divider, this clock is derived from the APB2 clock (PCLK2)
	RCC_ADCCLKConfig( RCC_PCLK2_Div6 );
	// Enable DMA1 clock
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1 , ENABLE );
	// Enable GPIOC, ADC1 and TIM1 clock
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1 | RCC_APB2Periph_TIM1 , ENABLE );
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit( &GPIO_InitStructure );
	
	// Configure TIM1_CH1 (PA8) as alternate function push-pull
  GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure PC.00 (ADC Channel 10) as analog input
  GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// First LED on the board (B.05)
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = 	GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval None
  */
void DMA_Configuration(void)
{
	DMA_InitTypeDef DMA_InitStructure; // Structure to initialize the DMA

	// Configure DMA1 on channel 1
	DMA_InitStructure.DMA_PeripheralBaseAddr = 	ADC1_DR_Address;//ADC1_DR_Address; // Address of peripheral the DMA must map to
	DMA_InitStructure.DMA_MemoryBaseAddr = 			(uint32_t) &adcBuffer; // Variable to which ADC values will be stored
	DMA_InitStructure.DMA_DIR = 								DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 					SIGNAL_BUFFER_SIZE; // In data unit (see periph and memory data size fields)
	DMA_InitStructure.DMA_PeripheralInc = 			DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = 					DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = 	DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = 			DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = 								DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = 						DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = 								DMA_M2M_Disable; // Memory to memory

	//Initialise and enable DMA1
	DMA_DeInit( DMA1_Channel1 ); //Set DMA registers to default values
	DMA_Init( DMA1_Channel1, &DMA_InitStructure );
	DMA_Cmd( DMA1_Channel1, ENABLE );
}

/**
  * @brief  Configures the ADC.
  * @param  None
  * @retval None
  */
void ADC_Configuration(void)
{
	ADC_InitTypeDef ADC_InitStructure; // Structure to initialize the ADC

	// Configure ADC1 on channel 1
	ADC_InitStructure.ADC_Mode = 								ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = 				DISABLE; // One channel only
	ADC_InitStructure.ADC_ContinuousConvMode = 	DISABLE; // Conversion on PWM rising edge only
	ADC_InitStructure.ADC_ExternalTrigConv = 		ADC_ExternalTrigConv_T1_CC1; // Timer 1 CC1
	ADC_InitStructure.ADC_DataAlign = 					ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 				1;

	// Initialise and enable ADC1
	ADC_DeInit( ADC1 ); //Set ADC registers to default values
	ADC_Init( ADC1, &ADC_InitStructure ); 
	ADC_RegularChannelConfig( ADC1, ADC_Channel_10, 1, ADC_SampleTime_71Cycles5);
	
	// Start transferts
  ADC_ExternalTrigConvCmd( ADC1, ENABLE ); // Enable ADC1 external trigger
	ADC_DMACmd( ADC1, ENABLE ); //Enable ADC1 DMA
	ADC_Cmd( ADC1, ENABLE ); //Enable ADC1

  // Enable JEOC interrupt
  //ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

	// Calibrate ADC1
	ADC_ResetCalibration( ADC1 );
	while ( ADC_GetResetCalibrationStatus(ADC1) ) {} //Check the end of ADC1 reset calibration register
	ADC_StartCalibration( ADC1 );
	while ( ADC_GetCalibrationStatus(ADC1) ) {} //Check the end of ADC1 calibration
}

/**
  * @brief  Configures the Timer.
  * @param  None
  * @retval None
  */
void TIMER_Configuration(void)
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure; // Timer base configuration
	TIM_OCInitTypeDef         TIM_OCInitStructure; // Timer for PWM mode
	
	// Time Base configuration
  TIM_TimeBaseStructInit( &TIM_TimeBaseStructure ); 
  TIM_TimeBaseStructure.TIM_Period = 				0x231;          
  TIM_TimeBaseStructure.TIM_Prescaler = 		0x0;       
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
  TIM_TimeBaseStructure.TIM_CounterMode = 	TIM_CounterMode_Down;  
  TIM_TimeBaseInit( TIM1, &TIM_TimeBaseStructure );
	
  // TIM1 channel1 configuration in PWM mode
  TIM_OCInitStructure.TIM_OCMode = 			TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
  TIM_OCInitStructure.TIM_OCPolarity = 	TIM_OCPolarity_High;         
  TIM_OC1Init( TIM1, &TIM_OCInitStructure );
	
	// TIM1 counter enable
  TIM_Cmd( TIM1, ENABLE );
  // TIM1 main Output Enable
  TIM_CtrlPWMOutputs( TIM1, ENABLE );
}
	
/**
  * @brief  Configures the Timer.
  * @param  None
  * @retval None
  */
void IT_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; // IT

  // Configure and enable ADC interrupt
  NVIC_InitStructure.NVIC_IRQChannel = 										ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 	5;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 				2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = 								ENABLE;
  NVIC_Init( &NVIC_InitStructure );

	// Enable the DMA global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = 										DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 	5;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 				2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = 								ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	// DMA IT
	DMA_ITConfig( DMA1_Channel1, DMA1_FLAG_HT1, ENABLE ); // Half transfer interrupt
	DMA_ITConfig( DMA1_Channel1, DMA1_FLAG_TC1, ENABLE ); // Transfer complete interrupt
}

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
/**
 * @brief Interrupt handler of DCMI DMA stream
 */
void DMAChannel1_IRQHandler( void )
{
	
	if ( DMA_GetITStatus( DMA1_IT_HT1 ) != RESET ) // Half buffer
	{
		DMA_ClearITPendingBit( DMA1_IT_HT1 );
		
		setLEDAcquisition( ON );
		
		idDataToProcess = 0;
	}
	else if ( DMA_GetITStatus( DMA1_IT_TC1 ) != RESET ) // Full buffer
	{
		DMA_ClearITPendingBit( DMA1_IT_TC1 );
		
		setLEDAcquisition( OFF );
		
		idDataToProcess = SIGNAL_HALF_BUFFER_SIZE;
	}
	
	signalProcessing();
	
}

/********************************************************************************
	* sampleAcquisitionInit
	*
	*      Init the sampling routine.
	*				Blocking function.
	* 			
	* @param Void
	* @return 0 if working
	*******************************************************************************/
void sampleAcquisitionInit( void )
{
	/*****************
	 * SYSTEM CLOCKS *
	 *****************/
	
	RCC_Configuration();
	
	/********
	 * GPIO *
	 ********/

	GPIO_Configuration();
	
	/*******
	 * DMA *
	 *******/

	DMA_Configuration();
	
	/*******
	 * ADC *
	 *******/
	
	ADC_Configuration();

	/*********
	 * TIMER *
	 *********/
	
	TIMER_Configuration();
	
	/********
	 * NVIC *
	 ********/
	
	IT_Configuration();
	
	/*********
	 * START *
	 *********/

}

