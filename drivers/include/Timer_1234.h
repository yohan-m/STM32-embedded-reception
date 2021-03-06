//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//


#include "stm32f10x.h"
  
/******************************************************************************
* File Name          : TIMER_1234.h
* Author             : T Rocacher
* Version            : revu 7/09/2011
* Date               : 01/03/2011
* Description        : Module qui configure les TIMER 1,2,3 et 4
*                     
*******************************************************************************/



#ifndef _TIMER_1234_H__
#define _TIMER_1234_H__


//=======================================================================================
// Rappel sur les ressources 
//
//
//  3 Timer "general Purpose", TIM2, TIM3 et TIM4 + TIM1
//  Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
//  Mapping des IO:
//  	TIM1_CH1 - PA08       TM2_CH1_ETR - PA0 TM3_CH1 - PA6		TIM4_CH1 - PB6	
//    	TIM1_CH1 - PA09       TM2_CH2 - PA1		TM3_CH2 - PA7		TIM4_CH2 - PB7
// 		TIM1_CH1 - PA10       TM2_CH3 - PA2		TM3_CH3 - PB0		TIM4_CH3 - PB8
// 		TIM1_CH4 - PA11		  TM2_CH4 - PA3		TM3_CH4 - PB1		TIM4_CH4 - PB9
//
//=======================================================================================



//=======================================================================================
// Fonctions basiques des timers
//=======================================================================================

//______________________________________________________________________________

float Timer_1234_Init(TIM_TypeDef *Timer, float Duree_us );
//______________________________________________________________________________

/*
Timer : de TIM1 � TIM4
Duree_us : c'est l'intervalle de temps exprim� en �s entre deux d�bordements successifs
Frequence_Ck_Timer_Khz : la fr�quence qui attaque le prescaler du Timer
Renvoie la dur�e v�ritable, au plus proche de la valeur souhait�e
Le prescaler est calcul� le plus petit possible pour une pr�cision la plus 
fine possible). Le Timer est lanc� en mode down.
NB: Lancer avant la fonction d'initialis�tion du syst�me d'hrologe. 
Elle permet de r�gler toutes les horloges par d�faut.

Exemple :
Init_Clock_System(); // lance l'initialis�tion du syst�me d'horloge de la puce
Freq_Reel_Khz = 1000.0 / (Timer_1234_Init(TIM2, 10.0 ));   // Lance le timer avec
une p�riodicit� de 10.0 uS. La fr�quence correspondante est donc en principe 100 kHz.
Comme la fonction renvoie la valeur r�elle en uS, l'inverse donne un r�sultat en MHz.
D'o� la multiplication flottante par 1000.0


*/
//______________________________________________________________________________






//______________________________________________________________________________

void Active_IT_Debordement_Timer( TIM_TypeDef *Timer, char Prio, void (*IT_function) (void));
//______________________________________________________________________________

/*
La fonction initialise le p�riph�rique et le NVIC de mani�re � g�n�rer une interruption 
� chaque d�bordement du timer pr�cis�.
Le handler est �crit dans la lib (non accessible). Il op�re un test sur les flags 
(sauf pour TIM1, vectorisation multiple) pour identifier la source d'IT (d�b, capure...),
 puis lance la fonction pr�cis�e en param�tre (pointeur de fonction).
Avant de lancer la fonction, le handler rabaisse le flag d'IT pour ne pas 
rerentrer immediatement. Aucune gestion de ce type n'est � faire dans la 
fonction �IT_function�

Exemple :
Active_IT_Debordement_Timer( TIM2, 1, IT_Timer2); // La fonction active une interruption 
lors du d�bordement du Timer 2. La fonction appel�e est IT_Timer2. La priorit� associ�e est	1 
*/

//______________________________________________________________________________
//============== MODIF TR 18 Avril 2012==================
void Active_IT_Compare_Timer( TIM_TypeDef *Timer, char Voie, char Prio, void (*IT_function) (void));
/*
M�me principe que la pr�c�dente. Permet de lancer une fonction d'IT sur comparaison
du compteur et du CCR (capture compare). Le CCR est donc � r�gler, gr�ce � la macro
d�j� existante : PWM_Valeur(Timer,Voie) */
//============== FIN MODIF TR 18 Avril 2012==================

//=======================================================================================
// Timers en PWM
//=======================================================================================


vu16 PWM_Init(TIM_TypeDef *Timer, char Voie, float Frequence_PWM_Khz);
/*
Cette fonction initialise la voie sp�cifi�e du timer sp�cifi� en PWM.
La fr�quence souhait�e est pass�e en param�tre.
La fonction renvoie un entier qui correspond � la r�solution de la PWM 
pour pouvoir ensuite r�gler les rapports cycliques (ARR+1)
3 Timer "general Purpose", TIM2, TIM3 et TIM4 + TIM1
Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
Mapping des IO, voir d�but du .h.

!!C'est au user de configurer la sortie correctement, Altenate ppull !
*/

void PWM_Complementaire_Timer1(char Voie);
/* petite fonction qui configure le timer suppos� d�j� en PWM
de mani�re � sortir �galement la PWM compl�mentaire sur l'un des 
3 channels ou c'est possible : 1, 2 et 3.
Ce sont donc les channel CH1N, CH2N et CH3N qui s'activent en opposition
de phase avec leur homoloque CH1, CH2, CH3.
NB : les dead time ne sont pas g�r�s, donc � priori � 0.
*/

#define PWM_Valeur(Timer,Voie) Timer->CCR##Voie
/*
Permet de fixer  la dur�e � l'�tat haut de la PWM, dont indirectement son rapport 
cyclique. La grandeur doit �tre comprise entre 0 et ARR. Ex:
Reso = PWM_Init (TIM3,2,25.0);
PWM_Valeur(TIM3,2) = Reso /4; // arrondi � gerer
*/

#define CNT(Timer) Timer->CNT
/*
Permet un acc�s direct au compteur du Timer sp�cifi� sans avoir � conna�tre 
les registres du STM32
*/
#define ARR(Timer) Timer->ARR
/*
Permet un acc�s direct � l'autoreload du Timer (modulo du timer) sp�cifi� sans 
avoir � conna�tre les registres du STM32
*/
#define Clear_Flag_Overflow(Timer) 	Timer->SR = ((Timer->SR)&~0x01)

//============== MODIF TR 18 Avril 2012==================
#define CptUp(Timer) Timer->CR1=Timer->CR1&~(1<<4)
#define CptDown(Timer) Timer->CR1=Timer->CR1|(1<<4)
//============== FIN MODIF TR 18 Avril 2012==================


//=======================================================================================
// Timers en mode incr�mental
//=======================================================================================

//  3 Timer "general Purpose", TIM2, TIM3 et TIM4
//  Chacun d'entre eux dispose de 4 voies de sorties num�rot�es de 1 � 4
//  Mapping des IO:
//  	TM2_CH1_ETR - PA0 	TM3_CH1 - PA6		TIM4_CH1 - PB6	
//    	TM2_CH2 - PA1		TM3_CH2 - PA7		TIM4_CH2 - PB7
//
//   Dans ce mode, le timerx utilise les 2 entr�es 1 et 2
//
char Timer_Inc_Init(TIM_TypeDef *Timer, char Resolution);

#define Reso_Demie_Per_Ch1 2
#define Reso_Demie_Per_Ch2 1
#define Reso_Quart_Per 3

/*
Fonction de configuration du timer en question en mode codeur incr�mental
Compte les fronts montant et descendant. Selon la configuration d�sir�e, 
la r�solution est de � p�riode du channel 1, ou  2, ou encore d' � de p�riode 
en comptant sur les deux voies.

!!C'est au user de configurer des canaux  en entr�e !!
*/
#define Reset_Timer(Timer) Timer->CNT=0
// remet � 0 le compteur 

#define Bloque_Timer(Timer) Timer->CR1=(Timer->CR1)&~(1<<0)
// bloque le timer (CEN=0)


#define Run_Timer(Timer) Timer->CR1=(Timer->CR1)|(1<<0)
// Lance timer (CEN=1)

//=======================================================================================
// Timers en mode capture
//=======================================================================================


vu16 Capture_Init(TIM_TypeDef *Timer, char Voie, float Largeur_Pulse_Max_us, vu16 Resolution, char Prio);
/*
Cette fonction d�termine le prescaler (le pas du compteur) n�cessaire au compteur 
consid�r�  pour mesurer une dur�e d'impulsion. On pr�cise le maximum de la dur�e
et la r�solution pr�cis�e 	correspond � cette dur�e. La fonction renvoie 
la r�solution effective, pour la dur�e max.
La voie est �galement pass�e en param�tre. La mesure repose sur une interruption 
d�clench�e sur front montant et sur front descendant. Le sens du front est invers� � 
chaque interruption. La fonction de lecture, ci-apr�s permet de renvoyer la dur�e.

!!C'est au user de configurer l'IO correspondant  � la voie du timer,  en entr�e !!
*/


vu16 Lire_Duree_Pulse(TIM_TypeDef *Timer,int Voie);
/*
Le r�le de la fonction est de retourner le comptage op�r� pendant l'impulsion. 
C'est un nombre sans dimension dont la conversion en seconde peut se faire � 
partir de la r�solution, et de la largeur d'impulsion maxi pass�s en param�tres 
de Capture_Init.
*/





#endif
