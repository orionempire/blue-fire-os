;**********************************************************
;	bluefire-os
;	Version: 00.00.02
;	Author: David Davidson
;	Name: stage2.asm
;	Last update: 2011-11-20
;***********************************************************

;***********************************************************
;	As in stage 1 we are still generating 16 bit code as
;	it will be running in real mode for now
;***********************************************************
bits 16

;***********************************************************
;	To keep things interesting we are going to use the other
;	addressing technique. We are going to set our segment registers
;	to 0 and have all addresses in the code prefix with 0x8000
;***********************************************************
org 0x8000

start:	jmp main	;jump over to first byte of executable code

;0x0A ascii for next line.  0x0D cursor to the beggining of the line.
cst_msg_loading 			DB 0x0D, 0x0A, "Hello world from Stage 2...", 0x0D, 0x0A, 0x00

main:
	cli				; clear interrupts

;***********************************************************
;	Set the segment registers back to 0x0000 as aformentioned
;	As we are now using the org directive to hard code the address offsets
;	The stack is staying at 000F:FFFF and should be empty at this point
;	but we set it again to be safe
;***********************************************************
	xor		ax, ax			; null segments
	mov		ds, ax
	mov		es, ax

	mov 	ax, 0x9000
	mov		ss, ax
	mov		sp, 0xFFFF
	sti						; restore interupts we need them before we can make any bios call

;***********************************************************
;	Display welcome message
;***********************************************************
	mov		si, cst_msg_loading
	call	print

end:
	jmp 	end				;stop here for now


;***********************************************************
;	print:
;	- Prints a string
;	assumes
;	- address of first character is in DS:SI
;	last character is /0 (NULL)
;***********************************************************
print:
	lodsb					; Load DS:SI into AL for proccessing
	or		al, al 			; this can only set the zero flag if AL is NULL
	jz		end_print		; if zero flag is set
	mov		ah, 0eh			; Bios paramaeter
	int		10h				; Bios function
	jmp		print			; repeat until null terminator is found.
end_print:
	ret
