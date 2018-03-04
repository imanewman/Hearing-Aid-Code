/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        main.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC33FJ256GP506
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
* certify, or support the code.
************************************************************************/

#include <p33FJ256GP506.h>
#include "..\h\WM8510CodecDrv.h"
#include "..\h\sask.h"
#include "..\h\SST25LF040ADrv.h"
#include "..\h\G711.h"


_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);

#define FRAME_SIZE 				128			/* Each audio frame will have these many samples	*/
#define SPEECH_SEGMENT_SIZE		98049L		/* Length of the intro message					*/
#define WRITE_START_ADDRESS	0x20000		/* Flash memory address for user				*/


/* Allocate memory for buffers and drivers	*/

int 		codecBuffer		[WM8510DRV_DRV_BUFFER_SIZE];
int 		samples			[FRAME_SIZE];
char 	encodedSamples	[FRAME_SIZE];
int 		decodedSamples	[FRAME_SIZE];
char 	flashMemoryBuffer	[SST25LF040ADRV_BUFFER_SIZE];

/* Instantiate the drivers 	*/
WM8510Handle codec;
SST25LF040AHandle flashMemoryHandle; 

/* Create the driver handles	*/
WM8510Handle * codecHandle = &codec;
SST25LF040AHandle *pFlashMemoryHandle 	= &flashMemoryHandle;


 long currentReadAddress;		/* This one tracks the intro message	*/
 long currentWriteAddress;		/* This one tracks the writes to flash	*/
 long userPlaybackAddress;		/* This one tracks user playback		*/

int record;							/* If set means recording			*/
int playback;							/* If set means playback is in progress	*/
int erasedBeforeRecord;				/* If set means that erase before record	*/
									/* is complete	*/ 
long address;							/* Used for erasing the flash			*/


int main(void)
{
	/* Configure Oscillator to operate the device at 40MHz.
	 * Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
	 * Fosc= 7.37M*40/(2*2)=80Mhz for 7.37M input clock */
	 int commandValue;
	 
	PLLFBD=41;				/* M=39	*/
	CLKDIVbits.PLLPOST=0;		/* N1=2	*/
	CLKDIVbits.PLLPRE=0;		/* N2=2	*/
	OSCTUN=0;			
	
	__builtin_write_OSCCONH(0x01);		/*	Initiate Clock Switch to FRC with PLL*/
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0b01);	/*	Wait for Clock switch to occur	*/
	while(!OSCCONbits.LOCK);

	/* Initialize flags and address variables	*/
	 record = 0;
	 playback = 0;
	 currentReadAddress = 0;
	 currentWriteAddress = WRITE_START_ADDRESS;
	 userPlaybackAddress = WRITE_START_ADDRESS;
	 address = 0;
	 erasedBeforeRecord = 0;	


	/* Intialize the board and the drivers	*/
	SASKInit();
	WM8510Init(codecHandle,codecBuffer);
	SST25LF040AInit		(pFlashMemoryHandle,flashMemoryBuffer);	/* For the  Flash	*/

	/* Start the flash driver functions	*/
	
	SST25LF040AStart	(pFlashMemoryHandle);
		
	/* Start Audio input and output function	*/
	WM8510Start(codecHandle);
		
	/* Configure codec for 8K operation	*/
	WM8510SampleRate8KConfig(codecHandle);
	
	/* Enable the serial flash device for writing */
	commandValue = 0;
	SST25LF040AIOCtl(pFlashMemoryHandle, SST25LF040ADRV_WRITE_STATUS_ENABLE,0);
	SST25LF040AIOCtl(pFlashMemoryHandle, SST25LF040ADRV_WRITE_STATUS,(void *)(&(commandValue)));

	/* Main processing loop. Executed for every input and 
	 * output frame	*/
	 
	while(1)
	{
			/* Obtaing the ADC samples	*/
			while(WM8510IsReadBusy(codecHandle));
			WM8510Read(codecHandle,samples,FRAME_SIZE);
					
			/* Playback the intro message if record or play functions are not active	
			 * Rewind the currentReadAddress if the message has reached the end	*/
			if(!record && !playback)
			{	
				while(SST25LF040AIsBusy(pFlashMemoryHandle));
				SST25LF040ARead(pFlashMemoryHandle,currentReadAddress,encodedSamples,FRAME_SIZE);
				while(SST25LF040AIsBusy(pFlashMemoryHandle));
				currentReadAddress += FRAME_SIZE;
				if(currentReadAddress >= SPEECH_SEGMENT_SIZE)
				{
					currentReadAddress = 0;
				}
			}
			
			/* If record is enabled, encode the samples using G711. 
			 * Store in flash. Erase flash before recording starts	*/
			 	
			if(record == 1)
			{
			
				if(erasedBeforeRecord == 0)
				{
					/* Stop the Audio input and output since this is a blocking
					 * operation. Also rewind record and playback pointers to
					 * start of the user flash area.*/
					 
					WM8510Stop(codecHandle);
					currentWriteAddress = WRITE_START_ADDRESS;
					userPlaybackAddress = WRITE_START_ADDRESS;
					RED_LED = SASK_LED_ON;
				
					/* Erase the user area of the flash. The intro message is not erased	*/
					for(address = WRITE_START_ADDRESS; address < SST25LF040ADRV_LAST_ADDRESS; address += 0x8000)
					{
						/* Erase each sector. Each sector is 0xFFFF long	*/
						SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_WRITE_ENABLE,0);	
						SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_BLOCK_ERASE,(void *)&address);	
						while(SST25LF040AIsBusy(pFlashMemoryHandle));
					}
					RED_LED = SASK_LED_OFF;
					
					/* Since erase is complete, the next time the loop is executed
					 * dont erase the flash. Start the audio input and output	*/
					 
					erasedBeforeRecord = 1;
					WM8510Start(codecHandle);
				}
				else
				{	
					/* Record the encoded audio frame. Yellow LED turns on when
					 * when recording is being performed	*/
					
					YELLOW_LED = SASK_LED_ON;
					G711Lin2Ulaw(samples,encodedSamples,FRAME_SIZE);
				
					while(SST25LF040AIsBusy(pFlashMemoryHandle));
					SST25LF040AIOCtl(pFlashMemoryHandle,SST25LF040ADRV_WRITE_ENABLE,0);
					SST25LF040AWrite(pFlashMemoryHandle,currentWriteAddress,encodedSamples,FRAME_SIZE);
					currentWriteAddress += FRAME_SIZE;
				
					if(currentWriteAddress >= SST25LF040ADRV_LAST_ADDRESS)
					{
					
						YELLOW_LED = SASK_LED_OFF;
						erasedBeforeRecord = 0;
						record = 0;
						playback = 1;
					}
				}
					
			}
			
			/* If playback is enabled, then start playing back samples from the
			 * user area. Playback only till the last record address and then 
			 * rewind to the start	*/
			 
			if(playback == 1)
			{
				GREEN_LED = SASK_LED_ON;
				erasedBeforeRecord = 0;		
				while(SST25LF040AIsBusy(pFlashMemoryHandle));				
				SST25LF040ARead(pFlashMemoryHandle,userPlaybackAddress,encodedSamples,FRAME_SIZE);
				while(SST25LF040AIsBusy(pFlashMemoryHandle));
			
				userPlaybackAddress += FRAME_SIZE;
				if(userPlaybackAddress >= currentWriteAddress)
				{
					userPlaybackAddress = WRITE_START_ADDRESS;
				}
			}
				
		
				
		
			/* Decode the samples	*/
			G711Ulaw2Lin (encodedSamples,decodedSamples, FRAME_SIZE);
	
			/* Wait till the codec is available for a new  frame	*/
			while(WM8510IsWriteBusy(codecHandle));	
		
			/* Write the frame to the output	*/
			WM8510Write (codecHandle,decodedSamples,FRAME_SIZE);
		
			
			/* The CheckSwitch functions are defined in sask.c	*/
			
			if((CheckSwitchS1()) == 1)
			{
				/* Toggle the record function and Yellow led.
				 * Rewind the intro message playback pointer. 
				 * And if recording, disable playback.*/
				 
				
				record ^=1;				
				currentReadAddress = 0;	
				if(record == 1)
				{
					playback = 0;
					GREEN_LED = SASK_LED_OFF;
//					commandValue = 0b001000000;
//					WM8510IOCtl(codecHandle,WM8510_MONO_MIXER_CTRL,	 (void *) &commandValue);
				}
				else
				{
//					commandValue = 0b000000001;
//					WM8510IOCtl(codecHandle,WM8510_MONO_MIXER_CTRL,	 (void *) &commandValue);
				YELLOW_LED = SASK_LED_OFF;
				}
			}
			
			
			if((CheckSwitchS2()) == 1)
			{
				/* Toggle the record function and AMBER led.
				 * Rewind the intro message playback pointer. 
				 * And if recording, disable playback.*/
				 
				GREEN_LED ^=1;
				playback ^=1;
				currentReadAddress = 0;		
				if(playback == 1)
				{
					record = 0;
					YELLOW_LED = SASK_LED_OFF;
				}
			}
			
			
	
	}
	
	
}
