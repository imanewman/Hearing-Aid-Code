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
#ifndef __DSPICDEM1_1PLUS_H__
#define __DSPICDEM1_1PLUS_H__

#define DEBOUNCE_INTERVAL 25 

#define TRIS_INPUT 1
#define TRIS_OUTPUT 0

#define DSPICDEM_LED_ON		0
#define DSPICDEM_LED_OFF		1

#define SW1_TRIS TRISAbits.TRISA12
#define SW2_TRIS TRISAbits.TRISA13
#define SW3_TRIS TRISAbits.TRISA14
#define SW4_TRIS TRISAbits.TRISA15

#define LED1_TRIS TRISDbits.TRISD0
#define LED2_TRIS TRISDbits.TRISD1
#define LED3_TRIS TRISDbits.TRISD2
#define LED4_TRIS TRISDbits.TRISD3

/* Marcos to control LED1-4	*/
#define LED1 LATDbits.LATD0
#define LED2 LATDbits.LATD1
#define LED3 LATDbits.LATD2
#define LED4 LATDbits.LATD3

/* Marcos used to read status of switches	*/
#define SW1 PORTAbits.RA12
#define SW2 PORTAbits.RA13
#define SW3 PORTAbits.RA14
#define SW4 PORTAbits.RA15

/* This function initializes the ports and interrupts
 * needed for operating the board LED and switches */
 
void dsPICDEM1_1PlusInit(int * debounce);	

/* These functions return the status of the switches */
int CheckSwitchS1(void);
int CheckSwitchS2(void);
int CheckSwitchS3(void);
int CheckSwitchS4(void);

#endif
