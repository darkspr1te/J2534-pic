

#ifndef _CAN_H
#define _CAN_H


/*********************************************************************
*
*                             Defines 
*
*********************************************************************/
#define TRUE    1
#define FALSE   0


struct can_speed_info {
    const unsigned int btr;
};


typedef struct
{
	long int ID;		// CAN ID
	unsigned int IDE;		// CAN_ID_STD for standard and CAN_ID_EXT for extended
	unsigned int RTR;
	unsigned DLC;
	unsigned Data[8];
	unsigned int FMI;
} CanMsg;

typedef struct
{
    int Rx_size;
    CanMsg CanBuffer1;
    CanMsg CanBuffer2;
    CanMsg CanBuffer0;
} Can_Buffer ;

enum CAN_SPEED {
	CAN_SPEED_33,
	CAN_SPEED_95,
	CAN_SPEED_125,
	CAN_SPEED_250,
	CAN_SPEED_500,
	CAN_SPEED_1000,
};

unsigned char temp_EIDH;    //Extended Identifier, high byte
unsigned char temp_EIDL;    //Extended Identifier, low byte
unsigned char temp_SIDH;    //Standard Identifier, high byte
unsigned char temp_SIDL;    //Standard Identifier, low byte
unsigned char temp_DLC;     //Data Length Control value, 0 to 8
unsigned char temp_D0;      //Data byte 0 through...
unsigned char temp_D1;
unsigned char temp_D2;
unsigned char temp_D3;
unsigned char temp_D4;
unsigned char temp_D5;
unsigned char temp_D6;
unsigned char temp_D7;      //Data byte 7


/*********************************************************************
*
*                        Function Prototypes 
*
*********************************************************************/
void InitECAN(void);

unsigned char can_data(void);
Can_Buffer Get_can_buffer(void);
int set_can_speed(enum CAN_SPEED);
void CanInit(void);
void can_Transmit(CanMsg Message);
CanMsg Get_can(void);
//void checkCanMessageReceived();
//Can_Buffer Get_can_buffer(void);

#endif