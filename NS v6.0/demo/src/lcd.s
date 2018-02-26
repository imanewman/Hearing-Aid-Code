/*********************************************************************
;                                                                    *
;                       Software License Agreement                   *
;                                                                    *
;   The software supplied herewith by Microchip Technology           *
;   Incorporated (the "Company") for its dsPIC controller            *
;   is intended and supplied to you, the Company's customer,         *
;   for use solely and exclusively on Microchip dsPIC                *
;   products. The software is owned by the Company and/or its        *
;   supplier, and is protected under applicable copyright laws. All  *
;   rights are reserved. Any use in violation of the foregoing       *
;   restrictions may subject the user to criminal sanctions under    *
;   applicable laws, as well as to civil liability for the breach of *
;   the terms and conditions of this license.                        *
;                                                                    *
;   THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
;   WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
;   BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
;   FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
;   COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
;   INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
;                                                                    *
;********************************************************************/
/******************************************************************************
;                                                                             *
;  Filename:               lcd.s                                              *
;  Description:		   Assembly routines for accessing the LCD	     		  *
;*****************************************************************************/


    /* include files */
    
    .include "p33Fxxxx.inc"
    
    /* global symbols */
    .global _LCD_initSPI
    .global _LCD_writeRow
	.global _LCD_displayBar
	.global _LCD_displayClipped
	.global _LCD_displaySpeech
	.global _LCD_displayBox
	.global _LCD_clearScreen
	.global _LCD_setColumn
	.global _LCD_drawColumn
	.global _LCD_pause		
	.global _LCD_pauseFast
				
    /* equates */
    .equ CHAR_COUNT, 4
    .equ LCD_COLUMNS, 20
    .equ NIBBLE, 4
    .equ NIBBLE_MASK, 0x000F

    /* code */
    .section .text

;####################################################################
;#
;#		Setup SPI port.
;#
;####################################################################

_LCD_initSPI:

    mov     #0x0200,        w0          ;
    mov     w0,             LATG        ; Set SS high
    mov     #0xF08F,        w0          ;
    mov     w0,             TRISG       ;

    clr     SPI2STAT                    ; Reset status register for SPI-2
    mov     #0x0010,        w0          ; Set frequency divider:
.ifdef __33FJ256GP710
    mov     w0,             SPI2CON1    ; Master Clk = Fosc/256
    bclr    SPI2CON1,       #SMP        ; Data sampled at middle of output time
    bclr    SPI2CON1,       #CKE        ; Data clocked out on rising edge
    bclr    SPI2CON1,       #CKP        ; CLK idle is low
    bset    SPI2CON1,       #MSTEN      ; Set Master mode for SPI-2
.else
    mov     w0,             SPI2CON     ; Master Clk = Fosc/256
    bclr    SPI2CON,        #SMP        ; Data sampled at middle of output time
    bclr    SPI2CON,        #CKE        ; Data clocked out on rising edge
    bclr    SPI2CON,        #CKP        ; CLK idle is low
    bset    SPI2CON,        #MSTEN      ; Set Master mode for SPI-2
.endif

    bclr    SPI2STAT,       #SPIROV     ; Reset Overflow error bit
    bset    SPI2STAT,       #SPIEN      ; Enable SPI-2
    bclr    LATG,           #LATG9      ; Enable SPI reception by controller

    return                              ; End of 'SPI_Init' procedure


;####################################################################
;#
;#		Write byte from W0 to LCD Controller.
;#
;####################################################################

WriteChar_LCD:
    push    w1
    mov     w0,             w1          ; Temporary saving
    mov     #0xA8,          w0          ; Send command to LCD
    call    WriteByte_LCD               ;

    mov     w1,             w0          ; Restore sent symbol
    call    WriteByte_LCD               ; Display character on LCD
    pop     w1
    return                              ; End of 'WriteChar_LCD' procedure


;####################################################################
;#
;#      WriteByte_LCD (int)
;#		Write byte from W0 to LCD Controller.
;#
;####################################################################

WriteByte_LCD:
    push    w1                          ; Save content of W1
    bclr    SPI2STAT,       #SPIROV     ;

BufferIsBusy1:
    btsc    SPI2STAT,       #SPITBF     ; Wait for buffer becomes available
    bra     BufferIsBusy1               ;

    mov     SPI2BUF,        w1          ; Read and discard input
    mov     w0,             SPI2BUF     ; Write byte to send
    
    pop     w1                          ; Restore content of W1
    return                              ; End of 'SPI_Init' procedure


;####################################################################
;#
;#  _LCD_writeRow (char *string, int row)
;#
;####################################################################
_LCD_writeRow:
    mov     w0, w4      ; w4 is the string pointer
    mov     w1, w5      ; w5 is the target LCD row

    mov     #0xC5,              w0      ; Send CHAR_POSITION command
    call    WriteByte_LCD               ; 
    mov     #0,                 w0      ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     w5,                 w0      ; Set Y-position of cursor
    call    WriteByte_LCD               ;
    clr     w0                          ; Prepare byte operations
    do      #LCD_COLUMNS-1, LCD_Lab6    ; Display message in loop
    mov.b   [w4++],             w0      ; Read next symbol from RAM
    call    WriteChar_LCD               ; Send it to LCD
LCD_Lab6:                               ;
    nop                                 ; End of message loop

    return

;####################################################################
;#
;#  _LCD_displayBar (leftvalue, rightvalue, mono/stereo)
;#
;####################################################################
_LCD_displayBar:

	mov 	w2, w5						; save the stereo flag
	mov 	w0, w2						; save the leftvalue
	mov 	w1, w3						; save the rightvalue

	call _LCD_displayBox
	
	; Do the left bar
	mov w2, w1							; load value
	mov #4, w4							; load bar
	call bar
	
	; Do the right bar
	mov w3, w1							; load value
	mov #2, w4							; load bar
	call bar
	
	; Do the fill bar if mono
	cp0 w5
	bra nz, lcdexit
	mov w3, w1							; load value
	mov #3, w4							; load bar
	call bar
	
lcdexit:	
return

;local function
bar:
			
    mov     #0xd7, w0	                ; Send PIX_POSITION command
    call    WriteByte_LCD               ; 
    mov     #0, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     w4, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
	
	
    mov     #0xda, w0	                ; Send PIX_LINE command
    call    WriteByte_LCD               ; 
    mov     w1, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     w4, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;

	nop
	     
return                                  ; End of 'DisplayBar' procedure

;####################################################################
;#
;#  _LCD_displayClipped ()
;#
;####################################################################
_LCD_displayClipped:

	mov 	w0, w1						; save the after value
	
	mov     #0xC5,              w0      ; Send CHAR_POSITION command
    call    WriteByte_LCD               ; 
    mov     #13,                 w0      ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #3,                 w0      ; Set Y-position of cursor
    call    WriteByte_LCD               ;

    mov     #'C',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD               ;
    mov     #'L',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD
    mov     #'I',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD
    mov     #'P',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD
    mov     #'P',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD
	mov     #'E',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD
	mov     #'D',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD

    nop
return  

;####################################################################
;#
;#  _LCD_displayClipped ()
;#
;####################################################################
_LCD_displaySpeech:

	mov 	w0, w1						; save the after value
	
	mov     #0xC5,              w0      ; Send CHAR_POSITION command
    call    WriteByte_LCD               ; 
    mov     #11,                 w0      ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #3,                 w0      ; Set Y-position of cursor
    call    WriteByte_LCD               ;

    mov     #'*',                 w0      ; Set Y-position of cursor
    call    WriteChar_LCD               ;
    nop
return  

;####################################################################
;#
;#  _LCD_displayBox ()
;#
;####################################################################
_LCD_displayBox:
    mov     #0xa9, w0			        ; Clear LCD row
    call    WriteByte_LCD        
    mov     #3, w0						
    call    WriteByte_LCD        

    ; Draw the box
    ; Set the first position
    mov     #0xd7, w0	                ; Send PIX_POSITION command
    call    WriteByte_LCD               ; 
    mov     #0, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #0, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
	
	mov     #0xda, w0	                ; Send PIX_LINE command
    call    WriteByte_LCD               ; 
    mov     #64, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #0, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;

	mov     #0xda, w0	                ; Send PIX_LINE command
    call    WriteByte_LCD               ; 
    mov     #64, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #6, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;

	mov     #0xda, w0	                ; Send PIX_LINE command
    call    WriteByte_LCD               ; 
    mov     #0, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #6, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;

	mov     #0xda, w0	                ; Send PIX_LINE command
    call    WriteByte_LCD               ; 
    mov     #0, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #0, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
    
    ; Display Upper limit dot (~-1dB)
    mov     #0xd8, w0	                ; Send PIX_ON command
    call    WriteByte_LCD               ; 
    mov     #60, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #5, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
    
    mov     #0xd8, w0	                ; Send PIX_ON command
    call    WriteByte_LCD               ; 
    mov     #60, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #1, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
    ; Display Lower limit dot (~-14dB)
    mov     #0xd8, w0	                ; Send PIX_ON command
    call    WriteByte_LCD               ; 
    mov     #12, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #5, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;

    mov     #0xd8, w0	                ; Send PIX_ON command
    call    WriteByte_LCD               ; 
    mov     #12, w0					    ; Set X-position of cursor
    call    WriteByte_LCD               ;
    mov     #1, w0					    ; Set Y-position of cursor
    call    WriteByte_LCD               ;
    
    nop
return

;####################################################################
;#
;#  _LCD_clearScreen ()
;#
;####################################################################

_LCD_clearScreen:
    mov     #0x82,          w0         ; Clear LCD display
    call    WriteByte_LCD              ; 

	return
;####################################################################
;#
;#  _LCD_setColumn (int x, int y)
;#
;####################################################################

_LCD_setColumn:
	mov w1, w2
	mov w0, w1
	; w1 = col
	; w2 = row
	; move to the correct col/row

	mov 	#0xdb, w0
	call    WriteByte_LCD
	mov 	w1, w0
	call    WriteByte_LCD
	mov		w2, w0
	call    WriteByte_LCD
return

;####################################################################
;#
;#  _LCD_drawColumn (int data)
;#
;####################################################################
	
_LCD_drawColumn:
	mov w0, w1
	;w1 = data
	; write data to col and inc col pointer.
	mov		#0xbc, w0
	call  	WriteByte_LCD
	mov 	w1, w0
	call  	WriteByte_LCD
	return
	
_LCD_pause:
    MOV  #0x0200, W1
lcdouter1: 
    MOV  #0xFFFF, W0
lcdinner1:    
    DEC  W0, W0
    BRA  NZ, lcdinner1
    DEC  W1, W1
    BRA  NZ, lcdouter1   
    return	

_LCD_pauseFast:
    MOV  #0x020, W1
lcdouter2: 
    MOV  #0x0500, W0
lcdinner2:    
    DEC  W0, W0
    BRA  NZ, lcdinner2
    DEC  W1, W1
    BRA  NZ, lcdouter2   
    return	
.end

