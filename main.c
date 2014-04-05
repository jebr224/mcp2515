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
#include "bps.h"
#include <p18f4480.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>
#include <spi.h>


unsigned char g_messageDebug = 1;
unsigned char g_debug = 1;



//defs
//#define bpsDevice = 0;
//#define mpptDevice = 1;
//#define  deviceMC = 2;

unsigned char bpsDevice;
unsigned char mpptDevice;
unsigned char deviceMC;

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


/*
void listenForBPS(void)
{
  
		char bpsDebug = 0x01;

		stCanFrame result;
		unsigned char bpsDevice = 0;
		unsigned int  pollCount = 100;
    	unsigned int BPSCount = 0, totalBPS = 31;
       	while (BPSCount < totalBPS)
		{
			if (bpsDebug){ printf("\r   loopin fur bps no,%i, addr H%x \n",BPSCount,BPSAnswerBaseAddr + 1 + BPSCount * 4);}
       		pollCount = 10;
      		result.id = 0;
			while(!(result.id ==  BPSAnswerBaseAddr + 1 + (BPSCount * 4) && pollCount))
			{
				mcp2515_get_message(&result ,bpsDevice);
				pollCount--;
			}
			if(result.id ==   BPSAnswerBaseAddr + 1 + BPSCount * 4)
			{
				printf("\r   BPS number checks in no %i  addr H%x \n",BPSCount , BPSAnswerBaseAddr + 1 + BPSCount * 4);
			}
			else
			{
				printf("\r   BPS TIMEOUT number %i \n",BPSAnswerBaseAddr + 1 + BPSCount * 4);
			}
			BPSCount++;
		}   
}
*/

void promptBPSBox1(void)
{
  
		char bpsDebug = 0x00;
		unsigned int * prtVoltage;
		stCanFrame result;
	//	unsigned char bpsDevice = 0;
		unsigned int  pollCount = 10;
    	unsigned int BPSCount = 0, totalBPS = 20;
	//	unsigned char voltage[32];
       	while (BPSCount < totalBPS)
		{
			if (bpsDebug){ printf("\r   loopin fur bps no,%i, addr H%x \n",BPSCount,BPSAnswerBaseAddr + 1 + BPSCount * 4);}
       		pollCount = 10;
      		result.id = 0;
				
            //(0b00100000000 + (BPSCount * 4))
			result.id = (0b00100000000 + (BPSCount * 4)+1);
			result.length = 0;
			mcp2515_send_message(&result, 0x02,  bpsDevice);
		
			while((result.id !=  BPSAnswerBaseAddr + 1 + (BPSCount * 4) && (pollCount > 0)))
			{
				
				mcp2515_get_message(&result ,bpsDevice);
				 pollCount--;
			}
			;
			if(result.id ==   BPSAnswerBaseAddr + 1 + BPSCount * 4)
			{
				if (bpsDebug){printf("\r   BPS number checks in no %i  addr H%x \n",BPSCount , BPSAnswerBaseAddr + 1 + BPSCount * 4);}
				prtVoltage = (void *) result.data;
				//voltage[BPSCount] = *prtVoltage;
				printf("\rV[0%.2d]=%u  \n",BPSCount,*prtVoltage  );
			}
			else
			{
				printf("\r   BPS TIMEOUT number 0 %i \n",BPSAnswerBaseAddr + 1 + BPSCount * 4);
			}
			BPSCount++;
		}   
}


void promptBPSBox2(void)  //never tested
{
	unsigned char pollCount =100; //total guess
	unsigned int BPSCount = 0 , totalBPS = 20; 
	unsigned int *prtVoltage = 2222;

	char messageReceived = 0;
	unsigned char dataReceived[8];	//maximum length that can be recieved
	unsigned char lengthReceived, flagsReceived;
	unsigned long addressReceived;


    while(BPSCount < totalBPS)
	{
     //USE build in pic18f4480 hardware to get can messages

		messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
		while((addressReceived!= BPSAnswerBaseAddr + 1 + (BPSCount *4) &&  pollCount))
   		{
			//Check dropbox for the code that will do this
			messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
			pollCount--;
	
		}
        if(addressReceived == BPSAnswerBaseAddr + 1 + (BPSCount *4))
		{
			prtVoltage = (void*)dataReceived; 
			printf("\rV[1%.2d]=%u  \n",BPSCount,*prtVoltage  );

		}
		else {
			printf("\r   BPS TIMEOUT number 1 %i \n",BPSAnswerBaseAddr + 1 + BPSCount * 4);
		}

		BPSCount++;

	}
	return;

}

void promptMPPT()
{

    	mpptData myMpptData[4];
		mcData myMCData;
		stCanFrame sample;
		stCanFrame result;
		unsigned char counter = 0, temp;
		int pollCount = 10;  //NOT the only place pollCount is set!
            
		
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
			pollCount = 10;
			sample.id =   MPPTRequestID + 1 + (counter * 2);
			
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
			if(result.id == MPPTAnswerID + 1 + (counter * 2)) 
			{
				//int voltage = parsMppt((result.data),myMpptData[counter]);
			    if(g_messageDebug) 
			    {
			        printf("\rMessage from the MPPT  \n");
				   //printCanMessage(result);
		 	    }

			}
			else
			{
				if(g_debug) printf("\r mppt TimeOUT, id 0x%x \n",  MPPTAnswerID + 1 + (counter * 2) );
			}
	
			counter++;
		}
		

}

void listenForMC()
{
        stCanFrame result;
		unsigned char pollCount = 20;
	
		result.id = 0;
	   	mcp2515_get_message(&result ,deviceMC);
		while( !(result.id == 0x040b) && pollCount)
	   	{
			mcp2515_get_message(&result ,deviceMC)	;
			pollCount--;
		}
		if (result.id = 0x040b)
	 	{
		   // int cool = convertToInt(result.data + 4 , 100 );
		    //printf("\rThe temp is %i\n",cool);
		    printCanMessage(result);
			//parsMC((result.data), result.id, myMCData);
		}
		else
		{
			printf("\r MC TimeOUT \n");
		}
		

}

void main()
{

  int test;
  unsigned char del;



 // unsigned char bpsDevice = 0x00;
  //unsigned char mpptDevice = 0x01;
  //unsigned char deviceMC = 0x02;


  unsigned char mpptCanConfig[4];
  unsigned char mcCanConfig[4];
  //unsigned char bpsCanConfig[4];

  unsigned short filterID = mcBaseAddress | motorTempMes  ;

  bpsDevice = 0;
  mpptDevice = 1;
  deviceMC = 2;

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
    

    //Set up MPPT canbus
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
    
    //set up up Motor controller can bus
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

	//set up bps can bus
    setBPS(mcCanConfig);
    test = mcp_init(bpsDevice, mpptCanConfig);		
	while ( test != 0)
	{
		printf("\rMPPT. Failer to init MCP 2515, reseting \n");
		test = mcp_init(bpsDevice, mpptCanConfig);
	}
	mcp2515_normal(bpsDevice);
	printf("\rBPS. Pass init MCP 2515\n");


    

		
    //Ending set up

    //starting main loop
	//------------------
    //getMessagesThatLookLike(&(filterID), deviceMC);
	while(1)
	{	
     	printf("\r#main loop\n");

	//listenForBPS();
     listenForMC();
	 promptMPPT();
	 promptBPSBox2();
	 promptBPSBox1();

	
	}
}
