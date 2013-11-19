#include <p18f4480.h>
#include <spi.h>
#include <stdio.h>
#include "mcp2515.h"

#include "mcp2515_defs_JB.h"


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
 
 void chip_active( unsigned char deviceNumber)
 {
     //used for breadboard JB
    //PORTAbits.RA4 = 0;  //Pin 4 is the CS pin, setting it low activates spi the chip
    
   // PORTDbits.RD0 = 0;
  //  PORTDbits.RD1 = 0;
   // PORTDbits.RD2 = 0;
	//Delay10TCYx(0x01); 
 
   if (deviceNumber == 0x00)
   {
      //PORTDbits.RD0 = 0;
      LATDbits.LATD0 = 0;
   }
   else if ( deviceNumber == 0x01)
   {
       //PORTDbits.RD1 = 0;
  LATDbits.LATD1 = 0;
   }
   else if(deviceNumber == 0x02)
   { 
        //PORTDbits.RD2 = 0;
  LATDbits.LATD2 = 0;
   } 
   else 
   {
      printf("\n\rinvalide SPI device to activate (what the fuck are you doing?)");
   }

	Delay10TCYx(0x01);  // setting pin 4 low deactivates the spi for the chip

 
 }
 
 
 //chip_enactive
 //--------------
 //This function is pic specificc
 //The fourth pin on bus A is used as the CS pin
 // //The opposite of the function is the chip_enactive.
 
 void chip_enactive( unsigned char deviceNumber)
 {
//used on breadboard JB
//	PORTAbits.RA4 = 1;
   if (deviceNumber == 0x00)
   {
     // PORTDbits.RD0 = 1;
     LATDbits.LATD0 = 1;
   }
   else if ( deviceNumber == 0x01)
   {
     //  PORTDbits.RD1 = 1;
         LATDbits.LATD1 = 1;
   }
   else if(deviceNumber == 0x02)
   { 
       // PORTDbits.RD2 = 1;
          LATDbits.LATD2 = 1;
   } 
   else 
   {
      printf("\n\rinvalide SPI device to activate (what the fuck are you doing?)");
   }

	Delay10TCYx(0x01);  // setting pin 4 low deactivates the spi for the chip
	
 }
 
 //mcp_read_register
 //-----------------
 //This function sends the SPI command for to read a register
 //The parameter passed in is the adress thats value will be returned.
 
unsigned char mcp_read_register(unsigned char adress,unsigned char device)
  {
    unsigned char data;
    chip_active(device);
	
    putcSPI(SPI_MCP_READ);
    putcSPI(adress);
    data = ReadSPI();
    Delay10TCYx(0x01);
    
    chip_enactive(device);

	return data;
  
  } 
  
 //mcp_write_adress
 //----------------
 //This function lets you write a char to the registers on the chip
 //For a list of all registers check the section 11 page 61 in the data sheet
 //If your lucky they will be in the def file
 //NOTE: The SPI_MCP_WRITE command allows sequential registers to be written in then same communtication block
 
void  mcp_write_adress(unsigned char adress , unsigned char value, unsigned char device)
 {
	chip_active(device);
  
	putcSPI(SPI_MCP_WRITE);
	putcSPI(adress);
	putcSPI(value);
  
	chip_enactive(device);
	
	return;
 }
  
  //page 64
  
void mcp2515_bit_modify(unsigned char adress, unsigned char mask, unsigned char data, unsigned char device)
{

	//printf("\r bit modify \n %b \n %b \n %b \n",adress, mask, data );
    chip_active(device);
	
	putcSPI(SPI_MCP_BIT_MODIFY);
	putcSPI(adress);
	putcSPI(mask);
	putcSPI(data);
	
	chip_enactive(device);

	return;

}

//mcp_normal
//----------
//This function set the MCP2515 into normal operation
//This can also be done by running the init command
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry)
 
 void mcp2515_normal(unsigned char device)
 {
	char value;

	//	mcp_write_adress(CANCTRL,  0);
	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), 0 , device);

	
	value = mcp_read_register(CANCTRL,device);

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
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry) FIXED
//NORMAL MODES 350 to 280 mA

 void mcp2515_sleep(unsigned char device)
 {
	char value;
	mcp2515_bit_modify(CANCTRL, (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), 1 << REQOP0, device);

	value = mcp_read_register(CANCTRL,device);
	
	
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
 
 void mcp2515_wakeUp(unsigned char device)
 {
	unsigned char value;

	
	mcp2515_bit_modify(CANINTE, 1 << WAKIE , 1 << WAKIE, device);
	//mcp2515_normal();
	Delay10TCYx(0x30);	
	
	value = mcp_read_register(CANINTE,device);
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
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry) FIXED
 
 void mcp2515_listen(unsigned char device)
 {
	char value;
	
	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2), ((1 << REQOP0) | (1 << REQOP1)), device);

	
	value = mcp_read_register(CANCTRL,device);

	
	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2))  != ( (1 << REQOP0) | (1 << REQOP1)),device )
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
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry) FIXED
 /*
 void mcp2515_loopBack(unsigned char device)
 {
	char value;
	//chip_active();
	//mcp_write_adress(CANCTRL, ( 1<< REQOP1));
    //chip_enactive();
	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2),  (1 << REQOP1), device) ;
	
	value = mcp_read_register(CANCTRL, device);
	
	if ((value & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != ( 1<< REQOP1), device )  //What
	{
		printf("\r Error in setting to loopBack mode  CANSTAT = %b \n", value);
		return;
	}else 
	{
		printf("\r status was set to loopBack mode \n");
		return;
	}
 
	
 }
 */
 
//mcp2515_config mode
//--------------
//This function lets special registers to be configured 
//The CNF1, CNF2, CNF3, TXRTSCTRL, filter and mask, and only be set while in config mode
//The init function configs the chip. IF you call init DO NOT CALL CONFIG
//NOTE: If the time perscaler for CLOCK out on the 2515 has been set, this will unset it(sorry) FIXED
 
 void mcp2515_config(unsigned char device)
 {
	char value;
	char pendingOp;
	//chip_active();
	//mcp_write_adress(CANCTRL, (1 << REQOP2)); //0b
	//chip_enactive(); 
	

	mcp2515_bit_modify(CANCTRL,  (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2),  (1 << REQOP2), device) ;
   

	value = mcp_read_register(CANCTRL,device);
		
	if ((value  & (1 << REQOP0 | 1 << REQOP1 | 1 << REQOP2)) != (1 << REQOP2) )
	{
		printf("\r Error in setting to config mode CANSTAT = %b \n", value);
	}else 
	{
		printf("\r status was set to config mode \n");
	}
 
    //this looks stupid
	pendingOp = mcp_read_register(CANSTAT, device);
	while(pendingOp &( (1<<OPMOD0) | (1<<OPMOD1) | (1<<OPMOD2) ) != (1<<OPMOD2))
		printf("\r waiting \n");
}

void mcp2515_oneShot(unsigned char device)		//FIX THIS LATER //One shot mode changes the CANCTRL reg 
{
	chip_active(device);
	mcp_write_adress( CANCTRL, ( 1 << OSM), device);
	chip_enactive(device);

}
  
int mcp_init( unsigned char device, unsigned char *config)
{
    unsigned char data ;
	unsigned char timingReg[4];
    timingReg[0]=config[0];
    timingReg[1]=config[1];

    timingReg[2]=config[2];
    timingReg[3]=config[3];
 
 
    //CMCON = 0xFF; //shutdown the comparitor modual, lets us use port D as gpio, jimmy figured this out
    //CMCON = 0b00000111;
    //ECCP1AS = 0b00000010;
    //CCP1CON = 0b00000000;
    //TRISA = 0x0F;
  //  TRISD = 0b00000000;//0b11111000; //temp
    // 16 Mhz crystal at a speed of 125k works a treat
    /*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0x07;  					      //Value for CNF1
    timingReg[2] = ((1<<BTLMODE) | (1<<PHSEG11)); //Value for CNF2
	timingReg[3] = (1<<PHSEG21);                  //Value for CNF3 	
	*/

	/*
	// 20 Mhz crystal at a speed of 125k works very well
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = (( 1 << SJW0)|(1<<BRP2));					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)  | (1<<PHSEG12) | (1<<PHSEG11) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] = ( (1<<PHSEG20) |(1<<PHSEG22)); //Value for CNF3 
	*/

	
	// 20 Mhz crystal at a speed of 1 Mbit (it looks like it would work, but the transiver is not fast enough
 	/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0;					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)   | (1<<PHSEG1) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] =  (1<<PHSEG22); //Value for CNF3 
	*/
	// 20 Mhz crystal at a speed of 100 k 
	/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = (1 << BRP2);					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)   | (1<<PHSEG10) | (1<<PHSEG11) | (1<<PHSEG12) | (1<<PHSEG0) | (1<<PHSEG1) ); //Value for CNF2
	timingReg[3] =  (1<<PHSEG20) | (1<<PHSEG21) | (1<<PHSEG22) ; //Value for CNF3 
	*/
 	// 125 MPPT Stupid
	/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0b00000100;				  //Value for CNF1
	timingReg[2] = 0b10001110;
	timingReg[3] = 0b00000101; //Value for CNF3 
	*/

//Value for CANINTE	
    chip_active(device);
	
    Delay10TCYx(0x1);	
    putcSPI(SPI_MCP_RESET);					//when the chip return from reset, it will be in config mode
	
	chip_enactive(device);
	
	Delay10TCYx(0x30);	 					//delay 3000 cyles for the chip to reset, experimental number
	
	chip_active(device);
	putcSPI(SPI_MCP_WRITE);
	putcSPI(CNF3);          				//page 43 //start writing at CNF3
	
	putcSPI(timingReg[3]); 					//Value to be stored at CNF3 Bitrate 125 kbps at 16 MHz
	putcSPI(timingReg[2]); 	                //Value for CNF2
	putcSPI(timingReg[1]);                  //Value for CNF1
	putcSPI(timingReg[0]);    		        //Value for CANINTE	
    
	chip_enactive(device);

	//High impedance on the RXnBF pin
	mcp_write_adress(BFPCTRL, 0,device);   
	
	//Set TXnRTS to use as possible inputs
	mcp_write_adress(TXRTSCTRL, 0,device); 
	
	// turn off filters => receive any message
	// Both RX buffers have filters disabled
	//page 27 //  RXM1 = 6  RXM0 = 5  
	// (1 << 6) || ( 1 << 5) = b01100000 = 0x60
	mcp_write_adress(RXB0CTRL, ((1<<RXM1)|(1<<RXM0)),device);// | (1<<RXRTR) | (1<<BUKT)));
	mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0)),device);// | (1<<RXRTR) | (1<<BUKT))); 
	
	
	mcp_write_adress(CANCTRL, 0,device); //set the chip to normal mode 
	
	data = mcp_read_register(CNF1,device);
	

	  
    //printf("\r The status was %X---- %X \n",data,speed);	 //Debug print statment 
    Delay100TCYx(0x01); //delay 1000
	
	if (data !=  timingReg[1])
	{
		printf("\r SomeThing fuked up \n");
		return -1;
	}
	else
	{
			printf("\rInit Timing is set \n");
	}
	return 0;
  }

  
  
//mcp2515_read_status
//--------------------
// This function returns the value of a given status register.
//Not all regesters are status register. If you would like to read a normal 
//register call the function [ unsigned char mcp_read_register(unsigned char adress) ]
char mcp2515_read_status(char type,unsigned char device)
{
	char data;
	chip_active(device); 
	putcSPI(type);
	data = ReadSPI();
	 Delay10TCYx(0x01);

	chip_enactive(device);
	return data;
}


//inputFiltersOff() 
//-------------------
//The messages that are loaded into buffers will be any messages on 
//the bus, if this function is called
void inputFiltersOff(unsigned char device)
{
	mcp2515_config(device);
	mcp_write_adress(RXB0CTRL, ((1<<RXM1)|(1<<RXM0) | (1<<RXRTR)  ),device);
	mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0) | (1<<RXRTR)  ), device); 
	mcp2515_normal(device);

	return;
} 

//inputFiltersON_RX_0
void inputFilersON_RX_0(unsigned char device)
{
	mcp2515_config(device);
	mcp_write_adress(RXB0CTRL, ((1<<RXM1) | (1<<RXRTR) | (0<< FILHIT0 )),device);
	//mcp_write_adress(RXB1CTRL, ((1<<RXM1)|(1<<RXM0) | (1<<RXRTR) | (1<< FILHIT0 )),device); //NOT Needed
	mcp2515_normal(device);
}

//getMessagesThatLookLike
//------------------------
//page 34 setting RXF1SIDH and RXF1SIDL
//This command will set the filter for the RX_0.


void getMessagesThatLookLike(unsigned short * prt,unsigned char device)
{
    unsigned char temp;
    inputFilersON_RX_0(device);
	mcp2515_config(device);
    mcp_write_adress(RXM0SIDH,0xFF,device);
    mcp_write_adress(RXM0SIDL,0xFF,device);

    temp = (unsigned char) (prt[0] >> 3);
    mcp_write_adress(RXF0SIDH, temp,device);
    temp = (unsigned char) (prt[0] << 5); 
    mcp_write_adress(RXF0SIDL, prt[0] << 5 ,device);


	mcp2515_normal(device);

    printf("%x \n",prt[0]);
    printf("%x \n",prt[1]);

}


char mcp2515_get_message(stCanFrame *inMessage, unsigned char device)
{
	// read status
	unsigned char status = mcp2515_read_status(SPI_MCP_RX_STATUS , device);
	unsigned char addr;
	unsigned char wast,length, counter;
	unsigned short id;
	//unsigned char d,e,f,g,h,i;
	unsigned char data[20];
	
	dispError(device);
	printf("\r RX_status -> %b \n",status);



	//lets see if any of the can registers have a message
	if (bit_is_set(status,6)) {
     	printf("\rYou got Mail in 1 \n");
		addr = SPI_MCP_READ_RX;

	}
	else if (bit_is_set(status,7)) {
		printf("\rYou got Mail in 2 \n");
		addr = SPI_MCP_READ_RX | 0x04;
	
	}
	else {
		// you have no message available
		printf("\rThere is no message in the buffer \n");
		return 0;
	}

	chip_active(device);
	putcSPI(addr);
	id =0;
	id = ((	unsigned short) ReadSPI()) <<3;
	id |= ((	unsigned  short) ReadSPI()) >>5;
	inMessage.id = id;
	
	wast = ReadSPI() ;
	wast = ReadSPI() ;
	length = ReadSPI() ;
	inMessage.length = length;
	counter = 0;
	while( counter < length)
	{
		inMessage.data[counter] = data[counter] = ReadSPI();
		counter = counter +1;
	}

	chip_enactive(device);

/*
// Debug printing
	printf("\r The Id is  %x \n",id);
	printf("\r The message has %i bytes of data \n",length);
	counter = 0;
	while( counter < length)
	{
		printf("\r The data at %i is %x  \n",counter, data[counter]);
	
		counter = counter +1;
	}
*/

	if( bit_is_set(status,6))
	{
		 mcp2515_bit_modify(CANINTF, (1<<RX0IF),0, device);
		printf("\r mod1 \n");
	}
	else
	{
		 mcp2515_bit_modify(CANINTF, (1<<RX1IF),0,device);
			printf("\r mod 2\n");
	}
	return addr;
}




unsigned char mcp2515_send_message(stCanFrame *message, unsigned char reg, unsigned char device)
{

	unsigned char length;
	unsigned char t = 0, topID,bottomID,*split;
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
	split = ((unsigned char *) &((*message).id));
	topID =(split[0] << 5);
	bottomID = ((split[0] >> 3) |(split[1] << 5));
	
	printf("\rtesting id %x, and %b message length %i\n",(*message).id ,(*message).id, (*message).length) ;
	

	chip_active(device);
	putcSPI(SPI_MCP_WRITE_TX | reg);
	
//	putcSPI((*message).id[0]  >> 3);
//  putcSPI((*message).id[1]  << 5);
	 putcSPI(bottomID);  //page 19 reg 3-3 //The 8 bits of the reg are the bits 2 to 11 of the id
     putcSPI(topID);   //page 20 reg 3-4 //The last 3 bits of reg  are the 0 to 1 bits of the id
	
    
  	 putcSPI(0);  //page 19 reg 3-3 //The 8 bits of the reg are the bits 2 to 11 of the id
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
	chip_enactive(device);
	
	Delay10TCYx(0x30);

	// send message
	chip_active(device);

	reg = (reg == 0) ? 1 : reg; 
	putcSPI(SPI_MCP_RTS | reg);

	chip_enactive(device);
	
	return 1;
}







unsigned char mcp2515_send_ex_message(stCanFrame *message, unsigned char reg, unsigned char device)
{



	unsigned char length;
	unsigned char t = 0, topID,bottomID,*split;
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
	split = ((unsigned char *) &((*message).id));
	topID =(split[0] << 5);
	bottomID = ((split[0] >> 3) |(split[1] << 5));
	
	printf("\rtesting id %x, and %b message length %i\n",(*message).id ,(*message).id, (*message).length) ;
	

	chip_active(device);
	putcSPI(SPI_MCP_WRITE_TX | reg);
	
//	putcSPI((*message).id[0]  >> 3);
//  putcSPI((*message).id[1]  << 5);
	 putcSPI(bottomID);  //page 19 reg 3-3 //The 8 bits of the reg are the bits 2 to 11 of the id
     putcSPI(topID | (1 << EXIDE));   //page 20 reg 3-4 //The last 3 bits of reg  are the 0 to 1 bits of the id
	   
  	 putcSPI(0);  //page 19 reg 3-3 //The 8 bits of the reg are the bits 2 to 11 of the id
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
	chip_enactive(device);
	
	Delay10TCYx(0x30);

	// send message
	chip_active(device);

	reg = (reg == 0) ? 1 : reg; 
	putcSPI(SPI_MCP_RTS | reg);

	chip_enactive(device);
	
	return 1;
}



 
void dispError(unsigned char device)
{
	unsigned char rec_error = mcp_read_register(REC,device);
	unsigned char tec_error = mcp_read_register(TEC,device);
	unsigned char error_flags =mcp_read_register(EFLG,device);

	
	printf("\r the rec_error is %i and the tec error is %i  \n",rec_error, tec_error);
	printf("\rThe error byte -> %b \n",error_flags);
	return; 
}

void delay(void)
{
	unsigned char t =0;
	unsigned char b = 0;
	while(t < 254)
	{

	while(b < 254)
		{
			b++;
		}
	b = 0;
	t++;
	}
}





void setmppt(unsigned char *buff)
{
    buff[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
    buff[1] = (( 1 << SJW0)|(1<<BRP2));					  //Value for CNF1
    buff[2] = ((1 <<BTLMODE) | (1 <<SAM)  | (1<<PHSEG12) | (1<<PHSEG11) | (1<<PHSEG0) ); //Value for CNF2
    buff[3] = ((1<<PHSEG20) |(1<<PHSEG22)); //Value for CNF3 
}

/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0;					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)   | (1<<PHSEG1) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] =  (1<<PHSEG22); //Value for CNF3 
	*/

	// 20 Mhz crystal at a speed of 1 Mbit (it looks like it would work, but the transiver is not fast enough
 	/*
	timingReg[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
	timingReg[1] = 0;					  //Value for CNF1
	timingReg[2] = ((1 <<BTLMODE) | (1 <<SAM)   | (1<<PHSEG1) | (1<<PHSEG0) ); //Value for CNF2
	timingReg[3] =  (1<<PHSEG22); //Value for CNF3 
	*/

void setmc(unsigned char *buff)
{
    buff[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
    buff[1] = (( 1 << SJW1));				  //Value for CNF1
    buff[2] = ((1 <<BTLMODE)  | (1<<PHSEG11) | (1<<PHSEG1) |  (1<<PHSEG10)); //Value for CNF2
    buff[3] = (1<<PHSEG20 ); //Value for CNF3 

}

void selectNoDevice()
{
 TRISD = 0b00000000; //temp
    ECCP1AS = 0b00000010;
TRISE = 0b00000000;
	chip_enactive(0x00);
	chip_enactive(0x01);
	chip_enactive( 0x02);
	return;
}

/* Works well to read the buss, but blinks the red LED
void setmc(unsigned char *buff)
{
    buff[0] = ((1<<RX1IE)|(1<<RX0IE));       //Value for CANINTE
    buff[1] = (( 1 << SJW0));				  //Value for CNF1
    buff[2] = ((1 <<BTLMODE)  | (1<<PHSEG11) | (1<<PHSEG1) |  (1<<PHSEG10)); //Value for CNF2
    buff[3] = (1<<PHSEG20 ); //Value for CNF3 
}
*/