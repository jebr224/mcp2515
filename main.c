
#define VIN_driveTek  (6.5587)
#define CUR_driveTek   (1)
#define VOUT_driveTek  (4.7251)

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


//unsigned char g_messageDebug = 1;
//unsigned char g_debug = 1;



//defs
//#define bpsDevice = 0;
//#define mpptDevice = 1;
//#define  deviceMC = 2;

unsigned char bpsDevice_0;
unsigned char bpsDevice_1;
unsigned char mpptDevice;
unsigned char deviceMC;







void theDelay()
{
	volatile int y = 2000000;
	for(y;y<0;y--)
	{
		volatile unsigned char x = 254;
		for(x;x<0;x--)
		{
			
		}
	}
	return ;
}




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

void promptBPSBox(unsigned char boxNum)
{

		char bpsDebug = 1;//0x00;
		unsigned int * prtVoltage;
		stCanFrame result, promptMsg;
	//	unsigned char bpsDevice = 0;
		unsigned int  pollCount = 250;   //NOTE: not only place pollCount is set
    	unsigned int BPSCount = 0, totalBPS = 20;
	//	unsigned char voltage[32];


		unsigned char canNum =254;
		if(boxNum ==1)
		{
			canNum = bpsDevice_1;
		}
		else if ( boxNum == 0 )
		{
			canNum = bpsDevice_0;
		}
		else 
		{
			return ;  
		}




       	while (BPSCount < totalBPS)
		{
		//	if (bpsDebug){ printf("\r   looping fuor bps no,%i, addr H%x \n",BPSCount,BPSAnswerBaseAddr + 1 + BPSCount * 4);}
       		pollCount = 500;
      		result.id = 0;
				
            //(0b00100000000 + (BPSCount * 4))
			promptMsg.id = (0b00100000000 + (BPSCount * 4)+1);
			promptMsg.length = 0;
			mcp2515_send_message(&promptMsg, 0x02,  canNum);
			Delay10TCYx(0x30);
			
			while((result.id !=  BPSAnswerBaseAddr + 1 + (BPSCount * 4) && (pollCount > 0)))
			{
				mcp2515_send_message(&promptMsg, 0x02,  canNum);
				//Delay10TCYx(0x30);
				theDelay();
				mcp2515_get_message(&result ,canNum);
				 pollCount--;
			}
			
			if(result.id ==   BPSAnswerBaseAddr + 1 + BPSCount * 4)
			{
			//	if (bpsDebug){printf("\r   BPS number checks in no %i  addr H%x \n",BPSCount , BPSAnswerBaseAddr + 1 + BPSCount * 4);}
				prtVoltage = (void *) result.data;
				//voltage[BPSCount] = *prtVoltage;
				printf("\rV[%i][%.2d] = %u  \n",boxNum,BPSCount,*prtVoltage  );
			}
			else
			{
				printf("\r#   BPS TIMEOUT number %i %i \n", boxNum,BPSAnswerBaseAddr + 1 + BPSCount * 4);
			}
			BPSCount++;
		}   
}
/*

void promptBPSBox2(void)  //can not setting up correctly
{
	unsigned char pollCount =100; //total guess // better guess
	unsigned int BPSCount = 0 , totalBPS = 20; 
	unsigned int *prtVoltage = 2222;

	char messageReceived = 0;
	unsigned char dataReceived[8];	//maximum length that can be recieved
	unsigned char lengthReceived, flagsReceived;
	unsigned long addressReceived;
    unsigned long addressSent;

    while(BPSCount < totalBPS)
	{
     //USE build in pic18f4480 hardware to get can messages
		pollCount = 100; 
		addressSent = 0b00100000000 + 1 + (BPSCount *4);
		ECANSendMessage(addressSent, NULL, 0 ,0);//fuck the rules ECAN_TX_STD_FRAME | ECAN_TX_PRIORITY_0 | ECAN_TX_NO_RTR_FRAME);
		ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
		while((((int)addressReceived) != BPSAnswerBaseAddr + 1 + (BPSCount *4) &&  pollCount))
   		{
			//Check dropbox for the code that will do this
			ECANSendMessage(addressSent, NULL, 0 ,0);// yeah ECAN_TX_STD_FRAME | ECAN_TX_PRIORITY_0 | ECAN_TX_NO_RTR_FRAME);
		   	theDelay();
			ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
			pollCount--;
		//	printf("\r addr is %i-- len is %i -- the id we want is %i\n",((int)addressReceived), lengthReceived,BPSAnswerBaseAddr + 1 + (BPSCount *4) );
	
	
		}
        if(addressReceived == BPSAnswerBaseAddr + 1 + (BPSCount *4))
		{

			prtVoltage = (void*)dataReceived; 
		
			printf("\rv[1][%.2d] = %u  \n",BPSCount,*prtVoltage);
      //	printf("\rv[0][%.2d] = %u  \n",BPSCount,*prtVoltage  );
		}
		else 
		{
			printf("\r   BPS TIMEOUT number 1 %i \n",BPSAnswerBaseAddr + 1 + BPSCount * 4);
		}

		BPSCount++;

	}
	return;

}
*/
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
			theDelay();
			result.id = 0;
			mcp2515_get_message(&result ,mpptDevice);			
	
			while(!(result.id == MPPTAnswerID + 1 + (counter * 2)) && pollCount ) //while we do not have the matching id, and the time has not expired keep waiting for the message
			{
		 		mcp2515_get_message(&result, mpptDevice);
		    	theDelay();
		 		pollCount--;
	
		 	}
			if(result.id == MPPTAnswerID + 1 + (counter * 2)) 
			{
				unsigned short voltageIn  =0 , voltageOut =0, current=0,  adcCount ;
				unsigned char mpptStatus = 0;
				mpptStatus = (result.data[0] & 0b11110000);//>> 4;
				adcCount = ( ((unsigned short) (result.data[0] & 0b00000011 ))) << 8 |result.data[1];
				voltageIn  = ( (unsigned short) (((float) adcCount * 100 )/  VIN_driveTek ));
				current    = (( ((unsigned short)(result.data[2] & 0b00000011)))  <<8 | result.data[3]) * CUR_driveTek;
				//voltageOut = (( ((unsigned short)(result.data[4] & 0b00000011))) << 8 | result.data[5]) * VOUT_driveTek;
				adcCount = ( ((unsigned short) (result.data[4] & 0b00000011 ))) << 8 |result.data[5];
				voltageOut = ( (unsigned short) (((float) adcCount * 100 )/  VOUT_driveTek ));
				printf("\rM[%i] %i %i %i 0x%x\n",counter, voltageIn, voltageOut, current, mpptStatus);

		//int voltage = parsMppt((result.data),myMpptData[counter]);
			  //  if(g_messageDebug) 
			   // {
			     //   printf("\rMessage from the MPPT  \n");
				   //printCanMessage(result);
		 	   // }

			}
			else
			{
				printf("\r# mppt TimeOUT, id 0x%x \n",  MPPTAnswerID + 1 + (counter * 2));
			}
	
			counter++;
		}
		

}



void listenForMC()
{
        stCanFrame result;
		unsigned int pollCount = 250;
	    unsigned short filterID = mcBaseAddress | velocityMes   ;
		float * prt_first, *prt_second;
		//int one, two;
		result.id = 0;

	    getMessagesThatLookLike(&(filterID), deviceMC);
	   	mcp2515_get_message(&result ,deviceMC);
		while( !(result.id == filterID) && pollCount)
	   	{

			mcp2515_get_message(&result ,deviceMC)	;
			pollCount--;
		}
		if (result.id == filterID)
	 	{
			//one = convertToInt(result.data);
			//two = convertToInt(result.data+4);
			prt_first = (float *) result.data;
		//	prt_second = ((float *) &(result.data[4]));
			printf("\rS = %i \n",	(int) *prt_first) ;
		  //	printf("\r first %i, second %i\n",(int) *prt_first,(int) *prt_second);
		   // int cool = convertToInt(result.data + 4 , 100 );
		    //printf("\rThe temp is %i\n",cool);
		    //printCanMessage(result);
			//parsMC((result.data), result.id, myMCData);
		}

		pollCount = 250;
		filterID = mcBaseAddress | curentVoltageMes;
		getMessagesThatLookLike(&(filterID), deviceMC);
	   	mcp2515_get_message(&result ,deviceMC);
		while( !(result.id == filterID) && pollCount)
	   	{

			mcp2515_get_message(&result ,deviceMC)	;
			pollCount--;
		}
		if (result.id == filterID)
	 	{
		prt_first = (float *) result.data;
		prt_second = ((float *) &(result.data[4]));
		printf("\rC = %i \n",	(int) ((*prt_second) *100));
		printf("\rW = %i \n", (int) (*prt_first)); 
	//	printf("\r first %i, second %i\n",(int) *prt_first,(int) *prt_second);
			
		}
		else
		{
			printf("\r#MC timeout\n");
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

  //unsigned short filterID = mcBaseAddress | motorTempMes  ;

	bpsDevice_0 = 0;
	bpsDevice_1 = 1;

 // bpsDevice = 0;
   mpptDevice = bpsDevice_0;
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
    //test = mcp_init(mpptDevice, mpptCanConfig);		
	//while ( test != 0)
	//{
	//	printf("\rMPPT. Failer to init MCP 2515, reseting \n");
	//	test = mcp_init(mpptDevice, mpptCanConfig);
	//}
//	mcp2515_normal(mpptDevice);
	//mcp2515_oneShot(mpptDevice);
	//printf("\rMPPT. Pass init MCP 2515\n");

    //theDelay();
    
    //set up up Motor controller can bus
    setmc(mcCanConfig);
    test = mcp_init(deviceMC,mcCanConfig);
    while(test !=0)
    {
		printf("\r MOTOR CONTROLLER.  Failer to init MCP 2515, reseting \n");
	    test = mcp_init(deviceMC,mcCanConfig);
    }
//	mcp2515_normal(deviceMC);
	mcp2515_oneShot(deviceMC);
	printf("\rMOTOR CONTROLLER. Pass init MCP 2515\n");
    theDelay();

	//set up bps can bus _0
   // setBPS(mcCanConfig);
    test = mcp_init(bpsDevice_0, mpptCanConfig);		
	while ( test != 0)
	{
		printf("\rMPPT. Failer to init MCP 2515, reseting \n");
		test = mcp_init(bpsDevice_0, mpptCanConfig);
	}
    //mcp2515_normal(bpsDevice);
	mcp2515_oneShot(bpsDevice_0);
	printf("\rBPS. Pass init MCP 2515\n");


	//set up bps can bus   
//    setBPS(mcCanConfig);
    test = mcp_init(bpsDevice_1, mpptCanConfig);		
	while ( test != 0)
	{
		printf("\rMPPT. Failer to init MCP 2515, reseting \n");
		test = mcp_init(bpsDevice_1, mpptCanConfig);
	}
    //mcp2515_normal(bpsDevice);
	mcp2515_oneShot(bpsDevice_1);

    // set up built in can port
	LATC = 0x00;
	LATB = 0x00;

	//ensure proper TRIS settings
	TRISB |= 0b00001000;//set the CAN input (RB3) to input
	TRISC &= 0b11111011;//set the CAN output (RB2) to output

	//actually start the CAN module
//	ECANInitialize();

		
    //Ending set up

    //starting main loop
	//------------------
    //getMessagesThatLookLike(&(filterID), deviceMC);
	while(1)
	{	
 
	//listenForBPS();
    


	 listenForMC();
	 promptMPPT();
	 promptBPSBox(0);
	 promptBPSBox(1);



//	 promptBPSBox2();
//	 promptBPSBox1();

	
	}
}
