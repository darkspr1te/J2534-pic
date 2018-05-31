#include <xc.h>
#include <String.h>
//#include <pic18f46k80.h>
#include <p18f25k80.h>
#include <p18cxxx.h>
#include <plib.h>
#include <stdlib.h>
#include <usart.h>
#include "can.h"
#include "pic-config.h"
#include "pins.h"
#include <stdio.h>
#include "functions.h"
#include "startup.h"

//notes
//calculators for baud etc
//http://www.nicksoft.info/el/calc/?ac=spbrg&submitted=1&mcu=+Generic+16bit+BRG&Fosc=32&FoscMul=1000000&FoscAutoSelector=0&MaxBaudRateError=1
//https://www.medo64.com/2014/05/canbus-setup/



#define FCY 8000000
#define UART_BITRATE     38400
#define UART_BITRATE_ALT 115200
#define UART_READ_TIMEOUT 20000



CanMsg Message_Can;
CanMsg RX_CAN[8];
unsigned char Txdata[] = "\r\nCan Testing";
unsigned int heartbeatCount;



unsigned int ADCValue = 0;
unsigned char ADCStringVal[4];
unsigned char ADCStringValTwo[4];
unsigned char stringval[10];
int receivedData = FALSE;

void CanTestMessage(void)
{
    // while (TXB1CONbits.TXREQ != 0); // Wait for the CAN TX buffer to be ready

    // Set the message SID
    TXB1SIDH = 0xFF; // SID 0xFFF
    TXB1SIDL = 0xF0; // Standard SID

    // Set the message data registers
    TXB1D0 = 0x00;
    TXB1D1 = 0x01;
    TXB1D2 = 0x02;
    TXB1D3 = 0x03;
    TXB1D4 = 0x04;
    TXB1D5 = 0x05;
    TXB1D6 = 0x06;
    TXB1D7 = 0x07;

    // Set the DLC register
    TXB1DLC = 0x08; // Transmitted message will have the TXRTR bit cleared
                                    // Data length = 8 bytes

    TXB1CONbits.TXREQ = 1; // Request the data transmission

    while (TXB1CONbits.TXREQ != 0); // Wait for the CAN TX buffer to be ready
    
    return;
}

void Heartbeat(void)
{
    // Toggle LED every 256th time this function is called
    if (heartbeatCount < 10)
    {
        heartbeatCount++;
  //      printf("HB\n\r");
        RSRX_LED ^= 1;
      //  RSTX_LED ^= 1;
    }
    else
    {
        heartbeatCount = 0;


      //  RSTX_LED ^= 1;
      //  OBDRX_LED ^= 1;
       // OBDTX_LED ^= 1;
       // __delay_ms(10);
       // OBDTX_LED ^= 1;
    
    }
}

void config_me(void){
  //  TRISD = 0;
    ANCON0 = 0;
    ANCON1 = 0;
    TRISA = 0b00001111;
   // TRISE = 0b00000000;
    TRISB = 0;
    TRISCbits.TRISC6 = 1; // set in datasheet
    TRISCbits.TRISC7 = 1; // set in datasheet
    
    
    SPBRG1=34;  // 4 MHz oscilator with PLL x4 = 16MHZ
    SPBRGH1=0;
    TXSTA1bits.BRGH = 1;  // high speed uart // http://www.nicksoft.info/el/calc/?ac=spbrg

    BAUDCON1bits.BRG16 = 1;// 8 bit data
    TXSTA1bits.TX9 = 0;
    TXSTA1bits.SYNC = 0;  // asynchronus mode

    RCSTA1bits.SPEN = 1;  // enable serial port
    INTCONbits.GIE = 0;  // set off interrupt
    TXSTA1bits.TXEN = 1; //  transmitter is enabled

}
/*
void can_change_250(void){
//setup pins
TRISB2 = 0;
TRISB3 = 1;

CANCON = 0b10000000; //set to Configuration mode
while ((CANSTAT & 0b11100000) != 0b10000000);

BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH

BRGCON1bits.BRP    = 1;
BRGCON2bits.PRSEG  = 2; //3 Tq
BRGCON2bits.SEG1PH = 1; //2 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 0; //1 Tq

ECANCONbits.MDSEL = 2; //Enhanced FIFO mode

CANCON = 0b00000000; //set to Normal mode
while ((CANSTAT & 0b11100000) != 0b00000000);
}

void can_change_125(){
    //setup pins
TRISB2 = 0;
TRISB3 = 1;

CANCON = 0b10000000; //set to Configuration mode
while ((CANSTAT & 0b11100000) != 0b10000000);

BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH

BRGCON1bits.BRP    = 3;
BRGCON2bits.PRSEG  = 2; //3 Tq
BRGCON2bits.SEG1PH = 1; //2 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 0; //1 Tq
ECANCONbits.MDSEL = 2; //Enhanced FIFO mode

CANCON = 0b00000000; //set to Normal mode
while ((CANSTAT & 0b11100000) != 0b00000000);
}

void can_change_500(void){
    
//setup pins
TRISB2 = 0;
TRISB3 = 1;

CANCON = 0b10000000; //set to Configuration mode
while ((CANSTAT & 0b11100000) != 0b10000000);

BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH

BRGCON1bits.BRP    = 0;
BRGCON2bits.PRSEG  = 2; //3 Tq
BRGCON2bits.SEG1PH = 1; //2 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 0; //1 Tq

ECANCONbits.MDSEL = 2; //Enhanced FIFO mode

CANCON = 0b00000000; //set to Normal mode
while ((CANSTAT & 0b11100000) != 0b00000000);
}
*/
void UART1Init(long baud_rate){
    RCSTA1bits.SPEN = 1;	// enable port
    TRISCbits.TRISC7 = 1;	// make rx pin an input
    RCSTA1bits.CREN = 1;	// enable receive
    TRISCbits.TRISC6 = 0;	// make tx pin an output
    TXSTA1bits.TXEN = 1;	// enable transmit
    TXSTA1bits.SYNC = 0;	// use async serial
    TXSTA1bits.BRGH = 1;	// high speed mode
    BAUDCON1bits.BRG16 = 1;	// use 16 bit baud rate generator
    SPBRG1 = (_XTAL_PLL/baud_rate/4)-1;	// set baud rate generator
    return;
}
 
void print_pic_settings(void)
{
    //unsigned int CURR_VAL=0;
    char buff [33];
    printf("\n\r\nBRGCON1 is 0x");
    //CURR_VAL=BRGCON1;
    itoa (buff,BRGCON1,16);
    printf(buff);
    printf("\n\rBRGCON2 is 0x");
    itoa (buff,BRGCON2,16);
    printf(buff);
    printf("\n\rBRGCON3 is 0x");
    itoa (buff,BRGCON3,16);
    printf(buff);
    printf("\n\r");
}


void checkCanMessageReceived(void) {
    CanMsg local_msg;
    PORTBbits.RB6 ^= 1;//led2
    printf("part1");
    printf("\n\rRXB0IF %d", PIR5bits.RXB0IF);
    printf("\n\rRXB0IE %d", PIE5bits.RXB0IE);
    // check if CAN receive buffer 0 interrupt is enabled and interrupt flag set
    if (PIE5bits.RXB0IE && PIR5bits.RXB0IF) {
        printf("part2");
        // now confirm the buffer 0 is full and take directly 2 bytes of data from there - should be always present
        if (RXB0CONbits.RXFUL) {
            
            // we can only receive normal and complex messages, so we need to know the canID
          //  CanHeader header = can_idToHeader(&RXB0SIDH, &RXB0SIDL);
            local_msg.ID =RXB0SIDH<< 3;
            local_msg.ID = local_msg.ID | RXB0SIDL;
            if (local_msg.ID == 0x7f8)printf("\n\rcan match\n\r");
            printf("\n\rcan id %4x",local_msg.ID);
            local_msg.DLC = RXB0DLC;
            local_msg.Data[0] =RXB0D0 ;
            local_msg.Data[1] =RXB0D1 ;
            local_msg.Data[2] =RXB0D2 ;
            local_msg.Data[3] =RXB0D3 ;
            local_msg.Data[4] =RXB0D4 ;
            local_msg.Data[5] =RXB0D5 ;
            local_msg.Data[6] =RXB0D6 ;
            local_msg.Data[7] =RXB0D7 ;
            printf("\n\rcan DLC %4x",local_msg.DLC);
            printf("\n\rcan d0 %4x",local_msg.Data[0]);
            printf("\n\rcan d1 %4x",local_msg.Data[1]);
            printf("\n\rcan d2 %4x",local_msg.Data[2]);
            printf("\n\rcan d3 %4x",local_msg.Data[3]);
            printf("\n\rcan d4 %4x",local_msg.Data[4]);
            printf("\n\rcan d5 %4x",local_msg.Data[5]);
            printf("\n\rcan d6 %4x",local_msg.Data[6]);
            printf("\n\rcan d7 %4x",local_msg.Data[7]);
            // also ignore highest bit of node ID = floor
          //  receivedNodeID = header.nodeID & MAX_7_BITS;
            // also take the 1st byte of data
         //   receivedDataByte = RXB0D0;
            // the main thread will process this message then
            receivedData = TRUE;
            
              RXB0CONbits.RXFUL = 0; // mark the data in buffer as read and no longer needed
        }
        PIR5bits.RXB0IF = 0; // clear the interrupt
        
    }   
    //PIR5 = 0; //PIE5bits.RXB1IE = 0;
   // printf("test");
    //print_can_buffer(local_msg);
   // RXB1CONbits.RXFUL = 0;
}


void print_can_buffer(Can_Buffer buffer)
{
    printf("\n\r");
    printf("[%04x]", buffer.CanBuffer1.ID);
    printf("{%02x}", buffer.CanBuffer1.DLC);
    printf("");
    for (int i=0;i<=7;i++)printf("%02x:", buffer.CanBuffer1.Data[i]);
    printf("\n\r");
    printf("[%04x]", buffer.CanBuffer2.ID);
    printf("{%02x}", buffer.CanBuffer2.DLC);
    printf("");
    for (int i=0;i<=7;i++)printf("%02x:", buffer.CanBuffer1.Data[i]);
       printf("\n\r");
  /*  printf("[%04x]", buffer.CanBuffer0.ID);
    printf("{%02x}", buffer.CanBuffer0.DLC);
    printf("");
    for (int i=0;i<=7;i++)printf("%02x:", buffer.CanBuffer1.Data[i]);
    printf("\n\r");*/
}

void main(void) {
unsigned char config=0,spbrg=0,baudconfig=0,i=0;
  
char buffer [33];
unsigned int onetwo=0;
char key_button =0;
char error=0;
Can_Buffer messages_in_a_can;


config_me();//config cpu in general
__delay_ms(10);//delay too reduce random chars to the FTDI UART
config_adc();//setup adc 0 for PIN 16 (12V+) reading
CanInit();// setup can no filters

startUp_device();

UART1Init(115200);//setup and init uart1 
OBDRX_LED=1;
OBDTX_LED=1;
error=set_can_speed(CAN_SPEED_125);//set can speed


printf("\n\rPress Any Key\n\r");
while (key_button ==0)key_button= Read1USART();//wait for any data from uart

OBDRX_LED =1;
OBDTX_LED =1;
RSTX_LED = 1;
//fake can message for testing
        
Message_Can.ID=0x7df;
Message_Can.DLC=7;
for (int b=0;b<=7;b++){Message_Can.Data[b]=(b<<2);}
Message_Can.Data[1]=2;
//can_Transmit(Message_Can);
key_button=0;
//Mem_CAN.ID=0;
//if (can_data() == TRUE){printf("\n\rMessage received");while (key_button ==0)key_button= Read1USART();}
//__delay_ms(10000);

    while (1) {
      while (PIR1bits.TX1IF == 0);
     //TXREG = 76;
     // printf((char *)Txdata);
      
            
      __delay_ms(10);
      if (PIE5bits.RXB0IE && PIR5bits.RXB0IF) {
      if (RXB0CONbits.RXFUL){messages_in_a_can = Get_can_buffer();printf("\n\rRX Size %d\n\r",messages_in_a_can.Rx_size);print_can_buffer(messages_in_a_can);RXB0CONbits.RXFUL = 0;}
      }
      /*
      if (can_data() == TRUE)
        {
          printf("\n\rMessage received");
          //CanMsg Get_can();
          memset(RX_CAN,0,sizeof(RX_CAN));
          RX_CAN[1]=Get_can();
          printf("\n\r");
          printf("Can ID %04x\n\r", RX_CAN[1].ID);
          printf("Can DLC %04x\n\r", RX_CAN[1].DLC);
          
          printf("Can Data 0 %04x\n\r", RX_CAN[1].Data[0]);
          printf("Can Data 1 %04x\n\r", RX_CAN[1].Data[1]);
          printf("Can Data 2 %04x\n\r", RX_CAN[1].Data[2]);
          printf("Can Data 3 %04x\n\r", RX_CAN[1].Data[3]);
          printf("Can Data 4 %04x\n\r", RX_CAN[1].Data[4]);
          printf("Can Data 5 %04x\n\r", RX_CAN[1].Data[5]);
          printf("Can Data 6 %04x\n\r", RX_CAN[1].Data[6]);
          printf("Can Data 7 %04x\n\r", RX_CAN[1].Data[7]);

      }*/
      Heartbeat();//so we know we not stuck
       printf("\n\rVoltage V%6.2f\r\n", ADCRead_Volts(0));
       
       key_button=0;
       if (DataRdy1USART()==0){}else key_button= Read1USART();
       if (key_button>=1)printf("%d",key_button);
       if (key_button==108){printf("can sent");can_Transmit(Message_Can);}
       if (receivedData){
           key_button=0;
           for (int o=0;o>=10;o++)
           printf("\n\rPress Any Key\n\r");
           while (DataRdy1USART()==0){key_button= Read1USART();}
           
            //while (key_button ==0)key_button= Read1USART();
           printf("\n\rdata\n\r");
            receivedData=FALSE;
       }
     //__delay_ms(10);
       printf("\n\rPress Any Key\n\r");
       char newbuf[10];
       gets1USART (&newbuf, 1);
       printf("key %c",newbuf);
       if (newbuf[0]=='l'){printf("can sent");can_Transmit(Message_Can);}
     for (int c = 0;c <= 200;c++)__delay_ms(10);
     //TXREG = 65;
    }


    return;
}
