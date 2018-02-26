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

#include "..\h\WM8731CodecDrv.h"

wm8731_state* wm8731_handle_glbl;		// Used in interrupts only


/*static const*/ sampling_ctrl sampling_ctrl_tbl[] = {
	/* 48k */
	{12288000, 48000, 256, 0x0, 0x0, 0x0},
	{18432000, 48000, 384, 0x0, 0x1, 0x0},
	{12000000, 48000, 250, 0x0, 0x0, 0x1},
	
	/* 32k */
	{12288000, 32000, 384, 0x6, 0x0, 0x0},
	{18432000, 32000, 576, 0x6, 0x1, 0x0},
	{12000000, 32000, 375, 0x6, 0x0, 0x1},
	
	/* 8k */
	{12288000, 8000, 1536, 0x3, 0x0, 0x0},
	{18432000, 8000, 2304, 0x3, 0x1, 0x0},
	{11289600, 8000, 1408, 0xb, 0x0, 0x0},
	{16934400, 8000, 2112, 0xb, 0x1, 0x0},
	{12000000, 8000, 1500, 0x3, 0x0, 0x1},
	
	/* 96k */
	{12288000, 96000, 128, 0x7, 0x0, 0x0},
	{18432000, 96000, 192, 0x7, 0x1, 0x0},
	{12000000, 96000, 125, 0x7, 0x0, 0x1},
	
	/* 44.1k */
	{11289600, 44100, 256, 0x8, 0x0, 0x0},
	{16934400, 44100, 384, 0x8, 0x1, 0x0},
	{12000000, 44100, 272, 0x8, 0x1, 0x1},
	
	/* 88.2k */
	{11289600, 88200, 128, 0xf, 0x0, 0x0},
	{16934400, 88200, 192, 0xf, 0x1, 0x0},
	{12000000, 88200, 136, 0xf, 0x1, 0x1},
};

int WM8731Init(wm8731_state* pwm8731_handle, int* in_buffer, int* out_buffer, unsigned long rate, int frame_size)
{
	int index_samp = -1;

	pwm8731_handle->mclk = WM8731_MCLK;
	pwm8731_handle->sampling_freq = rate;

	while(!(sampling_ctrl_tbl[++index_samp].rate == rate && 
		  sampling_ctrl_tbl[index_samp].mclk == pwm8731_handle->mclk));
	pwm8731_handle->index_samp = index_samp;

	pwm8731_handle->frame_size = frame_size;
	pwm8731_handle->in_buffer = in_buffer;
	pwm8731_handle->out_buffer = out_buffer;

    return index_samp;
}


void WM8731Control(wm8731_state* pwm8731_handle)
{

	i2c_add();
	dci_add();

#ifndef NO_DMA
	dma_add(pwm8731_handle);
#endif

	wm8731_handle_glbl = pwm8731_handle;
	i2c_write(WM8731_PWR, 		0x80);
	i2c_write(WM8731_RESET, 	0x00);
	i2c_write(WM8731_ACTIVE, 	0x00);
	i2c_write(WM8731_APANA, 	0x14);	// disable boost, disable mute, disable bypass, select DAC, disable side tone, -6dB attn. 
	i2c_write(WM8731_APDIGI,	0x02); 
	i2c_write(WM8731_PWR, 		0x01);
	i2c_write(WM8731_IFACE, 	0x43);  // DSP mode, 16 bits, MSB on 1st BCLK rising edge, Master Mode.
	i2c_write(WM8731_SRATE, ((sampling_ctrl_tbl[pwm8731_handle->index_samp].sr << 2) | 
							 (sampling_ctrl_tbl[pwm8731_handle->index_samp].bosr << 1) | 
							 (sampling_ctrl_tbl[pwm8731_handle->index_samp].usb)));
	i2c_write(WM8731_LINVOL, 	0x17);
	i2c_write(WM8731_RINVOL, 	0x17);
	i2c_write(WM8731_LOUT1V, 	0x79);
	i2c_write(WM8731_ROUT1V, 	0x79);
	i2c_write(WM8731_ACTIVE, 	0x01);

}	

void WM8731Start()
{

#ifndef NO_DMA
	dma_start();
#endif

	dci_start();
}

void WM8731Read(wm8731_state* pwm8731_handle)
{
	while(!pwm8731_handle->rx_status);
	pwm8731_handle->rx_status = 0;
}	

void WM8731WriteWait(wm8731_state* pwm8731_handle)
{
	while(!pwm8731_handle->tx_status);
}	

void WM8731Write(wm8731_state* pwm8731_handle)
{
	while(!pwm8731_handle->tx_status);
	pwm8731_handle->tx_status = 0;
}	


int WM8731Exit()
{
	i2c_exit();

#ifndef NO_DMA
	dma_exit();
#endif

	dci_exit();

	return (0);
}

void i2c_init()
{
	I2C2CONbits.I2CEN = 0;
	
	I2C2CONbits.I2CSIDL	= 0;				/* I2C is running during idle	*/
	I2C2CONbits.IPMIEN = 0;					/* IPMI is disabled				*/
	I2C2CONbits.A10M = 0;					/* 7 bit slave address			*/
	I2C2CONbits.DISSLW = 1;					/* No slew control				*/
	I2C2CONbits.SMEN = 0;					/* No SMBus Enabled				*/
	
	I2C2BRG = WM8731DRV_I2CBAUD;			/* I2C Baud						*/
	I2C2CONbits.I2CEN = 1;					/* Enable the I2C module		*/
}

int i2c_write(int reg, int val)
{
	unsigned char byte1 = (reg << 1) | (val >> 8);
	unsigned char byte2 = val & 0xff;

	I2C2CONbits.SEN = 1;					/* Send the start condition 			*/
	while(I2C2CONbits.SEN == 1);			/* Wait for the condition to complete	*/
	
	I2C2TRN = WM8731DRV_I2C_ADDR;			/* Address of the WM8731				*/
	while(I2C2STATbits.TRSTAT);				/* Wait till this has been tranmitted	*/
	if (I2C2STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{										/* was recieved							*/
		I2C2CONbits.PEN = 1;
		while(I2C2CONbits.PEN);				/* Send the stop condition if a NACK	*/
		return(-1);							/* was received							*/		
	}

	I2C2TRN = byte1;						/* The address of the register			*/
	while(I2C2STATbits.TRSTAT);				/* Wait till this has been tranmitted	*/
	if (I2C2STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{										/* was recieved							*/
		I2C2CONbits.PEN = 1;
		while(I2C2CONbits.PEN);				/* Send the stop condition if a NACK	*/
		return(-1);							/* was received							*/		
	}
	
	I2C2TRN = byte2;						/* The value of the register			*/
	while(I2C2STATbits.TRSTAT);				/* Wait till this has been tranmitted	*/
	if (I2C2STATbits.ACKSTAT)				/* If this value is 1 then a NACK was	*/
	{										/* was recieved							*/
		I2C2CONbits.PEN = 1;
		while(I2C2CONbits.PEN);				/* Send the stop condition if a NACK	*/
		return(-1);							/* was received							*/		
	}
	
	I2C2CONbits.PEN = 1;
	while(I2C2CONbits.PEN);					/* Send the stop condition				*/
	
	return(1);
}

void i2c_exit()
{
	I2C2CONbits.I2CEN = 0;
}

void i2c_add()
{
	i2c_exit();
	
	i2c_init();
}

void dci_init()
{

	DCICON1bits.DCIEN = 0;

	ANSELCbits.ANSC4 = 0;
	ANSELAbits.ANSA6 = 0;
	ANSELAbits.ANSA7 = 0;

	TRISAbits.TRISA6 = 0;
	TRISAbits.TRISA7 = 0;
	TRISGbits.TRISG12 = 0;

	LATAbits.LATA6 = 0;
	LATAbits.LATA7 = 0;
	LATGbits.LATG12 = 1;
	
	TRISCbits.TRISC4 = 1;
	TRISDbits.TRISD10 = 1;
	TRISDbits.TRISD9 = 1;
	TRISDbits.TRISD0 = 0;

	//RPINR24bits.CSDIR = 52;		//RC4
	//RPINR24bits.CSCKR = 74;		//RD10
	RPINR24 = 0x4A34;
	//RPINR25bits.COFSR = 73;		//RD9
	RPINR25 = 0x49;
	//RPOR0bits.RP64R = 0b001011;	//RD0
	RPOR0 = 0xB;

	DCICON1 = 0x0720;
	DCICON2 = 0x002F;
	TSCON = 0x0003; 
	RSCON = 0x0003;

#ifdef NO_DMA
	_DCIIE = 1;
#else
	_DCIIE = 0;
#endif

	_DCIIP = 6;
	_DCIIF = 0;
	
}

void dci_start()
{
	DCICON1bits.DCIEN = 1;
}

void dci_exit()
{
	DCICON1bits.DCIEN = 0;
	_DCIIE = 0;
	_DCIIF = 0;
}

void dci_add()
{
	dci_init();

}

#ifdef NO_DMA

void __attribute__((__interrupt__, no_auto_psv)) _DCIInterrupt(void)
{
	_DCIIF = 0;
	wm8731_handle_glbl->in_buffer[wm8731_handle_glbl->frame_ptr] = RXBUF0;
	TXBUF0 = wm8731_handle_glbl->out_buffer[wm8731_handle_glbl->frame_ptr++];
	if(wm8731_handle_glbl->frame_ptr >= wm8731_handle_glbl->frame_size)
	{
		wm8731_handle_glbl->frame_ptr = 0;
		wm8731_handle_glbl->rx_status = 1;
		wm8731_handle_glbl->tx_status = 1;
	}
}

#else

void dma_init(wm8731_state* pwm8731_handle)
{
	DMA2CON = 0x0000;				
	DMA2CNT = (pwm8731_handle->frame_size)-1;												
	DMA2REQ = 0b00111100;					

	DMA2PAD = (unsigned int) &RXBUF0;
	DMA2STAL = (volatile unsigned int) (pwm8731_handle->in_buffer);
	DMA2STAH = 0;

	_DMA2IF  = 0;		
	_DMA2IE  = 1;

	DMA3CON = 0x2000;				
	DMA3CNT = (pwm8731_handle->frame_size)-1;												
	DMA3REQ = 0b00111100;					

	DMA3PAD = (unsigned int) &TXBUF0;
	DMA3STAL = (volatile unsigned int) (pwm8731_handle->out_buffer);
	DMA3STAH = 0;

	_DMA3IF  = 0;		
	_DMA3IE  = 1;	
}

void dma_start()
{
	DMA2CONbits.CHEN = 1;	
	DMA3CONbits.CHEN = 1;		
}

void dma_exit()
{
	DMA2CONbits.CHEN = 0;
	DMA3CONbits.CHEN = 0;	
}

void dma_add(wm8731_state* pwm8731_handle)
{
	dma_exit();
	dma_init(pwm8731_handle);
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA2Interrupt(void)
{
	_DMA2IF = 0;		
	wm8731_handle_glbl->rx_status = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
{
	_DMA3IF = 0;		
	wm8731_handle_glbl->tx_status = 1;
}


#endif


