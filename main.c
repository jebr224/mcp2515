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
#include <p18f4480.h>
#include <usart.h>
#include <stdio.h>
#include <delays.h>
#include <spi.h>
#include "mcp2515_defs_JB.h"
//#include "ECANPoll.h"

typedef struct tagstCanFrame{
	unsigned char id[2];
	unsigned char control;
	unsigned char length;
	unsigned char rtr;
	unsigned char data[8];
	}stCanFrame;




 char bit_is_set( unsigned char memory, unsigned char bit);
 
 void chip_active(void);
 void chip_enactive(void);
 
 unsigned char  mcp_read_register(unsigned char adress);
char mcp2515_read_status(char type);
 void mcp_write_adress(unsigned char adress , unsigned char value);
 void mcp2515_bit_modify(unsigned char adress, unsigned char mask, unsigned char data);
 
 void mcp2515_normal();
 void mcp2515_sleep();
 void mcp2515_wakeUp();
 void mcp2515_listen(); 
 void mcp2515_loopBack();
 void mcp2515_config();
 void mcp2515_oneShot();

 void inputFiltersOff();
 
 int  mcp_init(unsigned char speed);
 char mcp2515_get_message( stCanFrame * inMessage);
 unsigned char mcp2515_send_message(stCanFrame *message, unsigned char reg);
 
void debugReg(void);
void dispError();

 //global  temp
 char can_speed = 0x07;


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
	
		test = mcp_init( 0x07);
		while ( test != 0)
		{
			printf("\rFailer to init MCP 2515, reseting \n");
			test = mcp_init(can_speed);
		}
		printf("\r SPI for MCP_2515 is open \n");
	
		
		if (bit_is_set(0b10111111,6))
			printf("\r Error -simple bit shift fails \n");
		else	
			printf("\r simple bit operations work right \n");
		
		
	//	mcp2515_oneShot();
		mcp2515_normal();
//		inputFiltersOff();
//      mcp2515_loopBack();

/*
		while(1)
		{
			stCanFrame  result;
			char wast = mcp2515_get_message(&result);
			printf("\rget_message done\n");
			Delay10TCYx(0x30);

		}
*/
	
		//send a test message
	
		while(1)
		{	
			stCanFrame sample;
		
			sample.id[0] = 0b11111111;
			sample.id[1] = 0b01010101;
			
			sample.data[0] = 0x40;
			sample.data[1] = 0xAA;
			sample.data[2] = 0x60;
			sample.data[3] = 0x70;
			
			sample.length = 4;
			
			sample.rtr = 0x00;
		
			mcp2515_send_message(&sample, 0x02);
			
			Delay10TCYx(0x30);
		}


	}
 }
 

 
 //bit_is_set
 //----------
 //This function returns 1, 0, or -1 based on the inputs. (-1 is error)
 //Memory is a 8 bit char, and if the bit x is 1 the function returns 1
 //Example memory = 0b00000001 and bit = 7 return 1
 //Example memory = 0b00000001 and bit = (anything but 7) return 0
 
  char bit_is_set( unsigned char memory, unsigned char data)
  {
		if (data > 7) return -1;
		return ( ( (1 << data) & memory) != 0);
  }
 
 
 //chip_active
 //-----------
 //This function is pic specific.
 //The fourth pin on bus A is used as the CS pin
 //The opposite of the function is the chip_enactive. 
 //For every call to chip active there should be a call to chip enactive
 
 void chip_active(void)
 {
    PORTAbits.RA4 = 0;  //Pin 4 is the CS pin, setting it low activates spi the chip
	Delay10TCYx(0x01);  
 }
 
 
 //chip_enactive
 //--------------
 //This function is pic specificc
 //The fourth pin on bus A is used as the CS pin
 // //The opposite of the function is the chip_enactive.
 
 void chip_enactive(void)
 {
	PORTAbits.RA4 = 1;
	Delay10TCYx(0x01);  // setting pin 4 low deactivates the spi for the chip
	
 }
 
 //mcp_read_register
 //-----------------
 //This function sends the SPI command for to read a register
 //The parameter passed in is the adress thats value will be returned.
 
unsigned char mcp_read_register(unsigned char adress)
  {
    unsigned char data;
    chip_active();
	
    putcSPI(SPI_MCP_READ);
    putcSPI(adress);
    data = ReadSPI();
	
    chip_enactive();
    
	return data;
  
  } 
  
 //mcp_write_adress
 //----------------
 //This function lets you write a char to the registers on the chip
 //For a list of all registers check the section 11 page 61 in the data sheet
 //If your lucky they will be in the def file
 //NOTE: The SPI_MCP_WRITE command allows sequential registers to be written in then same communtication block
 
void  mcp_write_adress(unsigned char adress , unsigned char value)
 {
	chip_active();
  
	putcSPI(SPI_MCP_WRITE);
	putcSPI(adress);
	putcSPI(value);
  
	chip_enactive();
	
	return;
 }
  
  //page 64
  
void mcp2515_bit_modify(unsigned char adress, unsigned char mask, unsigned char data)
{

	//printf("\r bit modify \n %b \n %b \n %b \n",adress, mask, data );
    chip_active();
	
	putcSPI(SPI_MCP_BIT_MODIFY);
	putcSPI(adress);
	putcSPI(mask);
	putcSPI(data);
	
	chip_enactive();

}

//mcp_normal
//----------
//This function set the MCP2515 into normal operation
//This can also be done by running the init command
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
 
 void mcp2515_normal()
 {
	char value;
	//chip_active();
	mcp_write_adress(CANCTRL,  0);
	//mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), 0 );
	
	//chip_enactive();
	
	value = mcp_read_register(CANCTRL);

	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != 0x00)
	{
		printf("\r Error in setting to normal mode   CANSTAT = %b \n", value);
		return;
	}else 
	{
		printf("\rstatus was set to normal mode\n");
		return;
	}
 
 }
  
//mcp2515_sleep
//-------------
//This function put the 2515 to sleep.
//The SPI of for the chip will still be active, 
//but can aspect of the chip will not be active.
//TX remains in the recessive state in sleep mode
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
//NORMAL MODES 350 to 280 mA

 void mcp2515_sleep()
 {
	char value;
    //chip_active();
	//mcp_write_adress(CANCTRL,  0b00100000 );
	//mcp_write_adress(CANCTRL,  1 << REQOP0 );
	mcp2515_bit_modify(CANCTRL, (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), 1 << REQOP0);
	
	//chip_enactive();
	
	value = mcp_read_register(CANCTRL);
	
	
	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != (1 << REQOP0 ))
	{
		printf("\r Error in setting to Sleep mode    CANSTAT =  %b \n", value);
		return;
	}else 
	{
		printf("\r status was set to sleep mode \n");
		return;
	}
 
 }
 //mcp2515_wakeUp()
 //---------------
 
 void mcp2515_wakeUp()
 {
	unsigned char value;

	
	mcp2515_bit_modify(CANINTE, 1 << WAKIE , 1 << WAKIE);
	//mcp2515_normal();
	Delay10TCYx(0x30);	
	
	value = mcp_read_register(CANINTE);
	if (value & (1 << WAKIE))
		printf("\r status was set to Awake mode \n");	
	else 
		printf("\r Wake up failed \n");
	return;
	
	
 }
 
 

//cmp2515_listen
//--------------
//This function puts the 2515 into a listen ONLY mode
//When in listen only mode the chip is unable to send any message of the bus,
//And no warrings will be given.
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
 
 void mcp2515_listen()
 {
	char value;
	//chip_active();
	//mcp_write_adress(CANCTRL, ((1 << REQOP0) | (1 << REQOP1))  );
	
	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), ((1 << REQOP0) | (1 << REQOP1)) );
	//chip_enactive();
	
	value = mcp_read_register(CANCTRL);

	
	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2))  != ( (1 << REQOP0) | (1 << REQOP1))  )
	{
		printf("\r Error in setting to listen mode   CANSTAT = %b \n", value);
		return;
	}else 
	{
		printf("\r status was set to listen mode \n");
		return;
	}
 
	
 }
 
 
//mcp_2515_loopBack
//-----------------
//loop back is used for debugging CAN applications
//This mode allows to load the messages that you send
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
 
 void mcp2515_loopBack()
 {
	char value;
	//chip_active();
	//mcp_write_adress(CANCTRL, ( 1<< REQOP1));
    //chip_enactive();
	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2),  (1 << REQOP1)) ;
	
	value = mcp_read_register(CANCTRL);
	
	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != ( 1<< REQOP1) ) 
	{
		printf("\r Error in setting to loopBack mode  CANSTAT = %b \n", value);
		return;
	}else 
	{
		printf("\r status was set to loopBack mode \n");
		return;
	}
 
	
 }
 
 
//mcp2515_config mode
//--------------
//This function lets special registers to be configured 
//The CNF1, CNF2, CNF3, TXRTSCTRL, filter and mask, and only be set while in config mode
//The init function configs the chip. IF you call init DO NOT CALL CONFIG
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
 
 void mcp2515_config()
 {
	char value;
	char pendingOp;
	//chip_active();
	//mcp_write_adress(CANCTRL, (1 << REQOP2)); //0b
	//chip_enactive(); 
	

	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2),  (1 << REQOP2)) ;
   

	value = mcp_read_register(CANCTRL);
		
	if ((value  & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != (1 << REQOP2) )
	{
		printf("\r Error in setting to config mode CANSTAT = %b \n", value);
	}else 
	{
		printf("\r status was set to config mode \n");
	}
 
	pendingOp = mcp_read_register(CANSTAT);
	while(pendingOp &( (1<<OPMOD0) | (1<<OPMOD1) | (1<<OPMOD2) ) != (1<<OPMOD2))
		printf("\r waiting \n");
}

void mcp2515_oneShot()		//FIX THIS LATER //One shot mode changes the CANCTRL reg 
{
	chip_active();
	mcp_write_adress( CANCTRL, ( 1 << OSM));
	chip_enactive();

}
  
int mcp_init(unsigned char speed)
{
    unsigned char data ;
	unsigned char timingReg[4];
	//speed = 0X07; //change this later
    TRISA = 0x0F;
    
    // 16 Mhz crystal at a speed of 125k works a treat
    /*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0x07;  					      //Value for CNF1
    timingReg[2] = ((1<<BTLMODE) | (1<<PHSEG11)); //Value for CNF2
	timingReg[3] = (1<<PHSEG21);                  //Value for CNF3 	
	*/

	// 20 Mhz crystal at a speed of 125k works very well
	
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = (( 1 << SJW0)|(1<<BRP2));					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)  | (1<<PHSEG12) | (1<<PHSEG11) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] = ( (1<<PHSEG20) |(1<<PHSEG22)); //Value for CNF3 
	
	
	// 20 Mhz crystal at a speed of 1 Mbit (it looks like it would work, but the transiver is not fast enough
 	/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0;					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)   | (1<<PHSEG1) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] =  (1<<PHSEG22); //Value for CNF3 
	*/
 

//Value for CANINTE	
    chip_active();
	
    Delay10TCYx(0x1);	
    putcSPI(SPI_MCP_RESET);					//when the chip return from reset, it will be in config mode
	
	chip_enactive();
	
	Delay10TCYx(0x30);	 					//delay 3000 cyles for the chip to reset, experimental number
	
	chip_active();
	putcSPI(SPI_MCP_WRITE);
	putcSPI(CNF3);          				//page 43 //start writing at CNF3
	
	/*
	putcSPI((1<<PHSEG21)); 					// Value to be stored at CNF3 Bitrate 125 kbps at 16 MHz
	putcSPI((1<<BTLMODE) | (1<<PHSEG11)); 	//Value for CNF2
	putcSPI(speed);                    		 //Value for CNF1
	putcSPI((1<<RX1IE)|(1<<RX0IE));    		 //Value for CNF0
	*/
	putcSPI(timingReg[3]); 					//Value to be stored at CNF3 Bitrate 125 kbps at 16 MHz
	putcSPI(timingReg[2]); 	                //Value for CNF2
	putcSPI(timingReg[1]);                  //Value for CNF1
	putcSPI(timingReg[0]);    		        //Value for CANINTE	
    
	chip_enactive();

	//High impedance on the RXnBF pin
	mcp_write_adress(BFPCTRL, 0);   
	
	//Set TXnRTS to use as possible inputs
	mcp_write_adress(TXRTSCTRL, 0); 
	
	// turn off filters => receive any message
	// Both RX buffers have filters disabled
	//page 27 //  RXM1 = 6  RXM0 = 5  
	// (1 << 6) || ( 1 << 5) = b01100000 = 0x60
	mcp_write_adress(RXB0CTRL, ((1<<RXM1)|(1<<RXM0)));// | (1<<RXRTR) | (1<<BUKT)));
	mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0)));// | (1<<RXRTR) | (1<<BUKT))); 
	
	
	mcp_write_adress(CANCTRL, 0); //set the chip to normal mode 
	
	data = mcp_read_register(CNF1);
	

	  
    //printf("\r The status was %X---- %X \n",data,speed);	 //Debug print statment 
    Delay100TCYx(0x01); //delay 1000
	
	if (data !=  timingReg[1])
	{
		printf("\r SomeThing fuked up \n");
		return -1;
	}
	
	
	
	
	
	
	return 0;
  }

  
  
char mcp2515_read_status(char type)
{
	char data;
	chip_active(); 
//	putcSPI(0x03); //? new
	putcSPI(type);
	data = ReadSPI();
	
	chip_enactive();
	return data;
}

  
void inputFiltersOff()
{
	mcp2515_config();
	mcp_write_adress(RXB0CTRL, ((1<<RXM1)|(1<<RXM0) | (1<<RXRTR)  ));
	mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0) | (1<<RXRTR)  )); 
	mcp2515_normal();

	mcp_write_adress(RXB0CTRL, ((1<<RXM1)|(1<<RXM0)| (1<<RXRTR) ));
	mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0)| (1<<RXRTR) )); 
} 


char mcp2515_get_message(stCanFrame *inMessage)
{
	// read status
	unsigned char status = mcp2515_read_status(SPI_MCP_RX_STATUS);
	unsigned char addr;
	unsigned char id,wast,length, counter;
	//unsigned char d,e,f,g,h,i;
	unsigned char data[20];
	
	dispError();
	printf("\r RX_status -> %b \n",status);



	//lets see if any of the can registers have a message
	if (bit_is_set(status,6)) {
     	//printf("\rYou got Main in 1 \n");
		addr = SPI_MCP_READ_RX;

	}
	else if (bit_is_set(status,7)) {
		//printf("\rYou got Mail in 2 \n");
		addr = SPI_MCP_READ_RX | 0x04;
	
	}
	else {
		// you have no message available
		printf("\rThere is no message in the buffer \n");
		return 0;
	}

	chip_active();
	putcSPI(addr);
	id = ReadSPI() <<3;
	id |= ReadSPI() >>5;

	
	wast = ReadSPI() ;
	wast = ReadSPI() ;
	length = ReadSPI() ;
	counter = 0;
	while( counter < length)
	{
		data[counter] = ReadSPI();
		counter = counter +1;
	}

	chip_enactive();

	printf("\r The Id is  %x \n",id);
	printf("\r The message has %i bytes of data \n",length);
	counter = 0;
	while( counter < length)
	{
		printf("\r The data at %i is %x  \n",counter, data[counter]);
	
		counter = counter +1;
	}
	

	if( bit_is_set(status,6))
	{
		 mcp2515_bit_modify(CANINTF, (1<<RX0IF),0);
		printf("\r mod1 \n");
	}
	else
	{
		 mcp2515_bit_modify(CANINTF, (1<<RX1IF),0);
			printf("\r mod 2\n");
	}
	return addr;
}




unsigned char mcp2515_send_message(stCanFrame *message, unsigned char reg)
{



	unsigned char length;
	unsigned char t = 0;
	if ((reg != 0) && (reg != 1) && (reg != 2))
	{
			printf("\r Register slection not Valid -Load message failed \n");
			return -1;
	} 
	else if (reg == 2)  reg = 0x04;
	else if (reg == 1)  reg = 0x02;
	else if (reg == 0)  reg = 0x00;
	else printf("\rsomething has gone wrong in the register selection part acpect of RX /n");
	
//	dispError();

	
	chip_active();
	putcSPI(SPI_MCP_WRITE_TX | reg);
	
	putcSPI((*message).id[0]  >> 3);
    putcSPI((*message).id[1]  << 5);
	
	putcSPI(0);
	putcSPI(0);
	
	length = (*message).length & 0x0f;
	
	if ((*message).rtr) {
		// a rtr-frame has a length, but contains no data
		putcSPI((1<<RTR) | length);
		printf("\rrtr != 0\n");
	}
	else {
		// set message length
		putcSPI(length);
		printf("\rrtr == 0\n");
		// data
		printf("\r writting data \n");
		for (t=0;t<length;t++) {
			printf("[%x]",(*message).data[t]);
			putcSPI((*message).data[t]);
		}
		printf("\n");
	}
	chip_enactive();
	
	Delay10TCYx(0x30);

	// send message
	chip_active();

	reg = (reg == 0) ? 1 : reg; 
	putcSPI(SPI_MCP_RTS | reg);

	chip_enactive();
	
	return 1;
}
 
void dispError()
{
	unsigned char rec_error = mcp_read_register(REC);
	unsigned char tec_error = mcp_read_register(TEC);
	unsigned char error_flags =mcp_read_register(EFLG);

	
	printf("\r the rec_error is %i and the tec error is %i  \n",rec_error, tec_error);
	printf("\rThe error byte -> %b \n",error_flags);
	return; 
}

