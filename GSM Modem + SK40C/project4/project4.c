 /*******************************************************************************
* This is the main program for GSM MODEM TC35 using SK40C to control
*
* Author: ANG JIA YI @ Cytron Technologies Sdn. Bhd.
*******************************************************************************/
// GSM modem TC35 with PIC16F877A 
// This template C file should be compiled with:
// MPLAB IDE v8.63
// HI-TECH C for the PIC10/12/16 MCU Family
// HI-TECH operate in LITE Mode with v9.80
// Last Updated date: 1 July 2012
// Please email support@cytron.com.my if you have any problem

#include <htc.h>		//include hitech C header file
#include "system.h"		//include system header file for PTK40A
#include "uart.h"		//include uart header file, to enable the functions of uart
#include "lcd.h"		//include lcd header file, to enable the functions of lcd
/*******************************************************************************
* DEVICE CONFIGURATION WORDS                                                   *
*******************************************************************************/
// This configuration bits are for PIC16F877A using 20MHz Crystal or HS for OSC
#if defined (HITECH_V9_80)	//if Hi-Tech V9.80 compiler is being used
__CONFIG(HS &			// High Speed Crystal.
		 WDTDIS &		// Disable Watchdog Timer.
		 PWRTEN &		// Enable Power Up Timer.
		 BORDIS &		// Disable Brown Out Reset.
		 LVPDIS);		// Disable Low Voltage Programming.

#elif defined (HITECH_V9_82)		//else if Hi-Tech V9.82 compiler is being used
// If Hi-TECH C Compiler v9.82 is use, this configuration bit should be used
__CONFIG(FOSC_HS &			// High Speed Crystal.
		 WDTE_OFF &		// Disable Watchdog Timer.
		 PWRTE_ON &		// Enable Power Up Timer.
		 BOREN_OFF &		// Disable Brown Out Reset.
		 LVP_OFF);		// Disable Low Voltage Programming.
#endif	//end of pre-processor if condition
/*******************************************************************************
* Defination of label		                                                  *
*******************************************************************************/
#define sw1 	RC0 
#define sw2		RC1
#define sw3		RC2 
#define sw4 	RC3
#define	sw5 	RC4 

/*******************************************************************************
* PRIVATE FUNCTION PROTOTYPES                                                  *
*******************************************************************************/
void delay_ms(unsigned int ui_value);
void beep(unsigned char uc_count);
void reading(void);
/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
unsigned char phonenumber = 0;	// declare a variable to store temperature
unsigned char numberspace = 0;
unsigned char ar[60]={0};
unsigned char a=0;
/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{

	unsigned char b = 1;
	// ensure all the hardware port in zero initially
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	PORTE = 0;

	// Initialize the I/O port direction, this must be configured according to circuit
	// please refer to PTK40A schematic for details
	// TRISX control pin direction, output pin must be configure as '0'
	// while input must be configure as '1'
	TRISA = 0b00010001;
	TRISB = 0b00001111;
	TRISC = 0b10011111;
	TRISD = 0;
	TRISE = 0;


	//Initialize LCD
	lcd_initialize(); //to ensure the Uart modlue is properly configured to be used
	
	//Initialize UART
	uart_initialize(); //to ensure the Uart modlue is properly configured to be used
	
	// GSM modem are widely use in our daily life. FOr example for cellphone
	// GSMn also be use to contol the contoller to do what ever we want to do
	// GSM is a good long range device to contol the thing we needed to control
	
	lcd_clear();
	lcd_putstr("   CYTRON TECH \n GSM MODEM TC35");			//to display the lcd so we know what project is loaded in PIC
	delay_ms(1000);	lcd_clear();
	uart_putstr("AT \r");								//to standby the GSM modem to be prepare
	delay_ms(1000);	
	lcd_clear();
	lcd_putstr("1=read 2=ccl\r3=up  4=down");				//to display user interface
	uart_putstr("AT+CMGF=1\r");							//to switch GSM to text mode so the messenge can be read out
	delay_ms(1000);
	uart_putstr("AT+CMGR=");							//to get reading to read which page you want
	while(1) 	// create an infinite loop
	{
		if(sw1==0)				//sw1 is to read the messenge page we choose
		{
			uart_transmit(b+0x30);		//to read the messenge page choosen
			uart_transmit('\r');		//to enter the page
			reading();
			uart_putstr("AT+CMGR=");	//prepare to read 2nd messenge later on
		}
		else if(sw2==0)
		{
			while(sw2==0);			//cancel reading
			uart_transmit('\r');
			delay_ms(1000);			//wait
			uart_putstr("AT+CMGR=");	//reprepare top read the messenge
		}
		else if(sw3==0)				//sw3 is to choose page in increasing path
		{
			while(sw3==0);
			if(b==20)
			{
				b=20;
			}
			else
			b++;
			lcd_clear();
			lcd_bcd(2,b);
		}
		else if(sw4==0)			//sw4 is to choose page in decreasing path
		{
			while(sw4==0);
			if(b==1)
			{
				b=1;
			}
			else
			b--;
			lcd_clear();
			lcd_bcd(2,b);
		}
	}		
	while(1) continue;	// infinite loop to prevent PIC from reset if there is no more program	
}

/*******************************************************************************
* PRIVATE FUNCTION: delay_ms
*
* PARAMETERS:
* ~ ui_value	- The period for the delay in miliseconds.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Delay in miliseconds.
*
*******************************************************************************/
void delay_ms(unsigned int ui_value)
{
	while (ui_value-- > 0) {
		__delay_ms(1);		// macro from HI-TECH compiler which will generate 1ms delay base on value of _XTAL_FREQ in system.h
	}	
}

/*******************************************************************************
* PRIVATE FUNCTION: beep
*
* PARAMETERS:
* ~ uc_count	- How many times we want to beep.
*
* RETURN:
* ~ void
*
* DESCRIPTIONS:
* Beep for the specified number of times.
*
*******************************************************************************/
void beep(unsigned char uc_count)
{
	while (uc_count-- > 0) {
		pwm_set_duty_cycle(1023);	//buzzer is share at RC2 pin, PWM
		delay_ms(80);
		pwm_set_duty_cycle(0);
		delay_ms(100);
	}
}

void reading(void)				//reading prototype
{
		a=0;
		while(uc_uart_receive()!='+');			//this 4 line to to indicate that the messenge start after +32 code
		while(uc_uart_receive()!='3');			//which avaiable in every messenge
		while(uc_uart_receive()!='2');
		while(uc_uart_receive()!=10);
				
		while(ar[a] != 13)						//this part is to record down the messenge in ar[a] in array for
		{
			if(RCIF == 1)
			{	
				a++;
				ar[a] = uc_uart_receive();								
			}			
		}				

			lcd_clear();
			lcd_putstr("reading sw2 ccl");
			lcd_goto(0x40);
			a=1;
			while(ar[a] != 13)				//this part is to display out the messenge which already store in the array form
			{
				lcd_putchar(ar[a]);	
				a++;							
			}
			
			while(sw2==1);					//close messenge by pressing sw2
			lcd_clear();					
			lcd_putstr("1=read 2=ccl\r3=up 4=down"); 	//be preparing to read the 2nd messenge.
			while(sw2==0);
}			