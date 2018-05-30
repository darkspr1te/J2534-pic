### J2534-pic project 

This project is using a clone elm327 device with a pic18f25k80 MCU, in it's default config you can't get CAN 500 to function from 4MHZ X-TAL (AFAIK)
So I've replaced the 4MHZ defualt X-TAL with a 8MHZ from the scraps bin, quick hack i know and wont fix all the other devices out there, pity but lets get 
codebase working, i have the orig 4mhz so i can try getting 500Kbps working another time.


This project is also a sister project to the OM127-reboot project 

OM127-reboot:-[URL](https://github.com/darkspr1te/OM127-reboot.git)


```
CPU:PIC18F25K80
CAN IC:MCP2551
K-Line : Transistor
```
### Warning: Using this software on your device will erase OEM bootloader+firmware and you will no longer be able to use it in it's original application

### Still TODO:

- [ ] Add J2534 responses
- [ ] Add protocols for :-
  - [ ] 1	SAE J1850 PWM (41.6 kbaud)
  - [ ] 2	SAE J1850 VPW (10.4 kbaud)
  - [ ] 3	ISO 9141-2 (5 baud init, 10.4 kbaud)
  - [ ] 4	ISO 14230-4 KWP (5 baud init, 10.4 kbaud)
  - [ ] 5	ISO 14230-4 KWP (fast init, 10.4 kbaud)
  - [x] 6	ISO 15765-4 CAN (11 bit ID, 500 kbaud)
  - [x] 7	ISO 15765-4 CAN (29 bit ID, 500 kbaud)
  - [x] 8	ISO 15765-4 CAN (11 bit ID, 250 kbaud)  
  - [x] 9	ISO 15765-4 CAN (29 bit ID, 250 kbaud)  
- [x] CAN Sniffer.

am currently using the ecan example files,now that i've fixed can speed and config i'll drop the ecan usage


```
X-TAL Speed is 8MHZ
PLL = X-TAL * 4

```

### Some handy resources
Baud rate calculator [URL](http://www.nicksoft.info/el/calc/?ac=spbrg&submitted=1&mcu=+Generic+16bit+BRG&Fosc=32&FoscMul=1000000&FoscAutoSelector=0&MaxBaudRateError=1)

CanBUS calculator [URL](https://www.medo64.com/2014/05/canbus-setup/)

