//////////////////////////////////////////////////////////////////////
// MICRO CONFIGURATION
#pragma config OSC = HS     // Oscillator 
#pragma config PWRT = OFF   // Power on delay 
#pragma config WDT = OFF    // WatchDog Timer
#pragma config LVP = OFF    // Low Voltage Programming OFF



// TURN OFF CODE PROTECTION REGISTERS 

// DEFINES -- After Initial Testing this part of the code can be ported to another header file which can be included
// Using Standard Indetifier Length - Max allowable identifier 0x7FF
// PORTED IT TO A HEADER FILE NAMED CANID.H - 04/06/2010

// INCLUDES

#include "mpptDriveTek.h"
#include "mcp2515.h"
#include <p18f4480.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>
#include <spi.h>



//#include "mpptDriveTek.c"
//#include "ECANPoll.h"


  

 
 //global  temp
 // char can_speed = 0x07;


void main()
{


  OpenUSART( USART_TX_INT_OFF &
  USART_RX_INT_OFF &
  USART_ASYNCH_MODE &
  USART_EIGHT_BIT &
  USART_CONT_RX &
  USART_BRGH_HIGH, 71);
  
 printf("\n\r Serial Open \n");

  OpenSPI( SPI_FOSC_16,   //Master clock 16mhz
  MODE_00,                 
  //MODE_01,
  SMPEND);                  //Output data a end of buff

  


 while(1)
	{	
		int test;
		unsigned char del;
		stCanFrame sample;
        unsigned char mpptDevice = 0x01;
        unsigned char deviceMC = 0x02;
	    unsigned char mpptCanConfig[4];
        unsigned char mcCanConfig[4];

        selectNoDevice();







        setmppt(mpptCanConfig);
        test = mcp_init(mpptDevice, mpptCanConfig);
		
        printf("\r debug %x %x %x %x\n", mpptCanConfig[0],mpptCanConfig[1],mpptCanConfig[2],mpptCanConfig[3]);
		while ( test != 0)
		{
			printf("\rMPPT . Failer to init MCP 2515, reseting \n");
			test = mcp_init(mpptDevice, mpptCanConfig);
		}
  
		printf("\r SPI for MCP_2515 is open \n");

		mcp2515_normal(mpptDevice);

        delay();
       
        setmc(mcCanConfig);
        printf("\r debug %x %x %x %x\n",mcCanConfig[0],mcCanConfig[1],mcCanConfig[2],mcCanConfig[3]);
        test = mcp_init(deviceMC,mcCanConfig);
        while(test !=0)
        {
			printf("\r MOTOR CONTROLLER.  Failer to init MCP 2515, reseting \n");
		    test = mcp_init(deviceMC,mcCanConfig);
        }
	    mcp2515_normal(deviceMC);
        delay();

		if (bit_is_set(0b10111111,6))
			printf("\r Error -simple bit shift fails \n");
		else	
			printf("\r simple bit operations work right \n");
		
		
	//	mcp2515_oneShot();

//		inputFiltersOff();
//      mcp2515_loopBack();


/*
		while(1)
		{
			unsigned char counter = 0, temp;
			stCanFrame  result;
			char wast = mcp2515_get_message(&result);
			printf("\rget_message done\n");
			printf("\The ID in hex is %x\n",result.id);
			printf("\rThe length is %i\n",result.length);
			counter = 0;
			while(counter < result.length)
			{
				temp = (result.data[counter]);
				printf("\r  The data at %i  is  %x  \n",counter,temp);
				counter++;	
			}
			Delay10TCYx(0x30);

		}

*/
		//send a test message
/*	
		while(1)
		{	
			stCanFrame sample;
			sample.id = 0x42;		
//			sample.id[0] = 0b11111111;
//			sample.id[1] = 0b01010101;
			
//			sample.id = 0b0000001110001000;
			sample.length = 4;
			sample.data[0] = 0x40;
			sample.data[1] = 0xAA;
			sample.data[2] = 0x60;
			sample.data[3] = 0x70;
			

			sample.rtr = 0x00;
		
			mcp2515_send_message(&sample, 0x02);
			
			Delay10TCYx(0x30);
		}
*/
		while(1)
		{	
            mpptData data;
			stCanFrame sample;
			stCanFrame result;
			char wast;
			unsigned char counter = 0, temp;
            

			
			sample.id = 0x711;
			sample.length = 0;
			sample.data[0] = 0x40;
			sample.data[1] = 0xAA;
			sample.data[2] = 0x60;
			sample.data[3] = 0x70;
			
			sample.rtr =0 ;//=0xff;// 0x00;

		
			mcp2515_send_message(&sample, 0x02,mpptDevice);

			

			Delay10TCYx(0x30);
		
			 if(mcp2515_get_message(&result ,mpptDevice))
			{ 
			   
				printf("\rThe address is %x \n",result.id);
				while(counter < result.length)
				{
					temp = (result.data[counter]);
					printf("\r  The data att %i  is  %x  \n",counter,temp);
					counter++;	
				}
			    parsMppt((result.data), data);
			 	delay();
			}

  
      	Delay10TCYx(0x30);
		 if(mcp2515_get_message(&result ,deviceMC))
		{
               printf("\r MOTOR CONROLLER \n");
	           printf("\rThe address is %x \n",result.id);
               printf("\rThe length is %i \n",result.length);
               counter = 0;
				while(counter < result.length)
				{
					temp = (result.data[counter]);
					printf("\r  The data att %i  is  %x  \n",counter,temp);
					counter++;	
				}
			 	delay();
		 }
         else
		 {
			printf("\r No message MC\n");
            //sample code
          //  mcp2515_send_message(&result, 0x02,deviceMC);
            
		 } 




		

	}	
}	


 
} 