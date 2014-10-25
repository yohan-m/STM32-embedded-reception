//__________________________________________________________
// BINOME :
// ETAT : 
//__________________________________________________________

//____ registres de périphériques du stm_32
#include "stm32f10x.h"
#include "Clock.h"
#include "Timer_1234.h"
#include "GPIO.h"



void Clignote(void)		// Routine d'interruption
{
	GPIO_Toggle(GPIOC, 12);
}

int main (void)
{
	float Duree_Reelle;		
	CLOCK_Configure();		// Initialisation du système d'horloge
												//	- Si la clé de compilation STM32F107 est défini, le CPU est clocké à 50MHz
												//  - par défaut (STM32F107 non défini), le CPU est clocké à 40MHz


	// Configuration
	GPIO_Configure(GPIOC, 12, OUTPUT, OUTPUT_PPULL); // Led Stat
	GPIO_Clear(GPIOC, 12);
	
	Duree_Reelle = Timer_1234_Init(TIM2, 100000.0 );	// Configuration du Timer 2 pour un cycle de 20ms 
																										// la valeur est donnée en float, en µs
																										// Duree_Reelle contient la valeur exacte du cycle du 											//Timer, au plus proche de ce qui est demandé.

	Active_IT_Debordement_Timer( TIM2, 1, Clignote);  // Autorisation d'une interruption de priorité 1 
																										// sur le Timer 2. Spécification du nom de la fonction 											// devant être la routine d'interruption associée, ici, 											//fonction Clignote.
	
	
	// Action en tâche de fond 
	while(1)
	{}
		
	return 0;
}

	