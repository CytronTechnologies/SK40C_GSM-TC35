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

/*******************************************************************************
* Global Variables                                                             *
*******************************************************************************/
unsigned char test[20]={0};
/*******************************************************************************
* MAIN FUNCTION                                                                *
*******************************************************************************/
int main(void)
{
	unsigned char check=0;
	unsigned char check1 = 0;
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
	lcd_putstr("   CYTRON TECH \n GSM MODEM TC35");
	delay_ms(1000);	lcd_clear();
	uart_putstr("AT \r");				//to calll the GSM modem to be ready
	delay_ms(1000);						//normally will fead back ok if look with hyperterminal signal
	lcd_clear();
	while(1) 	// create an infinite loop
	{
		lcd_goto(0x80);
		lcd_putstr("SMS mode \r press sw1");			//preparing to send messenge
		if(sw1==0)
		{
			lcd_clear();
			lcd_putstr("preparing\r 016 mode");
			uart_putstr("AT+CMGF=1\r");  //				//set to mesenge mode and text mode
			delay_ms(1000);
			uart_putstr("AT+CSCA=+60162999902\r");  //	set the phone messenge center number +60162999902 is for digi
			delay_ms(1000);
			uart_putstr("AT+CMGS=0125651453");  //set the phone number which u want to send to AT+CMGS=<ur number>
			delay_ms(500);
			uart_transmit('\r');		//enter to call the modem to switch to text mode
			while(1)
			{
				check=uc_uart_receive();
				if(check=='E')				//check if no error as feed back the could type messenge 
				{							// normally got > to inform use that can text messenge
				lcd_clear();
				lcd_putstr("error");			
				}
				else 
				break;
				
			}
				lcd_clear();
				lcd_putstr("choose test\rsw3=ok sw4=off");		//fix messenge to be type into the text mode after > icon
			while(1)
			{	

				if(sw3==0)						
				{
					while(sw3==0);
					uart_putstr("OK");				//send ok to text
					lcd_putstr("OK");
					break;
				}
				else if(sw4==0)
				{
					while(sw4==0);
					uart_putstr("OFF");				//send off to text
					lcd_putstr("OFF");
					break;
				}
			}
				lcd_clear();
				lcd_putstr("sw1=send \rsw2=cancel");	//lcd display to let user know what to do
			while(1)
			{
				if(sw1==0)
				{
					uart_transmit(0x1A);			//ctrl+z is send to send the messenge
					lcd_clear();
					lcd_putstr("sending");
					while(uc_uart_receive()!='+'); //conform that the messenge is already sended
					lcd_clear();
					lcd_putstr("sended");
					delay_ms(1000);
					lcd_clear();
					break;
				}
				else if(sw2==0)
				{
					lcd_clear();
					lcd_putstr("cancel\rwaiting");
					uart_putstr("\r\r");			//cancel the sending messenge and wait it to ready mode
					while(uc_uart_receive()!='E');
					lcd_clear();
					break;					
				}			
				
							
			}	
			
						
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

	

