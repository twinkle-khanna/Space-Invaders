; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

num EQU 0;	
	
    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB


;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutDec
        PUSH {LR, R0}
; ******Allocation Phase*******
        SUB SP, #8 ; Allocate num

; *****Access Phase*******
        CMP R0, #10 ;base case
        BLO basecase

        MOV R3, #0xa ;#10
        UDIV R2, R0, R3 ;divide r0 by 10

        MLS R1, R2, R3, R0 ;get remainder
        MOV R0, R2 ;put in n/10 as new input
        STR R1, [SP, #num] ;put val on stack

        BL LCD_OutDec ;call w/ next digit

        LDR R0, [SP, #num] ;get val from stack to print
        ADD R0, #0x30
        BL ST7735_OutChar ;print value
        B done

basecase
        ADD R0, #0x30
        BL ST7735_OutChar
done


;****Deallocation Phase*****   
        ADD SP, #8

        POP {LR, R0}


      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.01, range 0.00 to 9.99
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.00 "
;       R0=3,    then output "0.003 " error?
;       R0=89,   then output "0.89 "
;       R0=123,  then output "1.23 "
;       R0=999,  then output "9.99 "
;       R0>999,  then output "*.** "
; Invariables: This function must not permanently modify registers R4 to R11
; Lab 7 requirement is for at least one local variable on the stack with symbolic binding
LCD_OutFix
	MOV R1, #999 ;see if input is valid
	CMP R0, R1
	BHI invalidinput

;print first digit
	MOV R1, #100
	MOV R2, R0; copy input into r2, ex. 657
	UDIV R0, R1; R0 = R0/100, ex. 657/100 = 6
	MUL R1, R0; R1 = R0*100, ex 6*100 = 600
	SUBS R2, R1; R2 = R2-R1, ex. 657-600=57
	ADD R0, #0x30;
	PUSH {LR, R2}
	BL	ST7735_OutChar
	
;print '.'
	MOV	R0, #0x2E
	BL	ST7735_OutChar
	POP {LR, R2}
	
;print second digit, R2 holds remainder
	MOV R1, #10;
	UDIV R0, R2, R1; R0 = R2/10; ex. 57/10 = 5
	MUL R1, R0; R1 = R0*10, ex. 5*10 = 50
	SUBS R2, R1;  R2 = R2-R1, ex. 57=50 = 7
	ADD R0, #0x30;
	PUSH {LR, R2}
	BL	ST7735_OutChar
	POP {LR, R2}
	
;print third digit
	MOV R0, R2
	ADD R0, #0x30
	PUSH {LR, R2}
	BL	ST7735_OutChar
	POP {LR, R2}

	BX LR
	

invalidinput
	MOV	R0, #0x2A
	PUSH	{LR,R2}
	BL	ST7735_OutChar
	POP	{LR,R2}
	MOV	R0, #0x2E
	PUSH	{LR,R2}
	BL	ST7735_OutChar
	POP	{LR,R2}
	MOV	R0, #0x2A
	PUSH	{LR,R2}
	BL	ST7735_OutChar
	POP	{LR,R2}
	MOV	R0, #0x2A
	PUSH	{LR,R2}
	BL	ST7735_OutChar
	POP	{LR,R2}
	BX	LR
	ALIGN

;end
     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
