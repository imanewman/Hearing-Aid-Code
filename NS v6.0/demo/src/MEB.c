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

#include "..\h\MEB.h"

int debounceSW1;					/* Debounce counter for switch SW1	*/
int debounceSW2;					/* Debounce counter for switch SW2	*/
int debounceSW3;					/* Debounce counter for switch SW3	*/
int debounceS1;						/* Debounce counter for switch S1	*/

void MEBInit(void)
{
	/* Intialize the board LED and switch ports and turn all LEDS off 	*/

	LED1_TRIS = 0;
	LED1 = MEB_LED_OFF; 	
	LED2_TRIS = 0;
	LED2 = MEB_LED_OFF;
	LED3_TRIS = 0;
	LED3 = MEB_LED_OFF;

	SWITCH_SW1_TRIS = 1;
	SWITCH_SW2_TRIS = 1;
	SWITCH_S1_TRIS = 1;
	SWITCH_SW1_ANSEL = 0;
	SWITCH_SW2_ANSEL = 0;
	SWITCH_S1_ANSEL = 0;
}

int CheckSwitchSW1(void)
{
	/* This function returns a 1 if a valid key press was detected on S1 */
	int isActive = 0;
	if(SWITCH_SW1 == 0){
		
		debounceSW1++;
		if (debounceSW1 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 
			debounceSW1  = 0;
			isActive = 1;
		}
	}
	else	{
		debounceSW1 = 0;
		isActive = 0;
	}
	
	return(isActive);
}	

int CheckSwitchSW2(void)
{
	/* This function returns a 1 if a valid key press was detected on S1 */
	int isActive = 0;
	if(SWITCH_SW2 == 0){
		
		debounceSW2++;
		if (debounceSW2 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 
			debounceSW2  = 0;
			isActive = 1;
		}
	}
	else	{
		debounceSW2 = 0;
		isActive = 0;
	}
	
	return(isActive);
}

int CheckSwitchSW3(void)
{
	/* This function returns a 1 if a valid key press was detected on S1 */
	int isActive = 0;
	if(SWITCH_SW3 == 0){
		
		debounceSW3++;
		if (debounceSW3 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 
			debounceSW3  = 0;
			isActive = 1;
		}
	}
	else	{
		debounceSW3 = 0;
		isActive = 0;
	}
	
	return(isActive);
}

int CheckSwitchS1(void)
{
	/* This function returns a 1 if a valid key press was detected on S1 */
	int isActive = 0;
	if(SWITCH_S1 == 0){
		
		debounceS1++;
		if (debounceS1 == SWITCH_DEBOUNCE){
			
			/* This means that the Switch S1 was pressed long enough
			 * for a valid key press	*/
			 
			debounceS1  = 0;
			isActive = 1;
		}
	}
	else	{
		debounceS1 = 0;
		isActive = 0;
	}
	
	return(isActive);
}


