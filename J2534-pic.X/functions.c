/*
 * File:   functions.c
 * Author: darkspr1te
 *
 * Created on May 30, 2018, 12:48 PM
 */

//#include <pic18f46k80.h>
#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include "pic-config.h"

float ADCRead_Volts(unsigned char ch)
{
   float volts;
   if(ch>13) return 0;  //Invalid Channel
 //  ADCON0=0x00;
   ADCON0=(ch<<2);   //Select ADC Channel
    ADCON0bits.ADON=1;//switch on the adc module
    __delay_us(15);
    ADCON0bits.GODONE=1;  //Start conversion
    while(ADCON0bits.GODONE); //wait for the conversion to finish
    ADCON0bits.ADON=0;//switch off adc
 

      
  //   volts = 7 * ((float)ADCValue)/1024.0; //VCFG = 0b01 VNCFG=0b1
      volts = 5.7 * ((float)ADRES)/1024.0; //VCFG = 0b11 VNCFG=0b1
   return volts;
   //return (ADRESH << 8) | ADRESL;
}

unsigned int ADCRead_Raw(unsigned char ch)
{
   if(ch>13) return 0;  //Invalid Channel
 //  ADCON0=0x00;
   ADCON0=(ch<<2);   //Select ADC Channel
    ADCON0bits.ADON=1;//switch on the adc module
    __delay_us(15);
    ADCON0bits.GODONE=1;  //Start conversion
    while(ADCON0bits.GODONE); //wait for the conversion to finish
    ADCON0bits.ADON=0;//switch off adc
 

   return ADRES;
   //return (ADRESH << 8) | ADRESL;
}
void config_adc(void)
{
    ANCON0bits.ANSEL0 = 1; // analog input A0 on
    TRISAbits.TRISA0 = 1;//pin A0 input analog
    PORTAbits.RA0 = 0;

    ADCON1bits.TRIGSEL =0b00; // trigger select
    ADCON1bits.VCFG = 0b11;        // 11 on board V4.1, b10 onboard V2.0, 0b01 External vref ,00 AVDD 
    ADCON1bits.VNCFG =0b0; //1 external vss vref-
    ADCON1bits.CHSN =0b000; //analouge chanel select vref- 


    ADCON2bits.ADFM = 0b1;        // 0b1  10 bit ADC result is right justified, 00 left
    ADCON2bits.ACQT = 0b001; //Time select bits Tad
    ADCON2bits.ADCS = 0b100; //clock select bits 

  
    ADCON0bits.ADON = 1;        // ADC is enabled
   
}
void putch(char data) {    
     while (!TXIF)    
         continue;    
     TXREG = data;    
 } 