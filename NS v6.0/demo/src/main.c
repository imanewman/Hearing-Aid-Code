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

#if defined (__dsPIC33F__)
    #include <p33Fxxxx.h>
	#include "..\h\dsPICDEM1_1Plus.h"
	#include "..\h\Si3000CodecDrv.h"
	#include "..\h\lcd.h"
#elif defined (__dsPIC33E__)
    #include <p33Exxxx.h>
	#include "..\h\MEB.h"
	#include "..\h\WM8731CodecDrv.h"
#else
    #error Selected processor not supported
#endif

#include "..\h\ns_api.h"

#if defined (__dsPIC33F__)
	_FGS(GWRP_OFF & GCP_OFF);
	_FOSCSEL(FNOSC_FRC);
	_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_XT);
	_FWDT(FWDTEN_OFF);
#elif defined (__dsPIC33E__)
    _FOSCSEL(FNOSC_FRC);			
    _FOSC(FCKSM_CSECMD & POSCMD_XT & OSCIOFNC_OFF & IOL1WAY_OFF);
    _FWDT(FWDTEN_OFF);
    _FPOR(FPWRT_PWR128 & BOREN_ON & ALTI2C1_ON & ALTI2C2_ON);
    _FICD(ICS_PGD1 & RSTPRI_PF & JTAGEN_OFF);
#endif

#if defined (__dsPIC33F__)
	Si3000CodecHandle si3000Codec;					/* Handle for the codec drive	r  	*/
	Si3000CodecRegisters si3000CodecRegisters;		/* Codec registers				*/
	int si3000DrvBuffer[SI3000_DRV_BUFFER_SIZE];	/* Buffer for codec IO buffering	*/
#elif defined (__dsPIC33E__)
	int codecInBuffer	[NS_FRAME][2];
	int codecOutBuffer	[NS_FRAME][2];
	wm8731_state wm8731_handle = {
		.index_samp		= 10,
		.sampling_freq	= WM8731_SAMPLING_FREQ_8K,
		.frame_size		= 2*NS_FRAME,
#ifdef NO_DMA
		.frame_ptr		= 0,
#endif
		.rx_status		= 0,
		.tx_status		= 0,
		.in_buffer		= (int *) codecInBuffer,
		.out_buffer		= (int *) codecOutBuffer
	};	
#endif

/* Main processing buffer		*/
int signalIn[NS_FRAME] _XBSS(4);	
int sampleCount;

/* Noise Suppression module variables		*/
int  nsStateMemX 	[NS_XSTATE_MEM_SIZE_INT]  	 _XBSS(4); 		
int  nsScratchMemX  [NS_XSCRATCH_MEM_SIZE_INT] 	 _XBSS(4);
#if defined (__dsPIC33F__)
	int  nsScratchMemY 	[NS_YSCRATCH_MEM_SIZE_INT] 	 _YBSS(512);
#elif defined (__dsPIC33E__)
	int  nsScratchMemY 	[NS_YSCRATCH_MEM_SIZE_INT] 	 __attribute__((eds,space(ymemory),aligned(512)));
#endif

volatile int nsLevel;				/* Selected NS Level						*/
volatile int applyNS;				/* Indicates if NS should be ON or OFF		*/
volatile int resetNS;				/* Indicates if NS should be reset			*/
volatile int displayChanged = 0;	/* Signaling Variable to update the status	*/

#if defined (__dsPIC33F__)
	int debounce = 0;				/* Interrupt debounce */
#endif

int main(void)
{
#if defined (__dsPIC33F__)
    /* Configure Oscillator to operate the device at 40MHz.
     * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
     * Fosc= 7.37M*43/(2*2)=80Mhz for 7.37M input clock */
    PLLFBD=41;				/* M=40	*/
    CLKDIVbits.PLLPOST=0;		/* N1=2	*/
    CLKDIVbits.PLLPRE=0;		/* N2=2	*/
    OSCTUN=0;
#elif defined (__dsPIC33E__)
    /* Configure Oscillator to operate the device at 40MHz.
     * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
     * Fosc= 8.0M*40/(2*2)=80Mhz for 8.0M input clock */
    PLLFBD=38;				/* M=40	*/
    CLKDIVbits.PLLPOST=0;		/* N1=2	*/
    CLKDIVbits.PLLPRE=0;		/* N2=2	*/
    OSCTUN=0;
#endif

	__builtin_write_OSCCONH(0x03);		/*	Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b011);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);			/*  Wait for the PLL to lock 			*/
	
#if defined (__dsPIC33F__)	
	dsPICDEM1_1PlusInit(&debounce);	/* Initialise the dsPICDEM1_1Plus Demo board	*/
#elif defined (__dsPIC33E__)
	MEBInit();
#endif
	
	applyNS = 0;											/* NS is OFF on startup		*/
	resetNS = 0;											/* RESET is not requested	*/
	NS_init(nsStateMemX, nsScratchMemX, nsScratchMemY);		/* Initialise the Noise Suppression module	*/
	nsLevel = NS_getNoiseReduction(nsStateMemX);			/* Get the default Noise reduction level		*/

#if defined (__dsPIC33F__)		
	LCD_initSPI();											/* Initialise the LCD	*/
    	LCD_displayLogo(1);							 		/* Display the boot logo	*/
    
    	LCD_writeRow("Noise Suppression    ",0);    				/* Display the running screen	*/
	LCD_writeRow("OnOff SW1 : Init SW4",1);
	LCD_displayNS(nsLevel, applyNS);							/* Draw the VU meter on the LCD display	*/
	LCD_displayBox(); 
	LCD_displayVU(signalIn, NS_FRAME, NS_FALSE);
#endif

#if defined (__dsPIC33F__)		
	Si3000CodecInit(&si3000Codec,si3000DrvBuffer);			/* Initialise the Si3000 codec		*/
	Si3000CodecStart(&si3000Codec, &si3000CodecRegisters);	/* Start the Si3000 codec		*/
#elif defined (__dsPIC33E__)
	WM8731Init(&wm8731_handle, (int *) codecInBuffer, (int *) codecOutBuffer, WM8731_SAMPLING_FREQ_8K, 2*NS_FRAME);
	WM8731Control(&wm8731_handle);
	WM8731Start();
#endif
	
	/* This is the main loop. Here is what happens
	 * 1. Read a frame of audio data and update LCD VU meter
	 * 2. Reset NS is required
	 * 3. Update display
	 * 4. Apply NS with selected paramters to audio data
	 * 5. Output the processed audio data frame
	 * 6. Check the switches and update parameters.
	 */
	
	while(1)
	{
#if defined (__dsPIC33F__)				
		while(Si3000CodecReadBusy(&si3000Codec));			/* Wait until we have a full buffer for reading	*/
		Si3000CodecRead(&si3000Codec,signalIn,80);
#elif defined (__dsPIC33E__)
		WM8731Read(&wm8731_handle);
		for(sampleCount = 0; sampleCount < NS_FRAME; sampleCount++)
		{
			signalIn[sampleCount] = codecInBuffer[sampleCount][0];
		}
#endif

#if defined (__dsPIC33F__)					
		/* update the VU meter on the LCD display	*/
		LCD_displayVU(signalIn, NS_FRAME, NS_getIsSpeech(nsStateMemX)); 
#endif		
		
		if (resetNS) 
		{
			/* If NS algorithm reset button has been pressed, reset NS module	*/
			NS_init(nsStateMemX, nsScratchMemX, nsScratchMemY);
			NS_setNoiseReduction(nsStateMemX, nsLevel);
			resetNS = 0; 	

#if defined (__dsPIC33F__)					
			/* Clear LED4 because we have reset the NS algorithm	*/
			LED4 = DSPICDEM_LED_OFF;
#endif
		}
		
		if (displayChanged == 1)
		{
#if defined (__dsPIC33F__)					
			/* Update the LCD if a button has been pressed	*/
			LCD_displayNS(nsLevel, applyNS);
#endif
			displayChanged = 0;
			NS_setNoiseReduction(nsStateMemX, nsLevel);
		}
		
		/* Always perform the NS, signalIn is unchanged is nsOnOffFlag is 0	*/
		NS_apply(nsStateMemX, signalIn, applyNS);

#if defined (__dsPIC33F__)									
		/* Wait until we have an empty buffer to write to	*/
		while(Si3000CodecWriteBusy(&si3000Codec));
		Si3000CodecWrite(&si3000Codec,signalIn,80);
#elif defined (__dsPIC33E__)
		WM8731Write(&wm8731_handle);
		for (sampleCount = 0; sampleCount < NS_FRAME; sampleCount++)
		{
			codecOutBuffer[sampleCount][0] = signalIn[sampleCount];
			codecOutBuffer[sampleCount][1] = signalIn[sampleCount];
		}
#endif		

#if defined (__dsPIC33F__)									
		/* Decrement the interrupt debounce counter and check the switches */
		if (debounce > 0)
		{
		    debounce --;
		} 
		if(CheckSwitchS1() == 1)
		{
			/* Switch S1 toggles if NS is applied to the signal */
			LED1 ^= 1;
			applyNS ^= 1;
			displayChanged = 1;
		}	
		if(CheckSwitchS2() == 1)
		{
			/* Switch S2 reduces the Noise reduction level */
			if(nsLevel > 0){
				nsLevel --;
			}else{
				nsLevel = 0;
			}
			displayChanged = 1;
		}		
		if(CheckSwitchS3() == 1)
		{
			/* Switch S3 increases the Noise reduction level */
			if(nsLevel < 44){
				nsLevel ++;
			}else{
				nsLevel = 44;
			}
			displayChanged = 1;
		}	
		if(CheckSwitchS4() == 1)
		{
			/* Switch S4 resets the NS engine */
			resetNS = 1;
			LED4 = DSPICDEM_LED_ON;
		}			
#elif defined (__dsPIC33E__)
		if(CheckSwitchSW1() == 1)
		{
			/* Switch SW1 on dsPIC33E USB Starter Kit toggles if NS is applied to the signal */
			LED3 ^= 1;
			applyNS ^= 1;
			displayChanged = 1;
		}	
		if(CheckSwitchSW2() == 1)
		{
			/* Switch SW2 on dsPIC33E USB Starter Kit reduces the Noise reduction level */
			if(nsLevel > 0){
				nsLevel --;
			}else{
				nsLevel = 0;
			}
			displayChanged = 1;
		}		
		if(CheckSwitchSW3() == 1)
		{
			/* Switch SW3 on dsPIC33E USB Starter Kit increases the Noise reduction level */
			if(nsLevel < 44){
				nsLevel ++;
			}else{
				nsLevel = 44;
			}
			displayChanged = 1;
		}	
		if(CheckSwitchS1() == 1)
		{
			/* Switch S1 on Multimedia Expansion Board (LCD side) resets the NS engine */
			resetNS = 1;
		}
#endif
	}
}

