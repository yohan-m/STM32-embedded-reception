STM32-embedded-reception
========================

git repo for the ground STM32 source code

List of folder contains is this repository :
	- Keil project : Reception software
	- Hardware project : Emission PCB


Detailed explanations for each folder :
	- Keil project : STM32-embedded-reception
		- Basic : Open the keil project (..\project\receiver_stm32.uvproj), build all the project, load the executable on the STM32
		- What it does : It receives and process ultrasounds signal		
		- How it works
			- Using the ADC, the STM gets the amplitude of a signal
			- With four different RIF, the STM can filter the signal following these frequencies : 39,5 kHz; 40,0 kHz ; 40,5 kHz ; 41,0 kHz  
			- LED PB5 On : The STM is acquiring a signal value from the ADC
			- LED PB6 On : Receive a 39,5 kHz signal
			- LED PB7 On : Receive a 40,0 kHz signal
			- LED PB8 On : Receive a 40,5 kHz signal
			- LED PB9 On : Receive a 41,0 kHz signal
			
	











