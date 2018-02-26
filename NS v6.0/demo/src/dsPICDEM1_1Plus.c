/**********************************************************************
* © 2011 Microchip Technology Inc.
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
************************************************************************/

#include <p33Fxxxx.h>
#include "..\h\dsPICDEM1_1Plus.h"

static int  * debounceCounter;		/* Copy of the debounce counter */
static volatile int switchS1;		/* Switch S1 flag */
static volatile int switchS2;		/* Switch S2 flag */
static volatile int switchS3;		/* Switch S3 flag */
static volatile int switchS4;		/* Switch S4 flag */

void dsPICDEM1_1PlusInit(int * debounce)
{
	/* Configure ANx pins to Digital */
    	ADPCFG = 0xFFFF;        
   	AD1PCFGH =0xFFFF; 
   	AD1PCFGL =0xFFFF; 
   	AD2PCFGL =0xFFFF;
	
	/* Set the TRIS for Switch Port Pins */
	SW1_TRIS = TRIS_INPUT;    
	SW2_TRIS = TRIS_INPUT;
	SW3_TRIS = TRIS_INPUT;
	SW4_TRIS = TRIS_INPUT;
	
	/* Set the TRIS bits for the LEDS */
	LED1_TRIS = TRIS_OUTPUT;
	LED2_TRIS = TRIS_OUTPUT;
	LED3_TRIS = TRIS_OUTPUT;
	LED4_TRIS = TRIS_OUTPUT;
    
	/* Switch off all the LEDS */
   	LED1 = DSPICDEM_LED_OFF; 
   	LED2 = DSPICDEM_LED_OFF;  
   	LED3 = DSPICDEM_LED_OFF; 
   	LED4 = DSPICDEM_LED_OFF; 
   	
   	debounceCounter = debounce;
   	switchS1 = 0;	/* All switch flags reset */
   	switchS2 = 0;
   	switchS3 = 0;
   	switchS4 = 0;
   	
   	 INTCON2 = 0x001E;	/*Setup INT1, INT2, INT3 & INT4 pins to interupt */
                                		/*on falling edge and set up INT0 pin to interupt */
                               		/*on rising edge */
     
        IFS1bits.INT1IF = 0;    /*Reset INT1 interrupt flag */
        IEC1bits.INT1IE = 1;    /*Enable INT1 Interrupt Service Routine */

        IFS1bits.INT2IF = 0;    /*Reset INT0 interrupt flag */
        IEC1bits.INT2IE = 1;    /*Enable INT0 Interrupt Service Routine */

        IFS3bits.INT3IF = 0;    /*Reset INT1 interrupt flag */
        IEC3bits.INT3IE = 1;    /*Enable INT1 Interrupt Service Routine */

        IFS3bits.INT4IF = 0;    /*Reset INT1 interrupt flag */
        IEC3bits.INT4IE = 1;    /*Enable INT1 Interrupt Service Routine */
}

int CheckSwitchS1(void)
{
	/* This function will return 1 if a valid
	 * switch press was detected */
	 
	int wasPressed = 0;
	
	if(switchS1 == 1) {
		wasPressed = 1;
		switchS1 = 0;
	}	
	
	return(wasPressed);

}

int CheckSwitchS2(void)
{
	/* This function will return 1 if a valid
	 * switch press was detected */
	 
	int wasPressed = 0;
	
	if(switchS2 == 1) {
		wasPressed = 1;
		switchS2 = 0;
	}	
	
	return(wasPressed);

}	

int CheckSwitchS3(void)
{
	/* This function will return 1 if a valid
	 * switch press was detected */
	 
	 int wasPressed = 0;
	
	if(switchS3 == 1) {
		wasPressed = 1;
		switchS3 = 0;
	}	
	
	return(wasPressed);

}	

int CheckSwitchS4(void)
{
	/* This function will return 1 if a valid
	 * switch press was detected */
	 
	int wasPressed = 0;
	
	if(switchS4 == 1) {
		wasPressed = 1;
		switchS4 = 0;
	}	
	
	return(wasPressed);

}			

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	/* If the denounce count is zero then, we have a valid 
	 * switch press. Reset the value for the next switch press */
	 
	_INT1IF = 0;
	if(*debounceCounter == 0)
	{
		*debounceCounter = DEBOUNCE_INTERVAL;
		switchS1 = 1;
	}
}

void __attribute__((interrupt, no_auto_psv)) _INT2Interrupt(void)
{
	/* If the denounce count is zero then, we have a valid 
	 * switch press. Reset the value for the next switch press */
	
	_INT2IF = 0;
	if(*debounceCounter == 0)
	{
		*debounceCounter = DEBOUNCE_INTERVAL;
		switchS2 = 1;
	}
}		

void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)
{
	/* If the denounce count is zero then, we have a valid 
	 * switch press. Reset the value for the next switch press */
	
	_INT3IF = 0;
	if(*debounceCounter == 0)
	{
		*debounceCounter = DEBOUNCE_INTERVAL;
		switchS3 = 1;
	}
}

void __attribute__((interrupt, no_auto_psv)) _INT4Interrupt(void)
{
	/* If the denounce count is zero then, we have a valid 
	 * switch press. Reset the value for the next switch press */

	_INT4IF = 0;
	if(*debounceCounter == 0)
	{
		*debounceCounter = DEBOUNCE_INTERVAL;
		switchS4 = 1;
	}
}


