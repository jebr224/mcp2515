char bit_is_set( unsigned char memory, unsigned char bit);
 
 void chip_active(unsigned char device);
 void chip_enactive(unsigned char device);
 
 unsigned char  mcp_read_register(unsigned char adress,unsigned char device);
typedef struct tagstCanFrame{
//	unsigned char id[2];
	unsigned short id;
	unsigned char control;
	unsigned char length;
	unsigned char rtr;
	unsigned char data[8];
	}stCanFrame;

char mcp2515_read_status(char type,unsigned char device);
 void mcp_write_adress(unsigned char adress , unsigned char value,unsigned char device);
 void mcp2515_bit_modify(unsigned char adress, unsigned char mask, unsigned char data,unsigned char device);

 
 void mcp2515_normal(unsigned char device);
 void mcp2515_sleep(unsigned char device);
 void mcp2515_wakeUp(unsigned char device);
 void mcp2515_listen(unsigned char device); 
 void mcp2515_loopBack(unsigned char device);
 void mcp2515_config(unsigned char device);
 void mcp2515_oneShot(unsigned char device);

 void inputFiltersOff(unsigned char device);
 
 int  mcp_init(unsigned char device,unsigned char * canConfig);
 char mcp2515_get_message( stCanFrame * inMessage,unsigned char device);
 unsigned char mcp2515_send_message(stCanFrame *message, unsigned char reg,unsigned char device);
unsigned char mcp2515_send_ex_message(stCanFrame *message, unsigned char reg,unsigned char device);
 
void debugReg(void);
void dispError(unsigned char device);
void delay(void);


void setmppt( unsigned char *buff);

void setmc(unsigned char *mcCanConfig);

//void getMessagesThatLookLike(char * prt,unsigned chardevice);

