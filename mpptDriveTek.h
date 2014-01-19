#define batVoltageReachedFlag   0b10000000
#define mpptOverTemp            0b01000000
#define batNotConnected         0b00100000
#define arrayAbove26Volts       0b00010000
#define msbForTenBit		    0b00000011

#define MPPTRequestID           0x710
#define MPPTAnswerID            0x770

#define VIN_driveTek_ADC_COUNT_TO_VOLTS  (6.5587)  
#define VOUT_driveTek_ADC_COUNT_TO_VOLTS  (4.7251)

typedef struct mpptData{
unsigned short currentIn;
unsigned short voltageIn;
unsigned short voltageOut;
unsigned char temp;
unsigned char status;
}mpptData;

