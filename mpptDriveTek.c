#include <stdio.h>
#include "mpptDriveTek.h"



void parsMppt( unsigned char *start, mpptData *sampleData)
{
unsigned short adcCount;


sampleData->status=start[0];

sampleData->voltageIn = adcCount  = ( ((unsigned short) (start[0] & msbForTenBit))) << 8 | start[1];
sampleData->voltageIn = ( (unsigned short) (((float) adcCount * 100 )/  VIN_driveTek_ADC_COUNT_TO_VOLTS )); 

sampleData->currentIn =( ((unsigned short)(start[2] & msbForTenBit)))  <<8 | start[3];
////////////////////////////////////////NOT

adcCount = ( ((unsigned short)(start[4] & msbForTenBit))) <<8 | start[5];
sampleData->voltageOut  = ( (unsigned short) (((float) adcCount * 100 )/  VOUT_driveTek_ADC_COUNT_TO_VOLTS )); 

sampleData->temp = start[6];
printf("\n\rthe inVoltage is %i ,and the out current is %i  and the outVoltage is %i\n\n",sampleData->voltageIn,sampleData->currentIn,sampleData->voltageOut);

//return sampleData;
return;
}
