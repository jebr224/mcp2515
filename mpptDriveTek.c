//#include <stdio.h>
//#include "mpptDriveTek.h"
/*
unsigned int decodeValueMPPT(int* place)
{
   unsigned int  value;
   value = (*(place) & 0x3fff);
   return value;
}
*/

//unsigned int decodeValueMPPT(int* place)
//{
 // unsigned char *start =(unsigned char*) place;
  //return (((unsigned short) (start[0] & msbForTenBit))) << 8 | start[1];

//}
/*
void parsMppt( unsigned char  *start, mpptData *sampleData)
{


	unsigned short adcCount;
	//int voltageIn;
	adcCount = (((unsigned short) (start[0] & msbForTenBit))) << 8 | start[1];
	//printf("\r 1 = %i,   1 = %i\n",adcCount, ans);
}
*/


/*
unsigned short adcCount;


sampleData->status=start[0];

 adcCount  = ( ((unsigned short) (start[0] & msbForTenBit))) << 8 | start[1];
sampleData->voltageIn = ( (unsigned short) (((float) adcCount * 100 )/  VIN_driveTek_ADC_COUNT_TO_VOLTS )); 

sampleData->currentIn =( ((unsigned short)(start[2] & msbForTenBit)))  <<8 | start[3];
////////////////////////////////////////NOT

adcCount = ( ((unsigned short)(start[4] & msbForTenBit))) <<8 | start[5];
sampleData->voltageOut  = ( (unsigned short) (((float) adcCount * 100 )/  VOUT_driveTek_ADC_COUNT_TO_VOLTS )); 

sampleData->temp = start[6];
//printf("\n\rthe inVoltage is %i ,and the out current is %i  and the outVoltage is %i\n\n",sampleData->voltageIn,sampleData->currentIn,sampleData->voltageOut);

//return sampleData;
return;

*/
