
#include <p18cxxx.h>


#include "startup.h"
#include "can.h"
#include <stdlib.h>
#include "main.h"
#include <stdio.h>

    //I was planning on using a crystal, but this pretty
	// Set the internal oscillator to 64MHz
	//    OSCCONbits.IRCF = 7;
	//    OSCTUNEbits.PLLEN = 1;
    // Initialize global variables to 0


//***************************************************
//High ISR 
//#pragma interrupt high_isr 
void interrupt high_priority high_isr(void){
    int CANCON_Temp=0;
    int CANSTAT_Temp=0;
    CANCON_Temp=CANCON;
    CANSTAT_Temp=CANSTAT;
    if (CANCON_Temp &= 0b0000001){TXREG = '6';}
    if (CANCON_Temp &= 0b0000010){TXREG = '7';}
    if (CANCON_Temp &= 0b0000100){TXREG = '8';}
    
	//LATBbits.LATB3 ^= 1;       //Toggle portB pin 3 (red LED)
	//parse message and update values
	//INTCON = 0b01100100;//global interrupt bit off (when we are done with config we will turn on), 
    if (PIR5bits.IRXIF == 1)TXREG = 'A';
    if (PIR5bits.WAKIF == 1)TXREG = 'B';
    if (PIR5bits.ERRIF == 1)TXREG = 'C';
    if (PIR5bits.TXB2IF == 1)TXREG = 'D';
    if (PIR5bits.TXB1IF == 1)TXREG = 'E';
    if (PIR5bits.RXB1IF == 1){TXREG = 'F';checkCanMessageReceived();}
    if (PIR5bits.RXB0IF == 1){TXREG = 'G';checkCanMessageReceived();}
	PIR5 &= 0b01111100; //clear the interrupt flag so that another interrupt can happen
    PIR5=0;
 //   PIE5 &= 0b11111100; 
//    PIR4 = 0b0; //clear the interrupt flag so that another interrupt can happen
 //   PIR3 = 0b0; //clear the interrupt flag so that another interrupt can happen
 //   PIR2 = 0b00; //clear the interrupt flag so that another interrupt can happen
  //  PIR1 = 0b00; //clear the interrupt flag so that another interrupt can happen
  
  //  PIR1 &= 0b111111100; //clear the interrupt flag so that another interrupt can happen
	PORTBbits.RB7 ^= 1;//led3
    
    //TXREG = 'A';
    
    
   // INTCON = 0b11100100;//global interrupt bit off (when we are done with config we will turn on), 
   // return;
}


void low_priority  interrupt  low_isr(void){
	//LATBbits.LATB3 ^= 1;       //Toggle portB pin 3 (red LED)
	//segUpDate
   // PORTBbits.RB6 ^= 1;
  
    PORTBbits.RB5 ^= 1;//led4
    //TXREG = 'Q';
  //  printf("\n\rpir1 %d\n\r",PIR1);
	//PIR1 &= 0b1011011;///clear the interrupt flag (TM0)so that another interrupt can happen
  /*  PIR1 =0;
    printf("pir2 %d\n\r",PIR2);
    PIR2=0;
    printf("pir3 %d\n\r",PIR3);
    PIR3=0;
    printf("pir4 %d\n\r",PIR4);
    PIR4=0;*/
    printf("\n\rCAN %d",CANSTAT);
    printf("\n\rCAN %d",CANCON);
    printf("\n\rPIR5 %d\n\r",PIR5);
   // PIR5=0;
   // INTCONbits.T0IF=0;
    checkCanMessageReceived();
    //if (PIR5==3)PIR5=0;
    //reg = PIR1;
   // printf("pir1 %d\n\r",reg);   
   // PIR5 &= 0b1111100;///clear the interrupt flag (TM0)so that another interrupt can happen
    
   // printf("pir 5 %d\n\r",PIR5);
}



	



void startUp_interrupts(void){
	
	//high-priority interrupt vector is at 0x0008
	//low-priority  interrupt vector is at 0x0018

	INTCON = 0b01100100;//global interrupt bit off (when we are done with config we will turn on), 
						//peripheral interrupt on,
						//both timer interrupt on
						//hardware interrupt off,
	INTCON2 = 0b1000100;
	INTCON3 = 0;// no external interupts

	//PIR1

	PIE1 = 0b00000000; //TMR1 overflow is enabled
	PIE2 = 0;
	PIE3 = 0;
    
	PIE4 = 0;
	PIE5 = 0b00000001; //RXB1IE, and RXB0IE 
    PIE5bits.RXB0IE = 1;

	IPR1 = 0b00000000; //TMR is low priority; 0b0000000x where x is the bit that matters
	IPR2 = 0;
	IPR3 = 0;
	IPR4 = 0;
	IPR5 = 0b00000000;//RXB1IE, and RXB0IE  are high priority


	RCONbits.IPEN =1;//reset reg
	//IPEN=enable 2 priority levels
    INTCON = 0b11000100;
    INTCONbits.GIEH =1;
    INTCONbits.GIEL =1;
	return;
}

void startUp_timer(void){
	//we are using timer1 overflow  
	//each digit must have a freq  of at least 50 Hz, we have 6 digits so a min freq is 50*6=300
	// (foce / prescaler) =  24mHz /1 = 24 Mhz
    //(clk to over flow  = 24Mhz / 2^16 = 366.21 hz
	T1CON = 0b01001111;
			//TMR1CS 01
			//T1CKS  00 (1:1) 
			//SOSCEN 1 (I think?)
			//T1SYNC 1  
			//RD16   1
			//TMR1ON 1

	T1GCON = 0b000000000;
			//tmr1ge  0
			//t1gpol =0
			//t1gtm 0
			//t1gsmp 0
			//t1ggo 0
			//t1gcal 0
			//t1gss 00

	//24Mzh/8 = 3mHz
	//clk to overflow = 3/2^8
 


}
void startUp_GPIO(void){

    //portA setup
		TRISA  = 0;	//Set all port A pins to output
		LATA   = 0; //set output low
		ANCON0 = 0; //set pins to digital mode
	
   //portB setup
		TRISB  = 0; //Set all port B pins to output
		LATB   = 0; //Set output low
		ANCON0 = 0; //I think I need this
		ANCON1 = 0; //set it all to digital
		ODCON  = 0; //Open-drain capability is disabled 

	//portC setup
		TRISC = 0; //Set all port C pins to output
		LATC  = 0; //Set output low

	return;
}



void startUp_OSCILLATOR(void){
	//external osc freq will be 6Mhz
	//4X pll, mean system freq will be 24 Mhz
	
	OSCCON = 0b11111000;
			//pg 53
			//IDLEN 1
			//IRCF 111
			//OSTS 1
			//HFIOFS 0
			//scs 00
	OSCTUNE = 0b1100000;
			//intsrc 1
			//pllen 1
			//tun 0

	return;
}



void startUp_device(void){

	startUp_interrupts();
	//startUp_OSCILLATOR();
	//startUp_timer();
	//startUp_GPIO();
	//startUp_ECAN();
  
    
	return;

}


