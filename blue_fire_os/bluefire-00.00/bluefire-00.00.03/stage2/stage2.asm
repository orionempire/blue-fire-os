;**********************************************************
;	bluefire-os
;	Version: 00.00.02
;	Author: David Davidson
;	Name: stage2.asm
;	Last update: 2011-11-20
;	Purpose: Enable A20 Line, Load GDT, enable protect mode, enable paging.
;		Load a kernel to physical memory location 0x100000
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

%include "load_kernel.asm"

; ------ Mesages ------
;0x0A ascii for next line.  0x0D cursor to the beggining of the line.
cst_msg_loading 			DB 0x0D, 0x0A, "Loading kernel...", 0x0D, 0x0A, 0x00
cst_msg_unreal	 			DB 0x0D, 0x0A, "Welocme to unreal mode...", 0x0D, 0x0A, 0x00
; print one dot for every disk sector we load
cst_msg_progress 			DB ".", 0x00
; Fatal
cst_msg_fatal_failure  		DB 0x0D, 0x0A, "Fatal Failure.....", 0x0A, 0x00

;-------variables--------
; Currently set to 128 MB. If system memory changes it must be hard coded here
var_system_memory_amount	DD	0x8000000
;***********************************************************
; Global Descriptor Register (GDR)
;***********************************************************
gdtr:
	dw end_of_gdt - gdt_data - 1 	; limit (Size of GDT)
	dd gdt_data 			; base of GDT
;***********************************************************
; Global Descriptor Table (GDT)
;***********************************************************
gdt_data:
; Dummy descriptor			; 0x00
	DW 0					; Limit low (bytes 15:0)
	DW 0					; Base low (bytes 15:0)
	DB 0					; Base middle (bytes 23:16)
	DB 0					; Access byte (descriptor type)
	DB 0					; Flags, Limit middle (bytes 19:16)
	DB 0					; Base high (bytes 31:24)
; boot code:				; code descriptor
	DW 0xFFFF           	; Limit (bytes 15:0)
	DW 0                	; Base  (bytes 15:0)
	DB 0                	; Base  (bytes 23:16)
	DB 0b10011010        	; access  = Present, Ring 0, 1, Executable, Direction, Readable, Unaccessed (1,00,1,1110 -> 9A)
	DB 0b11001111        	; granularity = 4KB Granularity, 32bit segment size, reserved, reserved (AVL), Limit (bytes 19:16) (1,1,0,0,1111 - > CF)
	DB 0                	; Base  (bytes 31:24)
; boot data:	        	; data descriptor
	DW 0xFFFF           	; Limit (bytes 15:0)
	DW 0                	; Base  (bytes 15:0)
	DB 0                	; Base  (bytes 23:16)
	DB 0b10010010        	; access  = Present, Ring 0, 1, Not Executable, No Direction, Readable, Unaccessed (1,00,1,0010 -> 92)
	DB 0b11001111        	; granularity = 4KB Granularity, 32bit segment size, reserved, reserved (AVL), Limit (bytes 19:16) (1,1,0,0,1111 - > CF)
	DB 0                	; Base  (bytes 31:24)
end_of_gdt:

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
	mov		fs, ax
	mov		gs, ax

	mov 	ax, 0x9000
	mov		ss, ax
	mov		sp, 0xFFFF
	sti						; restore interupts we need them before we can make any bios call

;***********************************************************
;	Display welcome message
;***********************************************************
	mov		si, cst_msg_loading
	call	print

;***********************************************************
;	Allow more than 1MB to be accessed
;	Enable the A20 line
;	Enable Unreal mode
;
;***********************************************************
	call 	enable_a20_keyboard_out
	call 	enable_unreal_mode
	mov		si, cst_msg_unreal
	call	print


;***********************************************************
;	Load the kernel into memory
;	Ensure that floppy motor is off
;***********************************************************
	; load the the kernel into memory
	call 	load_kernel
	call 	turn_off_floppy_motor

	;interrupts will not be enabled again until protect mode
	cli
	call	enable_protect_mode
	jmp		0x8:protect_mode_start		; jump to end of code using Descripter Table Addressing

;***********************************************************
;		Halt Execution
;***********************************************************
fatal_failure:
	mov		si, cst_msg_fatal_failure
	call 	print
	mov		ah,	0x00	; Both bios call use paramater 0
	int		0x16		; BIOS 0x16 AH=0x00 -> await keypress
	int		0x19		; BIOS 0x19 AH=0x00 ->warm boot computer


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
	mov     al,0xAE			; enable the keyboard
	out     0x64,al

	call 	wait_for_clear_input
	popa
	sti
	ret

;***********************************************************
;	note- we must set es and ds so that the later copy above 1mb works
;	cached descriptors will persist even after the original descriptor is
;	restored.
;***********************************************************
enable_unreal_mode:
	cli
	push 	ds
	push	es

	lgdt 	[gdtr]        	; load GDT into GDTR
	mov  	eax, cr0		; switch to pmode by
	or 		al,1			; set pmode bit
	mov  	cr0, eax

	mov  	bx, 0x10		; select descriptor 2
	mov  	ds, bx			; 10h = 1 0000b
	mov  	es, bx			; 10h = 1 0000b

	and 	al,0xFE			; back to realmode
	mov  	cr0, eax		; by toggling bit again

	pop		es
	pop 	ds				; get back old segment
	sti
	ret

enable_protect_mode:
	mov	eax, cr0		; set bit 0 in cr0--enter pmode
	or	eax, 1
	mov	cr0, eax
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

;***********************************************************
; Turn off flopy motor
;***********************************************************
turn_off_floppy_motor:
	mov	dx, 0x03F2
	mov	al, 0x0C
	out	dx, al
	ret

;***********************************************************
; Protect Mode starts here
;***********************************************************
bits 32

%include "enable_paging.asm"

protect_mode_start:
;***********************************************************
;   Set segment registers
;***********************************************************
	mov	ax, 0x10	; set data segments to data selector
	mov	ds, ax
	mov	ss, ax
	mov	es, ax
	mov	esp, 9FFFFh		; Reset the stack, same place, new address mode

	call setup_and_enable_paging

;***********************************************************
;   Execute Kernel
;***********************************************************
	; Pass amount of system memory hard coded for now but
	; will be calulated later.
	push DWORD [var_system_memory_amount]
	; we have to save the address in case the kernel
	; executes a ret with the proper stack
	push fatal_bootstrap_return
	; jump to our kernel! It should be at 0xC0000000
	jmp	0x8:0xC0000000

;***********************************************************
;	If we make it here then kernel returned with out permission
;   Stop execution and halt
;***********************************************************
fatal_bootstrap_return:
	cli
	hlt

