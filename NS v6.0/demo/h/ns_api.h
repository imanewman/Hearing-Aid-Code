/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        ns_api.h
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
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
* certify, or support the code.*/
/**************************************************************************************/
#ifndef __NS_API_H__
#define __NS_API_H__

#define NS_FALSE 						  0
#define NS_TRUE 						  1
#define NS_SLOW 						100
#define NS_NORMAL 						200
#define NS_FAST 						300
#define NS_FRAME   					  	 80
#define NS_XSTATE_MEM_SIZE_INT   	    384
#define NS_XSCRATCH_MEM_SIZE_INT 	     50
#define NS_YSCRATCH_MEM_SIZE_INT   	    128

#define NS_NRLEVELDEFAULT 				 15
#define NS_POWERFACTORDEFAULT			  2
#define NS_VADHANGOVERDEFAULT 			  6
#define NS_NOISEFLOORDEFAULT	   0x1D1917

void NS_init(int* ptrStateX, int* xScratchMem,int * yScratchMem);
void NS_relocateXScratchMem(int* ptrStateX, int* xScratchMem);
void NS_relocateYScratchMem(int* ptrStateX, int* yScratchMem);
void NS_apply(int* ptrStateX, int* signalIn, int enable);

void NS_setNoiseReduction(int* ptrStateX, int leveldB);
int  NS_getNoiseReduction(int* ptrStateX);

void NS_setVadHangover(int* ptrStateX, int enable, int value);
int  NS_getVadHangover(int* ptrStateX);

void NS_setNoiseFactor(int* ptrStateX, int value);
int  NS_getNoiseFactor(int* ptrStateX);

void NS_setNoiseEstimationFactor(int* ptrStateX, int level);
int  NS_getNoiseEstimationFactor(int* ptrStateX);

void NS_setNoiseFloor(int* ptrStateX, long floor);
long NS_getNoiseFloor(int* ptrStateX);

int NS_getIsSpeech(int* ptrStateX);

int NS_estimateSNR(int* preNS, int* postNS);

#endif
