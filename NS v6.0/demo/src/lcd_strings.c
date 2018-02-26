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

/* include files */
#include "..\h\lcd.h"
#include <stdlib.h> // Required for abs()

// Microchip (R) Logo for boot screen
#define LOGOSIZE 32
char logo_row0[LOGOSIZE] = {  0,  0,128,192,224,240,248,252,252,126, 62, 30, 63,255,255,255,255,127, 62, 62,124,248,240,224,252,194,189,149,169, 66, 60};
char logo_row1[LOGOSIZE] = {240,254,255,255,127, 31, 15,  3,  3,142,248,224,192,  0,  3,  7, 30,120,240,192,128,  0,  3,  7, 31, 63,127,255,254,252,240};
char logo_row2[LOGOSIZE] = { 15,  7,  1,  0,128,224,240,252,254,255,255,255,255,255,255,252,240,224,128,  0,  3,135,222,252,240,224,128,  0,  1,  7, 15};
char logo_row3[LOGOSIZE] = {  0,  0,  0,  3,  7, 15, 31, 63, 63,127,127,127,127,255,255,255,255,255,255,127,126,127, 63, 63, 31, 15,  7,  3,  0,  0,  0};
	
void LCD_displayVU(int* buffer, int bufferLen, int speech)
{
	static int vuCount = 5;
	static int oldValue = 0;
	static int clippedPersist = 0;
	int i;
	int maxamp = 0;
	int temp;
	
	vuCount++;
	if (vuCount > 5)
	{
		vuCount = 0;
		for(i = 0; i < bufferLen ; i++)
		{
			temp = abs(buffer[i]);
			if (temp > maxamp) // Find the max value in the buffer
			{
				maxamp = temp;
			}
		}
		// Scale maxamp to fit the display (0 to 64)
		maxamp = maxamp >> 9;
		if (maxamp < oldValue)
		{
			oldValue -= 3;
		}
		else
		{
			oldValue = maxamp;
		}
		
		// Display a mono bar hence the same value and 0
		LCD_displayBar(oldValue, oldValue, 0);
		
		// 60 represents ~ -0.5dB FS
		if (maxamp > 60)
		{
			LCD_displayClipped();
			clippedPersist = 10;
		}
		
		if (clippedPersist > 0)
		{
			clippedPersist --;	
			LCD_displayClipped();
		}
		if (speech)
		{
			LCD_displaySpeech();
		}
	}
}

void LCD_displayNS(int value, int onOff)
{
	char buffer[20] = "                    ";
	int tens = 0;
	if (onOff == 1)
	{
		buffer[0] = 'O';
		buffer[1] = 'n';
		buffer[2] = ' ';
		buffer[3] = ':';
		buffer[4] = ' ';
		buffer[5] = ' ';
		buffer[6] = '1';
		buffer[7] = '5';			
		buffer[8] = 'd';
		buffer[9] = 'B';
	}
	else
	{
		buffer[0] = 'O';
		buffer[1] = 'f';
		buffer[2] = 'f';
		buffer[3] = ':';
		buffer[4] = ' ';
		buffer[5] = ' ';
		buffer[6] = '1';
		buffer[7] = '5';			
		buffer[8] = 'd';
		buffer[9] = 'B';
	}
	
	while (value >= 10)
	{
		tens ++;
		value -= 10;
	}

	buffer[6] = (char) 0x30 + tens;
	buffer[7] = (char) 0x30 + value;
	
	LCD_writeRow(buffer,2);
}	

void LCD_displayLogo(int start)
{
	int x;
	int i;
		
	LCD_clearScreen();
	LCD_writeRow("       Microchip    ",0);
	LCD_writeRow("       Noise        ",1);
	LCD_writeRow("       Suppression  ",2);
	LCD_writeRow("       Demo v6.0    ",3);
	
	// Display the 1st line of the logo
	i = 0;
	LCD_setColumn(start, 0);
	for (x = start; x < start + 32; x++)
	{
		LCD_drawColumn(logo_row0[i]);
		i++;
	}
	
	// Display the 2nd line of the logo
	i = 0;
	LCD_setColumn(start, 1);
	for (x = start; x < start + LOGOSIZE; x++)
	{
		LCD_drawColumn(logo_row1[i]);
		i++;
	}
	
	// Display the 3rd line of the logo	
	i = 0;
	LCD_setColumn(start, 2);
	for (x = start; x < start + LOGOSIZE; x++)
	{
		LCD_drawColumn(logo_row2[i]);
		i++;
	}
	// Display the 4th line of the logo
	i = 0;
	LCD_setColumn(start, 3);
	for (x = start; x < start + LOGOSIZE; x++)
	{
		LCD_drawColumn(logo_row3[i]);
		i++;
	}
	//Pause on the boot screen
	LCD_pause();

	// Wipe the screen left to right
	for (i = 0; i < 121; i++)
	{
		LCD_setColumn(i, 0);
		LCD_drawColumn(0);
		LCD_setColumn(i, 1);
		LCD_drawColumn(0);
		LCD_setColumn(i, 2);
		LCD_drawColumn(0);
		LCD_setColumn(i, 3);
		LCD_drawColumn(0);
		LCD_pauseFast();
	}
}

	


