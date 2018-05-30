/*
MCLR (pin 1) MCLR/V PP /RE3
Vmeasure (pin 2) RA0
J1850 Volts (pin 3)RA1
J1850 Bus+ (pin 4)RA2
Memory (pin 5)RA3
Baud Rate (pin 6)RA4
LFmode (pin 7)RA5
VSS (pins 8 and 19)V SS
XT1 (pin 9) and RA7
XT2 (pin 10)RA6
VPW In (pin 11)RC0
ISO In (pin 12)RC1
PWM In (pin 13)RC2
J1850 Bus- (pin 14)RC3

RTS (pin 15)RC4
Busy (pin 16)RC5
RS232UartTx (pin 17)RC6
RS232UartRx (pin 18)RC7
VSS (pin 19)V SS
VDD (pin 20)V DD
ISO K (pin 21)  RB0
ISO L (pin 22)RB1
CAN Tx (pin 23) RB2
CAN Rx (pin 24)RB3
RS232 Rx LED (pin 25), RB4
RS232 Tx LED (pin 26),RB5
OBD Rx LED (pin 27) RB6/PGC
OBD Tx LED (pin 28)RB7/PGD



PORTB, BIT4 (LED-RS232-CMD OFF)
PORTB, BIT5 (LED-WORK OFF)
PORTB, BIT7 (LED-CAN-TX OFF)
PORTB, BIT6 (LED-CAN-RX OFF)
*/
//These are tied into the LED's, LOW is LED on
#define OBDTX_LED        PORTBbits.RB7
#define OBDRX_LED        PORTBbits.RB6
#define RSTX_LED         PORTBbits.RB5
#define RSRX_LED         PORTBbits.RB4

//While CAN pins is defined here it's not actually used in code
#define CANRX_PIN            PORTBbits.RB3
#define CANTX_PIN            PORTBbits.RB2

#define ISOL_PIN             PORTBbits.RB1
#define ISOK_PIN             PORTBbits.RB0
#define ISORX_PIN            PORTCbits.RC1

#define RTS_PIN              PORTCbits.RC4
#define BUSY_PIN             PORTCbits.RC5

#define J1850_Plus          PORTCbits.RC0
#define J1850_Neg           PORTCbits.RC2
#define J1850_TX            PORTCbits.RC3

