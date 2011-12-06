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
cst_msg_loading 			DB 0x0D, 0x0A, "Loading Stage 2...", 0x0D, 0x0A, 0x00


;***********************************************************
; Global Descriptor Table (GDT)
;***********************************************************

gdt_data:
; Dummy descriptor			; 0x00
	dw 0					; Limit 15:0
	dw 0					; Base 15:0
	db 0					; Base 23:16
	db 0					; Access byte (descriptor type)
	db 0					; Limits 19:16, Flags
	db 0					; Base 31:24

; kernel code:				; code descriptor
	dw 0FFFFh           	; limit low
	dw 0                	; base low
	db 0                	; base middle
	db 10011010b        	; access
	db 11001111b        	; granularity
	db 0                	; base high

; kernel data:	        	; data descriptor
	dw 0FFFFh           	; limit low (Same as code)
	dw 0                	; base low
	db 0                	; base middle
	db 10010010b        	; access
	db 11001111b        	; granularity
	db 0                	; base high

; user code:	        	; code descriptor
	dw 0FFFFh           	; limit low
	dw 0                	; base low
	db 0                	; base middle
	db 11111010b        	; access
	db 11001111b        	; granularity
	db 0                	; base high

; user data:	         	; data descriptor
	dw 0FFFFh           	; limit low (Same as code)
	dw 0                	; base low
	db 0                	; base middle
	db 11110010b        	; access
	db 11001111b        	; granularity
	db 0                	; base high
end_of_gdt:
toc:
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data 			; base of GDT

main:
	cli						; clear interrupts

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
;	Enable the A20 line
;	Allows more than 1MB to be accessed
;***********************************************************
	call enable_a20_keyboard_out

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

;***********************************************************
;		enable_a20_keyboard_out:
;		- enable the a20 line
;		- Uses the keyboard controller out port method
;***********************************************************
enable_a20_keyboard_out:
	cli						;stop interupts
	pusha					;save state of all registers
	call 	wait_for_clear_input
	mov     al,0xAD
	out     0x64,al			; disable the keyboard
	call 	wait_for_clear_input

	mov     al,0xD0
	out     0x64,al			; tell controller to read key board output port
	call    wait_for_clear_output

	in      al,0x60
	push    eax				; get output port data and store it
	call 	wait_for_clear_input

	mov     al,0xD1
	out     0x64,al			; tell controller to write to output port
	call 	wait_for_clear_input

	pop     eax
	or      al,2			; set bit 1 (enable a20)
	out     0x60,al			; write out data back to the output port

	call 	wait_for_clear_input
	mov     al,0xAE		; enable the keyboard
	out     0x64,al

	call 	wait_for_clear_input
	popa
	sti
	ret

wait_for_clear_input:	;wait for a clear input buffer
	in      al,0x64
	test    al,2
	jnz     wait_for_clear_input
	ret

wait_for_clear_output:	;wait for a clear output buffer
	in      al,0x64
	test    al,1
	jz      wait_for_clear_output
	ret
