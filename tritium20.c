#include <stdio.h>
#include "tritium20.h"

unsigned char parsMC(unsigned char *start, unsigned short id, mcData *outController)
{


   unsigned char pass = 0;
   printf("\n\rwow %x\n",id);
   if(id == (mcBaseAddress | velocityMes)) //0x0403
   {
 	  float *temp;
	  printf("\n\rin\n");
      temp = (float *) start;
      outController->speed   = *temp * wheelRadius;
      temp = (float *) start+4;
      outController->current = *temp;
      printf("\n\rthe Speed is is %i ,and the current is %i \n", outController->speed, outController->current);

	  pass = 1;
	
   }
return pass;
}