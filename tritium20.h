#define mcBaseAddress 0x0400
#define dcBaseAddress 0x0500

#define bootLoadAddress 0x7F0 //to 0x7FF

#define statusMes              0x0001
#define curentVoltageMes       0x0002
#define velocityMes            0x0003
#define phaseCurrentMes        0x0004
#define motorVoltageMes        0x0005
#define motorCurrentMes        0x0006
#define backEMFMes             0x0007
#define vRail_1Mes             0x0008
#define vRail_2Mes  		   0x0009
#define fanSpeedMes            0x000A
#define motorTempMes           0x000B
#define cpuAirTempMes          0x000C
#define capOutAirTempMes       0x000D
#define odometerAmpHours       0x000E

#define wheelRadius            2.23694

typedef struct mcData{
float speed;
float current;
}mcData;



