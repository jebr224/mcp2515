//////////////////////////////////////////////////////////////////////
// MICRO CONFIGURATION
#pragma config OSC = HS     // Oscillator 
#pragma config PWRT = OFF   // Power on delay 
#pragma config WDT = OFF    // WatchDog Timer
#pragma config LVP = OFF    // Low Voltage Programming OFF




// INCLUDES

#include "mpptDriveTek.h"
#include "mcp2515.h"
#include "tritium20.h"
#include <p18f4480.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>
#include <spi.h>

unsigned char g_messageDebug = 1;
unsigned char g_debug = 1;



void printCanMessage(stCanFrame msg)
{
	int counter = 0;
    printf("\rThe address is %x \n",msg.id);
	while(counter < msg.length)
	{
		unsigned char temp = (msg.data[counter]);
		printf("\r  The data at %i  is  %x  \n",counter,temp);
		counter++;	
	}
}
void main()
{

  int test;
  unsigned char del;
  stCanFrame sample;
  unsigned char mpptDevice = 0x01;
  unsigned char deviceMC = 0x02;
  unsigned char mpptCanConfig[4];
  unsigned char mcCanConfig[4];

  unsigned short filterID = mcBaseAddress | motorTempMes  ;



  OpenUSART( USART_TX_INT_OFF &
  USART_RX_INT_OFF &
  USART_ASYNCH_MODE &
  USART_EIGHT_BIT &
  USART_CONT_RX &
  USART_BRGH_HIGH, 71);
  
   printf("\n\r Serial Open \n");

  OpenSPI( SPI_FOSC_16,   //Master clock 16mhz
  		MODE_00,                 
  		SMPEND);                  //Output data a end of buff

  



    selectNoDevice();

    setmppt(mpptCanConfig);

    test = mcp_init(mpptDevice, mpptCanConfig);		
	while ( test != 0)
	{
		printf("\rMPPT. Failer to init MCP 2515, reseting \n");
		test = mcp_init(mpptDevice, mpptCanConfig);
	}
	mcp2515_normal(mpptDevice);
	printf("\rMPPT. Pass init MCP 2515\n");

    delay();
       
    setmc(mcCanConfig);
    test = mcp_init(deviceMC,mcCanConfig);
    while(test !=0)
    {
		printf("\r MOTOR CONTROLLER.  Failer to init MCP 2515, reseting \n");
	    test = mcp_init(deviceMC,mcCanConfig);
    }
	mcp2515_normal(deviceMC);
	printf("\rMOTOR CONTROLLER. Pass init MCP 2515\n");
    delay();

	if (bit_is_set(0b10111111,6))
		printf("\r Error -simple bit shift fails \n");
	else	
		printf("\r simple bit operations work right \n");
		




		
    //Ending set up

    //starting main loop
	//------------------
    getMessagesThatLookLike(&(filterID), deviceMC);
	while(1)
	{	
    	mpptData myMpptData[4];
		mcData myMCData;
		stCanFrame sample;
		stCanFrame result;
		unsigned char counter = 0, temp;
		int pollCount = 10;
            
		
		//MPPT
        //---------------------
		//build the message prompts the MPPT data
		//The mppt address we want are. 
		// 0x711 , 0x713, 0x715, 0x717
		sample.id =   MPPTRequestID;
		sample.length = 0;
		sample.rtr =0;
		while(counter < 4)
		{
			sample.id =   MPPTRequestID + 1 + (counter * 2);
			if(g_debug) printf("\r the id is %x\n",MPPTRequestID + 1 + (counter * 2));
			mcp2515_send_message(&sample, 0x02,mpptDevice);		
			Delay10TCYx(0x30);
			result.id = 0;
			mcp2515_get_message(&result ,mpptDevice);			
	
			while(!(result.id == MPPTAnswerID + 1 + (counter * 2)) && pollCount ) //while we do not have the matching id, and the time has not expired keep waiting for the message
			{
		 		mcp2515_get_message(&result, mpptDevice);
		    	Delay10TCYx(0x30);
		 		pollCount--;
	
		 	}
			if(pollCount) 
			{
				parsMppt((result.data),myMpptData[counter]);
			    if(g_messageDebug) 
			    {
				    printf("\rMessage from the MPPT \n");
				    printCanMessage(result);
		 	    }

			}
			else
			{
				if(g_debug) printf("\r mppt TimeOUT \n");
			}
	
			

			counter++;
		}
		

		pollCount = 10;
        //MOTOR Controller
        //-------------------------------
        

   		Delay10TCYx(0x30);
		result.id = 0;
	   	mcp2515_get_message(&result ,deviceMC);
		while( !(result.id == 0x040b) && pollCount)
	   	{
			mcp2515_get_message(&result ,deviceMC)	;
		}
		if (result.id = 0x040b)
	 	{
		    int cool = convertToInt(result.data + 4 , 100 );
		    printf("\rThe temp is %i\n",cool);
		    printCanMessage(result);
			parsMC((result.data), result.id, myMCData);
		}
		else
		{
			printf("\r MC TimeOUT \n");
		}
				
		
	}
} 
			
