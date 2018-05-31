#include <stdio.h> //included for printf to uart function to work
#include <p18cxxx.h>
#include "can.h"
#include <stdlib.h>
//#include "functions.h"
/*********************************************************************
*
*                             Definitions 
*
*********************************************************************/
// ECAN bitrate define, choose only ONE rate
#define F_ECAN_100    0       // 1 sets ECAN module for 100kbps
#define F_ECAN_125    1       // 1 sets ECAN module for 125kbps
#define F_ECAN_500    0       // 1 sets ECAN module for 500kbps
#define F_ECAN_1000   0       // 1 sets ECAN module for 1000kbps



//Init Can Module 

void InitECAN(void)
{
    // Place CAN module in configuration mode, see CANCON register data
    CANCON = 0x80;    //REQOP bits <2:0> = 0b100
    while(!(CANSTATbits.OPMODE ==0x04));    //Wait for op-mode bits in the
                                            //CANSTAT register to = 0b100
                                            //to indicate config mode OK

    // Enter CAN module into Mode 0, standard legacy mode; see ECANCON register
    
    ECANCON = 0x00;
    
    // See Microchip application note AN754, Understanding Microchip's
    // CAN Module Bit Timing."  See also: Microchip Controller Area Network
    //(CAN) Bit Timing Calculator, available at Intrepid Control Systems:
    //www.intrepidcs.com/support/mbtime.htm.
    
    // Initialize CAN Bus bit rate timing. Assumes only four standard rates.  
    if (F_ECAN_100==1)  //  100 kbps @ 64 MHz 
    {
        BRGCON1 = 0x93; //0001 1111     //SJW=3TQ     BRP  19
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
    } 
    else if (F_ECAN_125==1) //  125 kbps @ 64 MHz
    {
        BRGCON1 = 0x8F; //0000 0111     //SJW=3TQ     BRP  15
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ 
    }
    else if (F_ECAN_500==1) //  500 kbps @ 64 MHz
    {
        BRGCON1 = 0x83; //0000 0111     //SJW=3TQ     BRP  3
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
  
    }
    else if (F_ECAN_1000==1)  //  1 Mbps (1000 kbps) @ 64 MHz
    {
          
        BRGCON1 = 0x81; //0000 0011     //SJW=3TQ     BRP  1
        BRGCON2 = 0xB8; //1011 1000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0101     //PS2  6TQ
    } 
    else                //default to 100 kbps if necessary
    {
        //  100 Kbps @ 64MHz  
        BRGCON1 = 0x93; //0001 1111     //SJW=3TQ     BRP  31
        BRGCON2 = 0xB8; //1010 0000     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ  
        BRGCON3 = 0x05; //0000 0010     //PS2  6TQ
    }

    // Initialize Receive Masks, see registers RXMxEIDH, RXMxEIDL, etc...
    // Mask 0 (M0) will accept NO extended addresses, but any standard address
    RXM0EIDH = 0x00;    // Extended Address receive acceptance mask, high byte 
    RXM0EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    RXM0SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    RXM0SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    
    // Mask 1 (M1) will accept NO extended addresses, but any standard address
    RXM1EIDH = 0x00;    // Extended Address receive acceptance mask, high byte    
    RXM1EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    RXM1SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    RXM1SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    
    // Mode 0 allows use of receiver filters RXF0 through RXF5. Enable filters
    // RXF0 and RXF1, all others disabled. See register RXFCONn.
    //  Only using two filters
    RXFCON0 = 0x03;     //Enable Filter-0 and Filter-1; disable others 
    RXFCON1 = 0x00;     //Disable Filters 8 through 15
    
    // Initialize Receive Filters
    //  Filter 0 = 0x32C0
    //  Filter 1 = 0x33C0
   
    RXF0EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF0EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    RXF0SIDH = 0x32;    //Standard Address Filter-0 high byte set to 0x32
    RXF0SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0

    RXF2EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF2EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    RXF2SIDH = 0x33;    //Standard Address Filter-0 high byte set to 0x33
    RXF2SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0
    
    // After configuring CAN module with above settings, return it
    // to Normal mode
    CANCON = 0x00;
    while(CANSTATbits.OPMODE==0x00);        //Wait for op-mode bits in the
                                            //CANSTAT register to = 0b000
                                            //to indicate Normal mode OK
    
    // Set Receiving Modes for receiver buffers 0 and 1
    RXB0CON = 0x00;     // See register RXB0CON
    RXB1CON = 0x00;     // See register RXB1CON    
}

/*********************************************************************
*
*     Function: Check the buffers to determine if they have messages
*               if so, transfer the info to the temporary-storage
*               variables. Note: Messages to receiver 0 or 1 get saved in
*               the same variables.  This id done for simplicity in
*               this example. You could save messages to separate
*               variables, or in separate arrays, if you wish. 
*
*********************************************************************/

/*
 enum CAN_SPEED {
	CAN_SPEED_33,
	CAN_SPEED_95,
	CAN_SPEED_125,
	CAN_SPEED_250,
	CAN_SPEED_500,
	CAN_SPEED_1000,
};
 */
int set_can_speed(enum CAN_SPEED speed){
    
TRISB2 = 0;
TRISB3 = 1;

CANCON = 0b10000000; //set to Configuration mode
while ((CANSTAT & 0b11100000) != 0b10000000);

BRGCON2bits.SEG2PHTS =  1; //freely programmable SEG2PH
    
    switch (speed) {
        case 0:
            break;
        case 1:
            break;
        case 2:
        BRGCON1bits.BRP    = 3;
        BRGCON2bits.PRSEG  = 2; //3 Tq
        BRGCON2bits.SEG1PH = 1; //2 Tq
        BRGCON3bits.SEG2PH = 1; //2 Tq
        BRGCON1bits.SJW    = 0; //1 Tq
            
/*
BRGCON1bits.BRP    = 1;
BRGCON2bits.PRSEG  = 6; //7 Tq
BRGCON2bits.SEG1PH = 5; //6 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 1; //2 Tq

*/
            break;
        case 3:
            BRGCON1bits.BRP    = 1;
BRGCON2bits.PRSEG  = 2; //3 Tq
BRGCON2bits.SEG1PH = 1; //2 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 0; //1 Tq
            break;
        case 4:
BRGCON1bits.BRP    = 0;
BRGCON2bits.PRSEG  = 2; //3 Tq
BRGCON2bits.SEG1PH = 1; //2 Tq
BRGCON3bits.SEG2PH = 1; //2 Tq
BRGCON1bits.SJW    = 0; //1 Tq


            break;
        case 5:
            break;
    }    
    ECANCONbits.MDSEL = 0; //Enhanced FIFO mode

CANCON = 0b00000000; //set to Normal mode
while ((CANSTAT & 0b11100000) != 0b00000000);
    return speed;
    
}


CanMsg Get_can(void)
{
    unsigned char RXMsgFlag;    // Temporary storage for message flag
    RXMsgFlag = 0x00;           // Set message flag to zero to start
    CanMsg Message_CAN_Local;
    if (RXB0CONbits.RXFUL)      // Check RXB0CON bit RXFUL to see if RX Buffer 0
                                // has received a message, if so, get the
                                // associated data from the buffer and save it.
    {
           temp_EIDH = RXB0EIDH;
        temp_EIDL = RXB0EIDL;
        temp_SIDH = RXB0SIDH;
        temp_SIDL = RXB0SIDL;
        temp_DLC =  RXB0DLC;
        temp_D0 =   RXB0D0;
        temp_D1 =   RXB0D1;
        temp_D2 =   RXB0D2;
        temp_D3 =   RXB0D3;
        temp_D4 =   RXB0D4;
        temp_D5 =   RXB0D5;
        temp_D6 =   RXB0D6;
        temp_D7 =   RXB0D7;
        
        Message_CAN_Local.ID = RXB0SIDH<< 3;
        Message_CAN_Local.ID = Message_CAN_Local.ID | RXB0SIDL;
        Message_CAN_Local.IDE = RXB0EIDH << 3 ;
        Message_CAN_Local.IDE = Message_CAN_Local.IDE | RXB0EIDL;
        Message_CAN_Local.DLC =  RXB0DLC;
        
        Message_CAN_Local.Data[0]=RXB0D0;
        Message_CAN_Local.Data[1]=RXB0D1;
        Message_CAN_Local.Data[2]=RXB0D2;
        Message_CAN_Local.Data[3]=RXB0D3;
        Message_CAN_Local.Data[4]=RXB0D4;
        Message_CAN_Local.Data[5]=RXB0D5;
        Message_CAN_Local.Data[6]=RXB0D6;
        Message_CAN_Local.Data[7]=RXB0D7;
        RXB0CONbits.RXFUL = 0;      // Reset buffer-0-full bit to show "empty"
        RXMsgFlag = 0x01;           // Set message flag to 1
    }
    else if (RXB1CONbits.RXFUL) // Check RXB1CON bit RXFUL to see if RX Buffer 1
                                // has received a message, if so, get the
                                // associated data from the buffer and save it.
    {
        temp_EIDH = RXB1EIDH;
        temp_EIDL = RXB1EIDL;
        temp_SIDH = RXB1SIDH;
        temp_SIDL = RXB1SIDL;
        temp_DLC =  RXB1DLC;
        temp_D0 =   RXB1D0;
        temp_D1 =   RXB1D1;
        temp_D2 =   RXB1D2;
        temp_D3 =   RXB1D3;
        temp_D4 =   RXB1D4;
        temp_D5 =   RXB1D5;
        temp_D6 =   RXB1D6;
        temp_D7 =   RXB1D7;
        RXB1CONbits.RXFUL = 0;      //Reset buffer-1-full bit to show "empty"
        RXMsgFlag = 0x01;           // Set message flag to 1
    }
    else if (B0CONbits.RXFUL) //CheckB0
    {
        temp_EIDH = B0EIDH;
        temp_EIDL = B0EIDL;
        temp_SIDH = B0SIDH;
        temp_SIDL = B0SIDL;
        temp_DLC = B0DLC;
        temp_D0 = B0D0;
        temp_D1 = B0D1;
        temp_D2 = B0D2;
        temp_D3 = B0D3;
        temp_D4 = B0D4;
        temp_D5 = B0D5;
        temp_D6 = B0D6;
        temp_D7 = B0D7;
        
        B0CONbits.RXFUL = 0;
        RXMsgFlag = 0x01;
    }
    
    if  (RXMsgFlag == 0x01)     // Test message flag.
                                // if message flag is a 1...
    {
        RXMsgFlag = 0x00;       // Clear the message flag
        PIR5bits.RXB1IF = 0;    // Clear the Buffer-1 interrupt bit (if used) 
      //  return TRUE;            // Return a "true" condition to the code that
        return Message_CAN_Local;
                                // called the ECAN_Receive function.
    }
    else
    {
        //return NULL;           // Otherwise, return a false condition.
    }    
}
unsigned char can_data(void)
{
   if (RXB0CONbits.RXFUL)
       return TRUE;
   else
       return FALSE;
   
  /*  if (RXB1CONbits.RXFUL)
           return TRUE;
       else
           return FALSE;*/
}

//void interrupt handleInterrupt(void) {
  //  checkCanMessageReceived();
//}

Can_Buffer Get_can_buffer(void)
{
    unsigned char RXMsgFlag;    // Temporary storage for message flag
    RXMsgFlag = 0x00;  // Set message flag to zero to start
    
    Can_Buffer Message_CAN_Local;
    Message_CAN_Local.Rx_size=0;
    if (RXB0CONbits.RXFUL)      // Check RXB0CON bit RXFUL to see if RX Buffer 0
                                // has received a message, if so, get the
                                // associated data from the buffer and save it.
    {
       /*    temp_EIDH = RXB0EIDH;
        temp_EIDL = RXB0EIDL;
        temp_SIDH = RXB0SIDH;
        temp_SIDL = RXB0SIDL;
        temp_DLC =  RXB0DLC;
        temp_D0 =   RXB0D0;
        temp_D1 =   RXB0D1;
        temp_D2 =   RXB0D2;
        temp_D3 =   RXB0D3;
        temp_D4 =   RXB0D4;
        temp_D5 =   RXB0D5;
        temp_D6 =   RXB0D6;
        temp_D7 =   RXB0D7;*/
       // Message_CAN_Local.CanBuffer1.ID=0;
        
        Message_CAN_Local.CanBuffer1.ID = RXB0SIDH<< 3;
        Message_CAN_Local.CanBuffer1.ID = Message_CAN_Local.CanBuffer1.ID | RXB0SIDL;
        Message_CAN_Local.CanBuffer1.IDE = RXB0EIDH << 3 ;
        Message_CAN_Local.CanBuffer1.IDE = Message_CAN_Local.CanBuffer1.IDE | RXB0EIDL;
       
        Message_CAN_Local.CanBuffer1.DLC =  RXB0DLC;
        
        Message_CAN_Local.CanBuffer1.Data[0]=RXB0D0;
        Message_CAN_Local.CanBuffer1.Data[1]=RXB0D1;
        Message_CAN_Local.CanBuffer1.Data[2]=RXB0D2;
        Message_CAN_Local.CanBuffer1.Data[3]=RXB0D3;
        Message_CAN_Local.CanBuffer1.Data[4]=RXB0D4;
        Message_CAN_Local.CanBuffer1.Data[5]=RXB0D5;
        Message_CAN_Local.CanBuffer1.Data[6]=RXB0D6;
        Message_CAN_Local.CanBuffer1.Data[7]=RXB0D7;
      
        RXB0CONbits.RXFUL = 0;      // Reset buffer-0-full bit to show "empty"
        RXMsgFlag = 0x01;           // Set message flag to 1
    }
    else if (RXB1CONbits.RXFUL) // Check RXB1CON bit RXFUL to see if RX Buffer 1
                                // has received a message, if so, get the
                                // associated data from the buffer and save it.
    {
        Message_CAN_Local.CanBuffer2.ID = RXB1SIDH<< 3;
        Message_CAN_Local.CanBuffer2.ID = Message_CAN_Local.CanBuffer1.ID | RXB0SIDL;
        Message_CAN_Local.CanBuffer2.IDE = RXB1EIDH << 3 ;
        Message_CAN_Local.CanBuffer2.IDE = Message_CAN_Local.CanBuffer1.IDE | RXB0EIDL;
       
        Message_CAN_Local.CanBuffer2.DLC =  RXB1DLC;
        
        Message_CAN_Local.CanBuffer2.Data[0]=RXB1D0;
        Message_CAN_Local.CanBuffer2.Data[1]=RXB1D1;
        Message_CAN_Local.CanBuffer2.Data[2]=RXB1D2;
        Message_CAN_Local.CanBuffer2.Data[3]=RXB1D3;
        Message_CAN_Local.CanBuffer2.Data[4]=RXB1D4;
        Message_CAN_Local.CanBuffer2.Data[5]=RXB1D5;
        Message_CAN_Local.CanBuffer2.Data[6]=RXB1D6;
        Message_CAN_Local.CanBuffer2.Data[7]=RXB1D7;
        RXB1CONbits.RXFUL = 0;      //Reset buffer-1-full bit to show "empty"
        RXMsgFlag = 0x02;           // Set message flag to 1
    }
    else if (B0CONbits.RXFUL) //CheckB0
    {
        Message_CAN_Local.CanBuffer0.ID = RXB0SIDH<< 3;
        Message_CAN_Local.CanBuffer0.ID = Message_CAN_Local.CanBuffer1.ID | RXB0SIDL;
        Message_CAN_Local.CanBuffer0.IDE = RXB0EIDH << 3 ;
        Message_CAN_Local.CanBuffer0.IDE = Message_CAN_Local.CanBuffer1.IDE | RXB0EIDL;
       
        Message_CAN_Local.CanBuffer0.DLC =  RXB0DLC;
        
        Message_CAN_Local.CanBuffer0.Data[0]=RXB0D0;
        Message_CAN_Local.CanBuffer0.Data[1]=RXB0D1;
        Message_CAN_Local.CanBuffer0.Data[2]=RXB0D2;
        Message_CAN_Local.CanBuffer0.Data[3]=RXB0D3;
        Message_CAN_Local.CanBuffer0.Data[4]=RXB0D4;
        Message_CAN_Local.CanBuffer0.Data[5]=RXB0D5;
        Message_CAN_Local.CanBuffer0.Data[6]=RXB0D6;
        Message_CAN_Local.CanBuffer0.Data[7]=RXB0D7;
        
        B0CONbits.RXFUL = 0;
        RXMsgFlag = 0x03;
    }
    Message_CAN_Local.Rx_size=RXMsgFlag;
    if  (RXMsgFlag >= 0x01)     // Test message flag.
                                // if message flag is a 1...
    {
        Message_CAN_Local.CanBuffer2.ID=0xff;
        RXMsgFlag = 0x00;       // Clear the message flag
        PIR5bits.RXB1IF = 0;    // Clear the Buffer-1 interrupt bit (if used) 
      //  return TRUE;            // Return a "true" condition to the code that
        //return Message_CAN_Local;
                                // called the ECAN_Receive function.
    }
    else
    {
        //return NULL;           // Otherwise, return a false condition.
        Message_CAN_Local.CanBuffer2.ID=0xaa;
    }    
    
    return Message_CAN_Local;
}


/*********************************************************************
*
*                      Transmit Sample Mesaage
*
*********************************************************************/
void can_Transmit(CanMsg Message)
{
    TXB0EIDH = 0x00;
    TXB0EIDL = 0x00;
    /*  while (TXB0CONbits.TXREQ) {
        // check the register is not in error (previous send failed)
        if (TXB0CONbits.TXERR) {
            // abort any pending transmissions now, it should clear the below flag as a result too
            CANCONbits.ABAT = 1;
        }
    }*/
    //0x35E    0110 1011 110
    //TXB0SIDH = 0x6B;
    TXB0SIDH = Message.ID >>3;
//    TXB0SIDL = 0xC0;
    TXB0SIDL = (Message.ID & 0b0111)<<5;

    TXB0DLC = Message.DLC;
 //   volatile data = &TXB0D0;
 //   int i;
   // for (i=0; i<Message.DLC; i++, data++) {
  //      data = Message.Data[i];
  //  }
    TXB0D0 = Message.Data[0];
    TXB0D1 = Message.Data[1];
    TXB0D2 = Message.Data[2];
    TXB0D3 = Message.Data[3];
    TXB0D4 = Message.Data[4];
    TXB0D5 = Message.Data[5];
    TXB0D6 = Message.Data[6];
    TXB0D7 = Message.Data[7];
    
    TXB0CONbits.TXREQ = 1; //Set the buffer to transmit

    
}


/**********************
 * 
 *Init Can system  
 * 
 * 
 * 
 */
void CanInit(void)
{
    // Switch CAN module to config mode
    CANCONbits.REQOP = 4; // Request configuration mode
    while (CANSTATbits.OPMODE != 4);// Wait until CAN modul enters to config mode

    CIOCONbits.ENDRHI = 1; // CANTX pin will drive V DD when recessive
    CIOCONbits.CLKSEL = 1; // Use the oscillator as the source of the CAN system clock

    // Switch the CAN module to legacy mode
    ECANCONbits.MDSEL = 0; // Legacy mode (Mode 0, default)

    // Set the baud rate to 125 kb/s
    BRGCON1bits.SJW = 0; // Synchronization jump width time = 2 x TQ
    BRGCON2bits.SAM = 1; // Bus line is sampled three times prior to the sample point
    BRGCON2bits.SEG2PHTS = 1; // Seg2 lenght freely programmable
    BRGCON2bits.PRSEG = 2; // Propagation time = 3 x TQ
    BRGCON2bits.SEG1PH = 1; // Phase Segment 1 time = 8 x TQ
    BRGCON3bits.SEG2PH = 1; // Phase Segment 2 time = 4 x TQ
    BRGCON1bits.BRP = 1; // TQ = (2 x 4)/FOSC > 500 ns

      // Initialize Receive Masks, see registers RXMxEIDH, RXMxEIDL, etc...
    // Mask 0 (M0) will accept NO extended addresses, but any standard address
    RXM0EIDH = 0x00;    // Extended Address receive acceptance mask, high byte 
    RXM0EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    RXM0SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    RXM0SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    
    // Mask 1 (M1) will accept NO extended addresses, but any standard address
    RXM1EIDH = 0x00;    // Extended Address receive acceptance mask, high byte    
    RXM1EIDL = 0x00;    // Extended Address receive acceptance mask, low byte
    RXM1SIDH = 0xFF;    // Standard Address receive acceptance mask, high byte
    RXM1SIDL = 0xE0;    // Standard Address receive acceptance mask, low byte
    
    // Mode 0 allows use of receiver filters RXF0 through RXF5. Enable filters
    // RXF0 and RXF1, all others disabled. See register RXFCONn.
    //  Only using two filters
    RXFCON0 = 0x03;     //Enable Filter-0 and Filter-1; disable others 
    RXFCON1 = 0x00;     //Disable Filters 8 through 15
    
    // Initialize Receive Filters
    //  Filter 0 = 0x32C0
    //  Filter 1 = 0x33C0
   
    RXF0EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF0EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    RXF0SIDH = 0x32;    //Standard Address Filter-0 high byte set to 0x32
    RXF0SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0

    RXF2EIDH = 0x00;    //Extended Address Filter-0 unused, set high byte to 0
    RXF2EIDL = 0x00;    //Extended Address Filter-0 unused, set low byte to 0
    RXF2SIDH = 0x33;    //Standard Address Filter-0 high byte set to 0x33
    RXF2SIDL = 0xC0;    //Standard Address Filter-0 low byte set to 0xC0
    
    
    
    // Switch CAN module to normal mode
    CANCONbits.REQOP = 0; // Request normal mode not loop 0b010 or config  0b100
    while (CANSTATbits.OPMODE != 0);// Wait until CAN modul enters to normal mode
       
    // Set Receiving Modes for receiver buffers 0 and 1
   
    RXB0CON = 0x00;     // See register RXB0CON
      RXB0CONbits.RXM0=0b11;
     RXB0CONbits.RXM1=0b1;
    RXB1CON = 0x00;     // See register RXB1CON    
     RXB1CONbits.RXM0=0b11;
     RXB1CONbits.RXM1=0b1;
    return;
}



/*
 typedef union {
    struct {
        unsigned FILHIT0                :1;
        unsigned JTOFF_FILHIT1          :1;
        unsigned RB0DBEN_FILHIT2        :1;
        unsigned RXRTRRO_FILHIT3        :1;
        unsigned FILHIT4                :1;
        unsigned RXM0_RTRRO             :1;
        unsigned RXM1                   :1;
        unsigned RXFUL                  :1;
    };
    struct {
        unsigned                        :1;
        unsigned JTOFF                  :1;
        unsigned RB0DBEN                :1;
        unsigned RXRTRRO                :1;
        unsigned                        :1;
        unsigned RTRRO                  :1;
    };
    struct {
        unsigned                        :1;
        unsigned FILHIT1                :1;
        unsigned FILHIT2                :1;
        unsigned FILHIT3                :1;
        unsigned                        :1;
        unsigned RXM0                   :1;
    };
    struct {
        unsigned RXB0FILHIT0            :1;
        unsigned RXB0FILHIT1            :1;
        unsigned RXB0FILHIT2            :1;
        unsigned RXB0FILHIT3            :1;
        unsigned RXB0FILHIT4            :1;
        unsigned RXB0M0                 :1;
        unsigned RXB0M1                 :1;
        unsigned RXB0FUL                :1;
    };
    struct {
        unsigned                        :3;
        unsigned RXB0RTRR0              :1;
        unsigned                        :1;
        unsigned RXB0RTRRO              :1;
    };*/