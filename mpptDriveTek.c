


#define batVoltageReachedFlag   0x10000000
#define mpptOverTemp            0x01000000
#define batNotConnected         0x00100000
#define arrayAbove26Volts       0x00010000
#define msbForTenBit		    0x00000011

typedef struct mpptData{
unsigned short currentIn;
unsigned short voltageIn;
unsigned short voltageOut;
unsigned char temp;
unsigned char status;
}mpptData;



mpptData parsMppt( unsigned char *start)
{

mpptData sampleData;
printf("\r\nhello\n");
sampleData.status=start[0];
sampleData.voltageIn = ( ((unsigned short)(start[0] & msbForTenBit))) << 8 | start[1];
sampleData.currentIn =( ((unsigned short)(start[2] & msbForTenBit)))  <<8 | start[3];
sampleData.voltageOut = ( ((unsigned short)(start[4] & msbForTenBit))) <<8 | start[5];
sampleData.temp = start[6];

printf("\n\rthe inVoltage is %i ,and the out current is %i  and the outVoltage is %i\n",sampleData.voltageIn,sampleData.currentIn,sampleData.voltageOut);
return sampleData;
}
