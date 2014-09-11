//------------------------------------------------------------------------------------
// Lab1 Part 3
//------------------------------------------------------------------------------------
//8051 Test program to demonstrate serial port I/O.  This program reads in an 8-bit
//numerical value from P1 and writes the value to P2. Potentiometers are connected 
//to Port 1 and are used to modigy the bits. The program continuosly reads in a bit
//bit from the first four pins and outputs the value to Port 2. 
//------------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------------
#include <c8051f120.h>
#include <stdio.h>
#include "putget.h"

//------------------------------------------------------------------------------------
// Global Constants
//------------------------------------------------------------------------------------
#define EXTCLK		22118400			// External oscillator frequency in Hz
#define SYSCLK		22118400			// Output of crystal oscillator
#define BAUDRATE	28800				// UART baud rate in bps

//------------------------------------------------------------------------------------
// Function Prototypes
//------------------------------------------------------------------------------------
void main(void);
void SYSCLK_INIT(void);
void PORT_INIT(void);
void UART0_INIT(void);

//------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//------------------------------------------------------------------------------------
/* P1 0x90 */
__sbit __at 0x90 P1_0;   // Input, associated with Port 1 Pin 0
__sbit __at 0x91 P1_1;   // Input, associated with Port 1 Pin 1
__sbit __at 0x92 P1_2;   // Input, associated with Port 1 Pin 2
__sbit __at 0x93 P1_3;   // Input, associated with Port 1 Pin 3
__sbit __at 0x94 P1_4;   // Input, associated with Port 1 Pin 4
__sbit __at 0x95 P1_5;   // Input, associated with Port 1 Pin 5
__sbit __at 0x96 P1_6;   // Input, associated with Port 1 Pin 6
__sbit __at 0x97 P1_7;   // Input, associated with Port 1 Pin 7

/* P2 0xA0 */
__sbit __at 0xA0 P2_0; // Output, associated with Port 2 Pin 0
__sbit __at 0xA1 P2_1; // Output, associated with Port 2 Pin 1
__sbit __at 0xA2 P2_2; // Output, associated with Port 2 Pin 2
__sbit __at 0xA3 P2_3; // Output, associated with Port 2 Pin 3
__sbit __at 0xA4 P2_4; // Output, associated with Port 2 Pin 4
__sbit __at 0xA5 P2_5; // Output, associated with Port 2 Pin 5
__sbit __at 0xA6 P2_6; // Output, associated with Port 2 Pin 6
__sbit __at 0xA7 P2_7; // Output, associated with Port 2 Pin 7

//------------------------------------------------------------------------------------
// MAIN Routine
//------------------------------------------------------------------------------------
void main(void)
{
	char choice;
	
	WDTCN = 0xDE;						// Disable the watchdog timer
	WDTCN = 0xAD;						// Note: = "DEAD"!
	
	SYSCLK_INIT();						// Initialize the oscillator
	PORT_INIT();						// Initialize the Crossbar and GPIO
	UART0_INIT();						// Initialize UART0

	SFRPAGE = UART0_PAGE;				// Direct output to UART0

	printf("\033[2J");					// Erase ANSI terminal & move cursor to home position
	printf("Part 3");
	
	while(1)
	{
		//Read value 8 bit number for Port1 and Write value to Port2
		P2_0 = P1_0; 
		P2_1 = P1_1;
		P2_2 = P1_2;
		P2_3 = P1_3;
		P2_4 = P1_4;
		P2_5 = P1_5;
		P2_6 = P1_6;
		P2_7 = P1_7;
	}
}

//------------------------------------------------------------------------------------
// SYSCLK_Init
//------------------------------------------------------------------------------------
//
// Initialize the system clock to use a 22.1184MHz crystal as its clock source
//
void SYSCLK_INIT(void)
{
	int i;
	char SFRPAGE_SAVE;

	SFRPAGE_SAVE = SFRPAGE;				// Save Current SFR page	SFRPAGE = CONFIG_PAGE;
    SFRPAGE   = CONFIG_PAGE;
	
	OSCXCN = 0x67;						// Start ext osc with 22.1184MHz crystal
	for(i=0; i < 3000; i++);			// Wait for the oscillator to start up
	while(!(OSCXCN & 0x80));
	CLKSEL = 0x01;						// Switch to the external crystal oscillator
	OSCICN = 0x00;						// Disable the internal oscillator

	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//------------------------------------------------------------------------------------
// PORT_Init
//------------------------------------------------------------------------------------
//
// Configure the Crossbar and GPIO ports
//
void PORT_INIT(void)
{	
	char SFRPAGE_SAVE;

	SFRPAGE_SAVE = SFRPAGE;				// Save Current SFR page
	SFRPAGE = CONFIG_PAGE;
	
	XBR0	 = 0x04;					// Enable UART0
	XBR1	 = 0x00;
	XBR2	 = 0x40;					// Enable Crossbar and weak pull-up
	P0MDOUT |= 0x01;					// Set TX0 on P0.0 pin to push-pull
	P1MDOUT	|= 0x40;					// Set green LED ooutput P1.6 to push-pull
	P2MDOUT |= 0xFF;					// Set port 2 to output
	
	P1MDOUT &= 0x00;					//set all pins of port 1 as analog input
	P1 |= ~0x00;

	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//------------------------------------------------------------------------------------
// UART0_Init
//------------------------------------------------------------------------------------
//
// Configure the UART0 using Timer1, for <baudrate> and 8-N-1
//
void UART0_INIT(void)
{
	char SFRPAGE_SAVE;

	SFRPAGE_SAVE = SFRPAGE;				// Save Current SFR page
	SFRPAGE = TIMER01_PAGE;

	TCON	 = 0x40;
	TMOD	&= 0x0F;
	TMOD	|= 0x20;					// Timer1, Mode 2, 8-bit reload
	CKCON	|= 0x10;					// Timer1 uses SYSCLK as time base
//	TH1		 = 256 - SYSCLK/(BAUDRATE*32)  Set Timer1 reload baudrate value T1 Hi Byte
	TH1		 = 0xE8;					// 0xE8 = 232
	TR1		 = 1;						// Start Timer1

	SFRPAGE = UART0_PAGE;
	SCON0	 = 0x50;					// Mode 1, 8-bit UART, enable RX
	SSTA0	 = 0x00;					// SMOD0 = 0, in this mode
										// TH1 = 256 - SYSCLK/(baud rate * 32)

	TI0 = 1;							// Indicate TX0 ready
	
	SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


