/**********************************************************************
* © 2007 Microchip Technology Inc.
*
* FileName:        AT25F4096Drv.h
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

#ifndef __SST25LF040ADRV_H__
#define __SST25LF040ADRV_H__

#include <p33Fxxxx.h>

#define SST25LF040ADRV_FCY							40000000
#define SST25LF040ADRV_SPI1_PRIMARY_PRESCALE		0x2
#define SST25LF040ADRV_SPI1_SECONDARY_PRESCALE		0x6
#define SST25LF040ADRV_BUFFER_SIZE					128
#define SST25LF040ADRV_LAST_ADDRESS					0x7FFFF
#define SST25LF040ADRV_TBP_PERIOD					0x320		/* Timer period for 20usec	*/

#define FLASH_VDD_REGULATOR	1

#if (FLASH_VDD_REGULATOR)
	#define REGULATOR_CONTROL_TRIS		TRISBbits.TRISB5
	#define REGULATOR_CONTROL_ANPCFG	AD1PCFGLbits.PCFG5
	#define REGULATOR_CONTROL_LAT		LATBbits.LATB5
	#define REGULATOR_WAKE_UP_DELAY	0x2000
#endif


typedef union u_SST25LF040Address
{
	char addressByte[4];
	long fullAddress;
}SST25LF040Address;	

typedef struct s_SST25LF040AHandle
{
	volatile char * buffer;
	volatile int statusWord;
	volatile int statusFlag;
	volatile int dataCount;
	volatile int dataIndex;
	volatile char* readData;
	SST25LF040Address writeAddress;
	
}SST25LF040AHandle;	

#define SST25LF040ADRV_CLR_STATE_COUNTER 0xFFF8
#define SST25LF040ADRV_GET_STATE_COUNTER 0x0007

#define SST25LF040ADRV_SET_RW_DONE		0x0008
#define SST25LF040ADRV_CLR_RW_DONE 		0xFFF7
#define SST25LF040ADRV_GET_RW_DONE 	0x0008

#define SST25LF040ADRV_SET_IS_READING	0x0010
#define SST25LF040ADRV_CLR_IS_READING	0xFFEF
#define SST25LF040ADRV_GET_IS_READING	0x0010

#define SST25LF040ADRV_SET_IS_WRITING	0x0020
#define SST25LF040ADRV_CLR_IS_WRITING	0xFFDF
#define SST25LF040ADRV_GET_IS_WRITING	0x0020

/* These are the commands that can be used with 
 * ioctl functions */
 
#define SST25LF040ADRV_CHIP_ERASE			0x1
#define SST25LF040ADRV_READ_FLASH_MFG_ID	0x2
#define SST25LF040ADRV_READ_STATUS			0x3
#define SST25LF040ADRV_WRITE_ENABLE			0x4
#define SST25LF040ADRV_WRITE_STATUS		0x5
#define SST25LF040ADRV_WRITE_DISABLE		0x6
#define SST25LF040ADRV_SECTOR_ERASE		0x7
#define SST25LF040ADRV_BYTE_PROGRAM		0x8
#define SST25LF040ADRV_BLOCK_ERASE			0x9
#define SST25LF040ADRV_WRITE_STATUS_ENABLE			0xA



#define SST25LF040A_PROGRAM_CODE				0xAF
#define SST25LF040A_BYTE_PROGRAM				0x2
#define SST25LF040A_BLOCK_ERASE_CODE			0x52
#define SST25LF040A_READ_CODE					0x3
#define SST25LF040A_CHIP_ERASE_CODE				0x60
#define SST25LF040A_READ_CHIPID_CODE				0x90
#define SST25LF040A_READ_STATUS_CODE			0x5
#define SST25LF040A_WRITE_ENABLE_CODE			0x6
#define SST25LF040A_WRITE_DISABLE_CODE			0x4
#define SST25LF040A_SECTOR_ERASE_CODE			0x20
#define SST25LF040A_ENABLE_WRITE_STATUS_CODE	0x50
#define SST25LF040A_WRITE_STATUS_CODE			0x1


#define SST25LF040A_CHIP_SELECT_TRIS			TRISBbits.TRISB2 
#define SST25LF040A_CHIP_SELECT				LATBbits.LATB2 	
#define SST25LF040A_CHIP_SELECT_ANPORT		AD1PCFGLbits.PCFG2



void 	SST25LF040AInit		(SST25LF040AHandle* pHandle, char * buffer);
void 	SST25LF040AWrite		(SST25LF040AHandle* pHandle, long address,  char* data,   int count);
void		SST25LF040ARead		(SST25LF040AHandle* pHandle, long address,  char* data,int count);
void 	SST25LF040AIOCtl		(SST25LF040AHandle* pHandle,int command, void *data);
int		SST25LF040AIsBusy	(SST25LF040AHandle* pHandle);
void 	SST25LF040AStart		(SST25LF040AHandle* pHandle);



#endif
