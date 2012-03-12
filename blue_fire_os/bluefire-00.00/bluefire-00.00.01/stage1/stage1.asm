;**********************************************************
;	bluefire-os
;	Version: 00.00.01
;	Author: David Davidson
;	Name: stage1.asm
;	Last update: 2011-11-23
;	Purpose: First stage of our tutorial OS, called by BIOS, loads stage 2 into memory
;	Usage: Must be on sector 1 of floppy. Must be 512 Bytes and end with 0xAA55
;   Usage: Will be loaded to 0x7C00 (0000:7C00) by BIOS.
;***********************************************************

;***********************************************************
;	Since we are starting in real mode everything will be
;	in 16 bits. This directive tells the assembler to assemble
;	for a 16 bit architecture. So for mov it would generate a
;	opcode of B8. When we use "bits 32" the same instruction will
;	generate a op code of 66B8
;***********************************************************
bits 16

;***********************************************************
;	As mentioned the code gets loaded at 0x7C00 by bios convention
;	if we used the "org 0x7C00" directive, all of the segment registers
;	would stay at 0x0000 and the assembler would make all address calculations
;	with a delta of 0x7C00, so for example a label on the 3rd byte would
;	be referenced as goto 0x7C03 (0000:7C03).
;	Because this is a tutorial OS we want transparency and visibility
;	over convenience so we will set all segment registers to 0x07C0 so that
;	the address of the label stays 0x0003 but gets addressed like 07C0:0003 (0x7C03)
 ;***********************************************************

org 0

start: jmp main			;Jump to first bytes of executable code.

; 0x0A ascii for next line.  0x0D cursor to the beggining of the line.
msg_loading db 0x0D, 0x0A, "Hello World.", 0x0D, 0x0A, 0x00

main:
	cli					;disable interrupts

;***********************************************************
;	Set the segment registers to 0x07C0 as aformentioned
;	We cant direct address a segment register so we copy it to ax first
;	The stack is set to 000F:FFFF, below restricted memory
;***********************************************************
	mov		ax, 0x07C0
	mov 	ds, ax
	mov 	es, ax
	mov		fs, ax
	mov		gs, ax

	;Stack goes right under Video Ram which is at A0000-BFFFF
	mov		ax, 0x9000
	mov 	ss, ax
	mov		sp, 0xFFFF	;The stack grows down

	sti					;restore interrupts so we can make bios calls

;***********************************************************
;	Say Hi
;***********************************************************
	mov		si, msg_loading
	call	print
end:
	jmp 	end			;stop here for now

;***********************************************************
;	print:
;	- Prints a string
;	assumes
;	- address of first character is in DS:SI
;	last character is /0 (NULL)
;***********************************************************
print:
	lodsb				;Load DS:SI into AL for proccessing
	or		al, al 		;this can only set the zero flag if AL is NULL
	jz		end_print	;if zero flag is set
	mov		ah, 0eh		;Bios paramaeter
	int		10h			;Bios function
	jmp		print		;repeat until null terminator is found.
end_print:
	ret

;$ represents the current line of the program, $$ represents the first line of the program.
TIMES 510-($-$$) DB 0x90

;a idiom to fill the rest of the fill the upto 512 with no ops (90) and then 0xAA55
;in little endian format 0xAA55 is 0b1010101001010101 which is about as good a sentinel
;as you are going to get in binary.
DW 0xAA55
