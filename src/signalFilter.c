//Include

//Include of the filter cosinus value 
/*#include "cos_39500"
#include "cos_40000"
#include "cos_40500"
#include "cos_41000"*/

#include "global.h"

int outputFilter[4] = {0,0,0,0};

int* signalTraitement (uint16_t * intputValue)
{
	int i=0;
	int a = 1.1234567e+06;
	
	for (i=0;i<NB_SAMPLES_TOTAL;i++)
	{
		a++;
		/*outputFilter[0] += intputValue[i]*cos_39500[i];
		outputFilter[1] += intputValue[i]*cos_40000[i];	
		outputFilter[2] += intputValue[i]*cos_40500[i];	
		outputFilter[3] += intputValue[i]*cos_41000[i];*/			
	}
	
	return outputFilter;
}
