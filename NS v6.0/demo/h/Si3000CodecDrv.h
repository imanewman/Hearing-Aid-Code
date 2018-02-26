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

#ifndef __SI3000CODECDRV_H__
#define __SI3000CODECDRV_H__


#include <p33Fxxxx.h>


#define SI3000_CODEC_FRAME 		80
#define SI3000_DRV_BUFFER_SIZE	(SI3000_CODEC_FRAME * 4)
#define SAMPLE_RATE				8000L 
#define SI3000_CODEC_DRV_FCY		40000000L
#define DCI_INTERRUPT_PRIORITY	4
#define FRAME_CLK				256L * SAMPLE_RATE
#define DCI_BRG_VALUE			(SI3000_CODEC_DRV_FCY / (2L * FRAME_CLK)) - 1L

#define SI3000_RESET_PIN_TRIS		TRISFbits.TRISF6
#define SI3000_RESET_PIN			LATFbits.LATF6
#define SI3000_RESET_DURATION	100

typedef struct Si3000CodecHandle
{
	int* 		inputBuffer1;					/* Ping Pong Input Buffer 1	*/
	int* 		inputBuffer2;					/* Ping Pong Input Buffer 2	*/
	int*			outputBuffer1;					/* Ping Pong Output Buffer 1	*/	
	int*			outputBuffer2;					/* Ping Pong Output Buffer 2	*/
	volatile int 	currentSampleIndex;			/* Tracks the sample being processed	*/
	volatile int 	currentFrameSize;				/* The size of the current frame being processed - 1	*/
	volatile int 	newFrameSize;				/* The size of the new frame	*/
	volatile int*	activeInputBuffer;				/* The active ping pong input buffer	*/
	volatile int*	activeOutputBuffer;				/* The active ping pong output buffer	*/
	volatile int  	statusFlag;					/* Tracks the state of the driver		*/
	volatile int	commandValue;				/* The value to be output		*/
	volatile int 	command;					/* The address of the register to be output			*/
} Si3000CodecHandle;

/* This data structure is available only for initialisation	*/
/* You still have to use the ioctl call if you want to control	*/
/* the Si3000 at run time							*/

typedef struct Si3000CodecRegisters
{
	unsigned int control1;
	unsigned int control2;
	unsigned int pll1DivideN1;
	unsigned int pll1MultiplyM1;
	unsigned int rxGainControl1;
	unsigned int adcVolumeControl;
	unsigned int dacVolumeControl;
	unsigned int analogAttenuation;

} Si3000CodecRegisters;

/* Si3000 Status Flag	
 * Bit 0 - State Counter Bit 0
 * Bit 1 - State Counter Bit 1
 * Bit 2 - State Counter Bit 2
 * Bit 3 - Read Busy
 * Bit 4 - Write Busy
 * Bit 5 - Primary Secondary Frame Indicator (P/S)
 * Bit 6 - Command request
 * Bit 7 - Buffer Indicator
 * */

/* If COMMAND_REQ = 1
 * State Counter = 0 -> Set the LSB of the primary frame when P/S = 0 and increment state counter
 * State Counter = 1 -> Form the secondary frame when P/S = 1 and increment  state counter
 * State Counter = 2 -> Send a primary frame with LSB = 0 when P/S = 0 and increment state counter
 * State Counter = 3 -> Read the secondary frame recieved from the codec when P/S = 1. Clear COMMAND_REQ
 * State Counter = 0.
 */

/* These are GET, SET and CLR macros for the different
 * flags	*/
 
#define GET_STATE_COUNTER	0x0007
#define CLR_STATE_COUNTER	0xFFF8

#define GET_CANSEND_CMD		0x0002
#define CLR_CANSEND_CMD		0xFFFD
#define SET_CANSEND_CMD		0x0002

#define GET_COMMAND_SENT	0x0001
#define CLR_COMMAND_SENT	0xFFFE
#define SET_COMMAND_SENT	0x0001

#define GET_READ_BUSY		0x0008
#define SET_READ_BUSY		0x0008
#define CLR_READ_BUSY		0xFFF7

#define GET_WRITE_BUSY		0x0010
#define SET_WRITE_BUSY		0x0010
#define CLR_WRITE_BUSY		0xFFEF

#define GET_PRI_SEC_FRAME 	0x0020
#define TGL_PRI_SEC_FRAME 	0x0020

#define GET_COMMAND_REQ	0x0040
#define SET_COMMAND_REQ	0x0040
#define CLR_COMMAND_REQ	0xFFBF

#define GET_BUFFER_IND		0x0080
#define TGL_BUFFER_IND		0x0080

#define IS_READ_COMMAND		0x2000

#define CONTROL_1_WRITE				0x0100
#define CONTROL_2_WRITE				0x0200
#define PLL1_DIVIDE_N1_WRITE			0x0300
#define PLL1_MULTIPLY_M1_WRITE 		0x0400
#define RX_GAIN_CONTROL_1_WRITE		0x0500
#define ADC_VOLUME_CONTROL_WRITE	0x0600
#define DAC_VOLUME_CONTROL_WRITE	0x0700
#define ANALOG_ATTENUATION_WRITE	0x0900

#define STATUS_REPORT_READ			0x2800

#define CONTROL_1_READ				0x2100
#define CONTROL_2_READ				0x2200
#define PLL1_DIVIDE_N1_READ			0x2300
#define PLL1_MULTIPLY_M1_READ 		0x2400
#define RX_GAIN_CONTROL_1_READ		0x2500
#define ADC_VOLUME_CONTROL_READ	0x2600
#define DAC_VOLUME_CONTROL_READ	0x2700
#define ANALOG_ATTENUATION_READ	0x2900

#define  DEFAULT_COMMAND			ADC_VOLUME_CONTROL_READ


/* These are the driver functions	*/
void 	Si3000CodecStart		(Si3000CodecHandle *pHandle, Si3000CodecRegisters *pRegisters);
int 		Si3000CodecWriteBusy	(Si3000CodecHandle *pHandle);
int 		Si3000CodecReadBusy	(Si3000CodecHandle *pHandle);
void 	Si3000CodecInit		(Si3000CodecHandle *pHandle, int *pBuffer);
void 	Si3000CodecRead		(Si3000CodecHandle *pHandle,int * data, int size);
void 	Si3000CodecWrite		(Si3000CodecHandle *pHandle,int * data, int size);
void 	Si3000CodecIOCtl		(Si3000CodecHandle *pHandle, int command, void * data);

#endif
