#include <xc.h>
#include <String.h>
#include <pic18f46k80.h>
#include <plib.h>
#include <stdlib.h>
#include <usart.h>
#include "ECAN.h"
#include "pic-config.h"




//notes
//calculators for baud etc
//http://www.nicksoft.info/el/calc/?ac=spbrg&submitted=1&mcu=+Generic+16bit+BRG&Fosc=32&FoscMul=1000000&FoscAutoSelector=0&MaxBaudRateError=1
//https://www.medo64.com/2014/05/canbus-setup/

// CONFIG1L
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define OBDTX_LED        PORTBbits.RB7
#define OBDRX_LED        PORTBbits.RB6
#define RSTX_LED         PORTBbits.RB5
#define RSRX_LED         PORTBbits.RB4
#define CANRX            PORTBbits.RB3
#define CANTX            PORTBbits.RB2
#define ISOL             PORTBbits.RB1
#define ISOK             PORTBbits.RB0
#define FCY 8000000
#define UART_BITRATE     38400
#define UART_BITRATE_ALT 115200
#define UART_READ_TIMEOUT 20000

#define _XTAL_FREQ 8000000
  
#define _XTAL_PLL (_XTAL_FREQ * 4)

CanMsg Message_Can;
unsigned char Txdata[] = "\r\nCan Testing";
unsigned int heartbeatCount;



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
        RSTX_LED ^= 1;
    }
    else
    {
        heartbeatCount = 0;


      //  RSTX_LED ^= 1;
        OBDRX_LED ^= 1;
        OBDTX_LED ^= 1;
       // __delay_ms(10);
       // OBDTX_LED ^= 1;
    
    }
}

void config_me(void){
    TRISD = 0;
    ANCON0 = 0;
    ANCON1 = 0;
    TRISA = 0b00001111;
    TRISE = 0b00000000;
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
    puts1USART("\n\r\nBRGCON1 is 0x");
    //CURR_VAL=BRGCON1;
    itoa (buff,BRGCON1,16);
    puts1USART(buff);
    puts1USART("\n\rBRGCON2 is 0x");
    itoa (buff,BRGCON2,16);
    puts1USART(buff);
    puts1USART("\n\rBRGCON3 is 0x");
    itoa (buff,BRGCON3,16);
    puts1USART(buff);
    puts1USART("\n\r");
}

void main(void) {
unsigned char config=0,spbrg=0,baudconfig=0,i=0;
  
char buffer [33];
unsigned int onetwo=0;
char key_button =0;
config_me();


CanInit();


UART1Init(115200);

itoa(buffer,set_can_speed(CAN_SPEED_500),10);
puts1USART("speed is index ");
puts1USART(buffer);
print_pic_settings();
puts1USART("\n\rPress Any Key");
//__delay_ms(10000);
//__delay_ms(10000);
while (key_button ==0)key_button= Read1USART();
//gets1USART(key_button,1);
Message_Can.ID=0x7f8;
Message_Can.DLC=8;
Message_Can.Data[0]=1;
Message_Can.Data[1]=2;
ECAN_Transmit(Message_Can);
__delay_ms(10000);

    while (1) {
      while (PIR1bits.TX1IF == 0);
     //TXREG = 76;
      puts1USART((char *)Txdata);
      ECAN_Transmit(Message_Can);
      Heartbeat();
      //printf("test %d\n",buffer);
     __delay_ms(1000);
     //TXREG = 65;
    }


    return;
}
