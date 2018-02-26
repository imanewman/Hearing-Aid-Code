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

#include "p33Exxxx.h"

//#define NO_DMA 1

#define WM8731_LINVOL   0x00
#define WM8731_RINVOL   0x01
#define WM8731_LOUT1V   0x02
#define WM8731_ROUT1V   0x03
#define WM8731_APANA    0x04
#define WM8731_APDIGI   0x05
#define WM8731_PWR      0x06
#define WM8731_IFACE    0x07
#define WM8731_SRATE    0x08
#define WM8731_ACTIVE   0x09
#define WM8731_RESET	0x0f

#define WM8731DRV_I2C_ADDR 0x34
#define WM8731DRV_I2CBAUD 272

#define WM8731_SAMPLING_FREQ_8K		8000u
#define WM8731_SAMPLING_FREQ_32K	32000u
#define WM8731_SAMPLING_FREQ_44K1	44100u
#define WM8731_SAMPLING_FREQ_48K	48000u
#define WM8731_SAMPLING_FREQ_88K2	88200u
#define WM8731_SAMPLING_FREQ_96K	96000u

#define WM8731_MCLK 12000000u
#define FRAME_SIZE 160

typedef struct __wm8731_state
{
	int index_samp;
	unsigned long mclk;
	unsigned long sampling_freq;
	int frame_size;
#ifdef NO_DMA
	int frame_ptr;
#endif
	int rx_status;
	int tx_status;
	int* in_buffer;
	int* out_buffer;

} wm8731_state;


int WM8731Exit();
int WM8731Init(wm8731_state* pwm8731_handle, int* in_buffer, int* out_buffer, unsigned long rate, int frame_size);
void WM8731Start();
void WM8731Control(wm8731_state* pwm8731_handle);
void WM8731Read(wm8731_state* pwm8731_handle);
void WM8731Write(wm8731_state* pwm8731_handle);
void WM8731WriteWait(wm8731_state* pwm8731_handle);
void i2c_init();
int i2c_write(int reg, int val);
int i2c_read(int reg);
void i2c_exit();
void i2c_add();
void dci_init();
void dci_exit();
void dci_add();
void dci_start();
#ifndef NO_DMA
void dma_exit();
void dma_init(wm8731_state* pwm8731_handle);
void dma_add(wm8731_state* pwm8731_handle);
void dma_start();
#endif

typedef struct _sampling_ctrl {
         long mclk;
         long rate;
         unsigned int fs;
         unsigned char sr:4;
         unsigned char bosr:1;
         unsigned char usb:1;
 } sampling_ctrl;

