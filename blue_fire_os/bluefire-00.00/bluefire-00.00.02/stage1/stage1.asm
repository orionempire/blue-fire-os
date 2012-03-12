;**********************************************************
;	bluefire-os
;	Version: 00.00.01
;	Author: David Davidson
;	Name: stage1.asm
;	Last update: 2011-11-23
;	Purpose: First stage of our tutorial OS, called by BIOS, loads stage 2 into memory
;	Usage: Must be on sector 1 of a floppy. Must be 512 Bytes and end with 0xAA55
;   Usage: Will be loaded to 0x7C00 (0000:7C00) by BIOS.
;	Usage: must have a stage 2 written raw starting at floppy sector 1400
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

start: jmp main			; Jump to first bytes of executable code.

; 0x0A ascii for next line.  0x0D cursor to the beggining of the line.
cst_msg_loading 			DB 0x0D, 0x0A, "Loading Stage 2...", 0x0D, 0x0A, 0x00
; print one dot for every disk sector we load
cst_msg_progress 			DB ".", 0x00
; Fatal
cst_msg_fatal_failure  		DB 0x0D, 0x0A, "Fatal Failure.....", 0x0A, 0x00

; The constants that represent where we put stage 2 on the floppy in floppy sectors.
cst_stage2_floppy_sector 	DW	2800
cst_stage2_sector_count		DW	40

; The constants that represent the structure of a floppy drive
cst_drive_number			DB	0		; We only support floppy drive A
cst_heads_per_cylinder		DW	2		; A floppy can only have one plater (height of data)
cst_sectors_per_track		DW	18		; A floppy has fixed amt of sectors on a track	(width of data)
cst_bytes_per_sector		DW 	512		; evrey sector has 512 bytes init (density of data)

;Variables used to return values from function LBACHS
calculated_sector 			DB 0x00
calculated_head   			DB 0x00
calculated_cylinder			DB 0x00

main:
	cli					; Disable interrupts

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
	mov		sp, 0xFFFF	; The stack grows down.

	sti					; Restore interrupts so we can make bios calls.

;***********************************************************
;	Display welcome message
;***********************************************************
	mov		si, cst_msg_loading
	call	print

load_stage_2:
	; load to 0800:0000(0x8000)
	mov		ax, 0x0800
	mov		es, ax
	mov		bx, 0x0000
	; load from (brackets mean contents of) ...
	mov		ax, [cst_stage2_floppy_sector]		; starting sector
	mov		cx, [cst_stage2_sector_count]		; sector count

	call	read_sectors

;***********************************************************
;	Jump to where we loaded our stage 2 code
;	retf will jump to address on stack wich was just pushed.
;***********************************************************
	push    WORD 0x0800		;(0800:0000)
	push    WORD 0x0000
	; jump to execute sector (0800:0000) where stage 2 was loaded
	retf

;***********************************************************
;		Halt Execution
;***********************************************************
fatal_failure:
	mov		si, cst_msg_fatal_failure
	call print
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
;	read_sectors:
;	-> AX 		=> Starting Sector
;	-> CX 		=> Amount of sectors to read
;	-> ES:BX 	=> Buffer to read to
;***********************************************************
read_sectors:
	.read_sectors_start:
		mov		di,0x0005						; Five retries in case of error
	.sector_loop:
		push 	ax								; Save the registers that we are going to use.
		push 	bx
		push 	cx
		call LBACHS								; Calculate what the starting sector is
		mov     ah, 0x02						; 02 Bios read sector function
		mov     al, 0x01 						; Read one sector at a time
		mov     ch, BYTE [calculated_cylinder]	; cylinder as returned by LBACHS
        mov     cl, BYTE [calculated_sector]	; sector as returned by LBACHS
        mov     dh, BYTE [calculated_head]		; head as returned by LBACHS
        mov     dl, BYTE [cst_drive_number]		; drive is always 0
		int     0x13							; invoke BIOS 0x13 AH=0x02
		jnc     .success                        ; test carry flag against a read error
		xor     ax, ax							; 0 is BIOS reset disk
		int     0x13							; invoke BIOS 0x13 AH=0x00
		dec     di								; decrement error counter
		pop     cx
		pop     bx
		pop     ax
		jnz     .sector_loop 					; if less the 5 tries, attempt to read again
		;int     0x18							; Else, 0x18 is a bios reset TODO -> replace
		call 	fatal_failure
	.success:
		mov     si, cst_msg_progress
		call    print
		pop     cx
		pop     bx
		pop     ax
		add     bx, WORD [cst_bytes_per_sector] ; queue next buffer
		inc     ax                              ; queue next sector
		loop    .read_sectors_start             ; Decreases CX and jums if not zero.
		ret

;***********************************************************
; 	LBACHS
;	Convert LBA to CHS
; 	AX -> LBA Address to convert
;
;	Returns <-
; 		calculated_sector 	= (logical sector mod sectors per track) + 1
; 		calculated_head   	= (logical sector / sectors per track) MOD number of heads
; 		calculated_cylinder = logical sector / (sectors per track * number of heads)
;
;***********************************************************
LBACHS:
    xor     dx, dx                              ; Zero out dx
    div     WORD [cst_sectors_per_track]		; Calculate sector against DX:AX store quotient->AX remainder->DX
    inc     dl                                  ; Adjust for sector 0
    mov     BYTE [calculated_sector], dl		; Store the result.
    xor     dx, dx                              ; Zero out dx
    div     WORD [cst_heads_per_cylinder]		; calculate sector against DX:AX store quotient->AX remainder->DX
    mov     BYTE [calculated_head], dl			; Store the result.
    mov     BYTE [calculated_cylinder], al		; Store the result.
	ret

;$ represents the current line of the program, $$ represents the first line of the program.
TIMES 510-($-$$) DB 0x90

; An idiom to fill the rest of the fill the upto 512 with no ops (90) and then 0xAA55
; in little endian format 0xAA55 is 0b1010101001010101 which is about as good a sentinel
; as you are going to get in binary.
DW 0xAA55

; --------- Used BIOS functions ---------
; INT 13h AH=00h: Reset Disk Drive
;	Parameters:
;		AH 	00h
;		DL 	Drive
;	Results:
;		CF 	Set on error
;
; INT 13h AH=02h: Read Sectors From Drive
;	Parameters:
;		AH 	02h
;		AL 	Sectors To Read Count
;		CH 	Track
;		CL 	Sector
;		DH 	Head
;		DL 	Drive
;		ES:BX 	Buffer Address Pointer
;	Results:
;		CF 	Set On Error, Clear If No Error
;		AH 	Return Code
;		AL 	Actual Sectors Read Count
;
; INT 16h AH=00h: Read Character
;	Results:
;		Will wait for keypress on keyboard
;
; INT 19h AH=00h: After POST this interrupt is used by BIOS to load the operating system.
;	Results:
;		Computer will reboot.
