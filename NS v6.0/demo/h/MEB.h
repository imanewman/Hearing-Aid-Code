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

#ifndef __MEB_H__
#define __MEB_H__

#include <p33Exxxx.h>

#define SWITCH_DEBOUNCE	100

/* The LEDs and their ports	and analog/digital selection bits */
#define LED1_TRIS	_TRISD0			/* LED1 on dsPIC33E USB Starter Kit */
#define LED1		_LATD0
#define LED2_TRIS	_TRISD1			/* LED2 on dsPIC33E USB Starter Kit */
#define LED2		_LATD1
#define LED3_TRIS	_TRISD2			/* LED3 on dsPIC33E USB Starter Kit */
#define LED3		_LATD2

/* The Switches and their ports and analog/digital selection bits */
#define SWITCH_SW1_TRIS	 _TRISD6	/* SW1 on dsPIC33E USB Starter Kit */
#define SWITCH_SW1		 _RD6
#define	SWITCH_SW1_ANSEL _ANSD6
#define SWITCH_SW2_TRIS	 _TRISD7	/* SW2 on dsPIC33E USB Starter Kit */
#define SWITCH_SW2		 _RD7
#define	SWITCH_SW2_ANSEL _ANSD7
#define SWITCH_SW3_TRIS	 _TRISD13	/* SW3 on dsPIC33E USB Starter Kit */
#define SWITCH_SW3		 _RD13
#define SWITCH_S1_TRIS	 _TRISB15	/* S1 on Multimedia Expansion Board */
#define SWITCH_S1		 _RB15
#define SWITCH_S1_ANSEL	 _ANSB15

#define MEB_LED_ON 1
#define MEB_LED_OFF 0

int CheckSwitchSW1(void);
int CheckSwitchSW2(void);
int CheckSwitchSW3(void);
int CheckSwitchS1(void);
void MEBInit(void);

#endif
